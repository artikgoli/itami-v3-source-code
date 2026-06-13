#pragma once
#include "../entity_cache/entity_cache.h"
#include "../../utilities/memory/memory.h"
#include "../../../valve/math/math.h"
#include <atomic>
#include <mutex>
#include <thread>
#include <bit>

struct smoke_voxel_t
{
	vector3d world;
	uint8_t  alpha;
};

struct smoke_data_t
{
	std::vector<smoke_voxel_t> voxels;
	bool valid = false;
};

struct smoke_frame_t
{
	std::vector<smoke_data_t> smokes;
};

class c_smoke_system
{
public:
	void init( );
	void update_bases( float global_time );
	bool line_through_smoke( const vector3d& from, const vector3d& to ) const;

private:
	void voxel_loop( );

	static uint32_t compact_1by2( uint32_t v )
	{
		v &= 0x09249249u;
		v = ( v ^ ( v >> 2  ) ) & 0x030C30C3u;
		v = ( v ^ ( v >> 4  ) ) & 0x0300F00Fu;
		v = ( v ^ ( v >> 8  ) ) & 0x00000FFFu;
		return v;
	}

	smoke_frame_t          buf_[ 2 ];
	std::atomic<int>       front_{ 0 };
	std::vector<uintptr_t> smoke_bases_;
	std::mutex             bases_mutex_;
};

inline void c_smoke_system::init( )
{
	std::thread( [ this ]( ) { voxel_loop( ); } ).detach( );
}

inline void c_smoke_system::update_bases( float global_time )
{
	static float last_update = 0.f;
	if ( global_time - last_update < 1.5f )
		return;
	last_update = global_time;

	static const auto tick_off = schema_system->map_offsets[ FNV1A::HashConst( "C_SmokeGrenadeProjectile->m_nSmokeEffectTickBegin" ) ];

	static const auto  embedded_off  = schema_system->map_offsets[ FNV1A::HashConst( "C_SmokeGrenadeProjectile->m_VoxelFrameData" ) ];
	constexpr uint64_t state_ptr_off = 0x70;

	const auto entities = entity_cache->get_entities( );

	std::vector<uintptr_t> new_bases;

	for ( const auto& e : entities )
	{
		if ( !e.entity )
			continue;

		const uintptr_t base = reinterpret_cast<uintptr_t>( e.entity );

		const int tick = memory->read<int>( base + tick_off );
		if ( tick <= 0 )
			continue;

		const uintptr_t state_ptr = memory->read<uintptr_t>( base + embedded_off + state_ptr_off );
		if ( state_ptr < 0x10000 )
			continue;

		new_bases.push_back( base );
	}

	{
		std::lock_guard lock( bases_mutex_ );
		smoke_bases_ = std::move( new_bases );
	}
}

inline void c_smoke_system::voxel_loop( )
{
	static const auto  embedded_off    = schema_system->map_offsets[ FNV1A::HashConst( "C_SmokeGrenadeProjectile->m_VoxelFrameData" ) ];
	constexpr uint64_t state_ptr_off   = 0x70;
	constexpr uint64_t rgba_ptr_off    = 0xE0;
	constexpr uint64_t center_off      = 0xE8;
	constexpr uint64_t frame_index_off = 0x100;
	constexpr uint64_t bitset_base     = 4104; // todo: reverse this, i moge to ukrasc bo temu samemu gosciowi pomagalem z reversem hitboxow wiec elo

	while ( true )
	{
		std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) );

		std::vector<uintptr_t> bases;
		{
			std::lock_guard lock( bases_mutex_ );
			bases = smoke_bases_;
		}

		const int      back  = 1 - front_.load( );
		smoke_frame_t& frame = buf_[ back ];
		frame.smokes.clear( );

		for ( const uintptr_t base : bases )
		{
			const uintptr_t wrapper   = base + embedded_off;
			const uintptr_t state_ptr = memory->read<uintptr_t>( wrapper + state_ptr_off );
			const uintptr_t rgba_ptr  = memory->read<uintptr_t>( wrapper + rgba_ptr_off );
			const vector3d  center    = memory->read<vector3d>( wrapper + center_off );
			const int       frame_idx = memory->read<int>( wrapper + frame_index_off );

			if ( !state_ptr || !rgba_ptr || center.is_zero( ) )
				continue;

			if ( frame_idx < 0 || frame_idx > 255 )
				continue;

			uint64_t bitset[ 512 ] {};
			if ( !memory->read_raw( state_ptr + bitset_base + ( uintptr_t ) frame_idx * 4096, bitset, sizeof( bitset ) ) )
				continue;

			uint8_t rgba[ 32768 * 4 ] {};
			if ( !memory->read_raw( rgba_ptr, rgba, sizeof( rgba ) ) )
				continue;

			smoke_data_t sd {};
			sd.voxels.reserve( 1024 );

			for ( uint32_t w = 0; w < 512; ++w )
			{
				uint64_t bits = bitset[ w ];
				while ( bits )
				{
					const uint32_t tz     = ( uint32_t ) std::countr_zero( bits );
					bits &= bits - 1;

					const uint32_t morton = ( w << 6 ) | tz;
					const uint32_t vx     = compact_1by2( morton );
					const uint32_t vy     = compact_1by2( morton >> 1 );
					const uint32_t vz     = compact_1by2( morton >> 2 );

					const int     lin = ( int ) vx + 32 * ( ( int ) vy + 32 * ( int ) vz );
					const uint8_t a   = rgba[ ( size_t ) lin * 4 + 3 ];
					if ( a <= 4 )
						continue;

					sd.voxels.push_back( {
						{
							center.x + ( float( vx ) - 16.f ) * 20.f,
							center.y + ( float( vy ) - 16.f ) * 20.f,
							center.z + ( float( vz ) - 16.f ) * 20.f
						},
						a
					} );
				}
			}

			if ( sd.voxels.empty( ) )
				continue;

			sd.valid = true;
			frame.smokes.emplace_back( std::move( sd ) );
		}

		front_.store( back );
	}
}

inline bool c_smoke_system::line_through_smoke( const vector3d& from, const vector3d& to ) const
{
	constexpr float half       = 10.f;
	constexpr float extinction = 6.f; // wymarly pasty z githaba 5000 lat temu before kirk
	constexpr float limit   = 12.f;

	const float seg_len = ( to - from ).length( );
	if ( seg_len < 1e-6f )
		return false;

	const smoke_frame_t& frame = buf_[ front_.load( ) ];

	for ( const auto& s : frame.smokes )
	{
		if ( !s.valid )
			continue;

		float tau = 0.f;

		for ( const auto& v : s.voxels )
		{
			const vector3d bmin = { v.world.x - half, v.world.y - half, v.world.z - half };
			const vector3d bmax = { v.world.x + half, v.world.y + half, v.world.z + half };

			float t0, t1;
			if ( !math::segment_aabb_intersect( from, to, bmin, bmax, t0, t1 ) )
				continue;

			const float inside_len = ( t1 - t0 ) * seg_len;
			if ( inside_len <= 0.f )
				continue;

			tau += extinction * ( v.alpha / 255.f ) * ( inside_len / 20.f );

			if ( tau >= limit )
				return true;
		}
	}

	return false;
}

inline auto smoke_system = std::make_unique<c_smoke_system>( );
