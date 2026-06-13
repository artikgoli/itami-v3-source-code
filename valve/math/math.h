#pragma once
#include "vector3d.h"
#include "matrix3x4.h"

class sha1
{
public:
	void reset( )
	{
		this->m_state[ 0 ] = 0x67452301;
		this->m_state[ 1 ] = 0xEFCDAB89;
		this->m_state[ 2 ] = 0x98BADCFE;
		this->m_state[ 3 ] = 0x10325476;
		this->m_state[ 4 ] = 0xC3D2E1F0;
		this->m_count = 0;
	}

	void update( const void* data, std::size_t len )
	{
		const auto* bytes = static_cast< const std::uint8_t* >( data );
		auto index = static_cast< std::size_t >( this->m_count & 63 );

		this->m_count += len;

		std::size_t i { 0 };

		if ( index )
		{
			auto part_len = 64 - index;

			if ( len >= part_len )
			{
				std::memcpy( this->m_buffer + index, bytes, part_len );
				this->transform( this->m_buffer );
				i = part_len;
			}
			else
			{
				std::memcpy( this->m_buffer + index, bytes, len );
				return;
			}
		}

		for ( ; i + 64 <= len; i += 64 )
		{
			this->transform( bytes + i );
		}

		if ( i < len )
		{
			std::memcpy( this->m_buffer, bytes + i, len - i );
		}
	}

	void final( )
	{
		std::uint8_t padding[ 64 ] {};
		padding[ 0 ] = 0x80;

		auto index = static_cast< std::size_t >( this->m_count & 63 );
		auto pad_len = ( index < 56 ) ? ( 56 - index ) : ( 120 - index );
		auto bit_count = this->m_count * 8;

		this->update( padding, pad_len );

		std::uint8_t bits[ 8 ] { };

		for ( int i = 0; i < 8; ++i )
		{
			bits[ 7 - i ] = static_cast< std::uint8_t >( bit_count >> ( i * 8 ) );
		}

		this->update( bits, 8 );

		for ( int i = 0; i < 5; ++i )
		{
			this->m_digest[ i * 4 + 0 ] = static_cast< std::uint8_t >( this->m_state[ i ] >> 24 );
			this->m_digest[ i * 4 + 1 ] = static_cast< std::uint8_t >( this->m_state[ i ] >> 16 );
			this->m_digest[ i * 4 + 2 ] = static_cast< std::uint8_t >( this->m_state[ i ] >> 8 );
			this->m_digest[ i * 4 + 3 ] = static_cast< std::uint8_t >( this->m_state[ i ] );
		}
	}

	std::uint32_t get_first_uint32( ) const
	{
		std::uint32_t result;
		std::memcpy( &result, this->m_digest, sizeof( result ) );
		return result;
	}

private:
	static std::uint32_t rotl( std::uint32_t v, int n )
	{
		return ( v << n ) | ( v >> ( 32 - n ) );
	}

	void transform( const std::uint8_t* block )
	{
		std::uint32_t w[ 80 ] { };

		for ( int i = 0; i < 16; ++i )
		{
			w[ i ] = static_cast< std::uint32_t >( block[ i * 4 ] ) << 24 | static_cast< std::uint32_t >( block[ i * 4 + 1 ] ) << 16 | static_cast< std::uint32_t >( block[ i * 4 + 2 ] ) << 8 | static_cast< std::uint32_t >( block[ i * 4 + 3 ] );
		}

		for ( int i = 16; i < 80; ++i )
		{
			w[ i ] = this->rotl( w[ i - 3 ] ^ w[ i - 8 ] ^ w[ i - 14 ] ^ w[ i - 16 ], 1 );
		}

		auto a = this->m_state[ 0 ];
		auto b = this->m_state[ 1 ];
		auto c = this->m_state[ 2 ];
		auto d = this->m_state[ 3 ];
		auto e = this->m_state[ 4 ];

		for ( int i = 0; i < 80; ++i )
		{
			std::uint32_t f, k;

			if ( i < 20 )
			{
				f = ( b & c ) | ( ( ~b ) & d );
				k = 0x5A827999;
			}
			else if ( i < 40 )
			{
				f = b ^ c ^ d;
				k = 0x6ED9EBA1;
			}
			else if ( i < 60 )
			{
				f = ( b & c ) | ( b & d ) | ( c & d );
				k = 0x8F1BBCDC;
			}
			else
			{
				f = b ^ c ^ d;
				k = 0xCA62C1D6;
			}

			auto temp = this->rotl( a, 5 ) + f + e + k + w[ i ];
			e = d;
			d = c;
			c = this->rotl( b, 30 );
			b = a;
			a = temp;
		}

		this->m_state[ 0 ] += a;
		this->m_state[ 1 ] += b;
		this->m_state[ 2 ] += c;
		this->m_state[ 3 ] += d;
		this->m_state[ 4 ] += e;
	}

	std::uint32_t m_state[ 5 ] {};
	std::uint64_t m_count { 0 };
	std::uint8_t m_buffer[ 64 ] {};
	std::uint8_t m_digest[ 20 ] {};
};

namespace math
{
	inline float remap_val_clamped( float val, float a, float b, float c, float d )
	{
		if ( a == b )
			return val >= b ? d : c;

		float val_ = ( val - a ) / ( b - a );
		val_ = std::clamp( val_, 0.0f, 1.0f );

		return c + ( d - c ) * val_;
	}

	inline void sin_cos( float rad, float& sine, float& cosine )
	{
		sine = sinf( rad );
		cosine = cosf( rad );
	}

	inline void angle_vectors( const vector3d& angles, vector3d& forward )
	{
		float sp, sy, cp, cy;

		sin_cos( deg_2_rad( angles.x ), sp, cp );
		sin_cos( deg_2_rad( angles.y ), sy, cy );

		forward.x = cp * cy;
		forward.y = cp * sy;
		forward.z = -sp;
	}

	inline void angle_vectors(const vector3d& angles, vector3d* forward, vector3d* right, vector3d* up)
	{
		float pitch = angles.x * (M_PI / 180.0f);
		float yaw = angles.y * (M_PI / 180.0f);
		float roll = angles.z * (M_PI / 180.0f);

		float sp = sinf(pitch);
		float cp = cosf(pitch);
		float sy = sinf(yaw);
		float cy = cosf(yaw);
		float sr = sinf(roll);
		float cr = cosf(roll);

		if (forward)
		{
			forward->x = cp * cy;
			forward->y = cp * sy;
			forward->z = -sp;
		}

		if (right)
		{
			right->x = -1 * sr * sp * cy + -1 * cr * -sy;
			right->y = -1 * sr * sp * sy + -1 * cr * cy;
			right->z = -1 * sr * cp;
		}

		if (up)
		{
			up->x = cr * sp * cy + -sr * -sy;
			up->y = cr * sp * sy + -sr * cy;
			up->z = cr * cp;
		}
	}

	inline void angle_matrix( const vector3d& angles, matrix3x4& matrix )
	{
		float sp, sy, sr, cp, cy, cr;

		sin_cos( deg_2_rad( angles.y ), sy, cy );
		sin_cos( deg_2_rad( angles.x ), sp, cp );
		sin_cos( deg_2_rad( angles.z ), sr, cr );

		matrix[ 0 ][ 0 ] = cp * cy;
		matrix[ 1 ][ 0 ] = cp * sy;
		matrix[ 2 ][ 0 ] = -sp;
		float crcy = cr * cy;
		float crsy = cr * sy;
		float srcy = sr * cy;
		float srsy = sr * sy;
		matrix[ 0 ][ 1 ] = sp * srcy - srsy;
		matrix[ 1 ][ 1 ] = sp * crsy + srcy;
		matrix[ 2 ][ 1 ] = sr * cp;
		matrix[ 0 ][ 2 ] = sp * crcy + srsy;
		matrix[ 1 ][ 2 ] = sp * srcy - srcy;
		matrix[ 2 ][ 2 ] = cr * cp;
		matrix[ 0 ][ 3 ] = 0.0f;
		matrix[ 1 ][ 3 ] = 0.0f;
		matrix[ 2 ][ 3 ] = 0.0f;
	}

	inline void matrix_cpy( const matrix3x4& src, matrix3x4& dst )
	{
		if ( &src != &dst )
		{
			memcpy( dst._this, src._this, 12 * sizeof( float ) );
		}
	}

	inline void set_matrix_origin( const matrix3x4& source, const vector3d& origin, matrix3x4& output )
	{
		matrix3x4 tmp {};
		matrix_cpy( source, tmp );

		tmp[ 0 ][ 3 ] = origin.x;
		tmp[ 1 ][ 3 ] = origin.y;
		tmp[ 2 ][ 3 ] = origin.z;

		output = tmp;
	}

	inline float vec_dot_product( const vector3d& a, const vector3d& b )
	{
		return ( a.x * b.x
			+ a.y * b.y
			+ a.z * b.z );
	}


	inline void vector_transform( const vector3d& input, const matrix3x4& matrix, vector3d& output )
	{
		output.x = vec_dot_product( input, vector3d( matrix[ 0 ][ 0 ], matrix[ 0 ][ 1 ], matrix[ 0 ][ 2 ] ) ) + matrix[ 0 ][ 3 ];
		output.y = vec_dot_product( input, vector3d( matrix[ 1 ][ 0 ], matrix[ 1 ][ 1 ], matrix[ 1 ][ 2 ] ) ) + matrix[ 1 ][ 3 ];
		output.z = vec_dot_product( input, vector3d( matrix[ 2 ][ 0 ], matrix[ 2 ][ 1 ], matrix[ 2 ][ 2 ] ) ) + matrix[ 2 ][ 3 ];
	}

	inline bool segment_aabb_intersect(const vector3d& p0, const vector3d& p1, const vector3d& bmin, const vector3d& bmax, float& out_t0, float& out_t1)
	{
		vector3d d = p1 - p0;
		float t0 = 0.0f, t1 = 1.0f;

		auto axis = [&](float p0a, float da, float minA, float maxA) -> bool {
			if (std::fabs(da) < 1e-6f) {
				if (p0a < minA || p0a > maxA)
					return false; // parallel, outside slab
				return true;      // parallel, inside slab
			}
			float inv = 1.0f / da;
			float tNear = (minA - p0a) * inv;
			float tFar = (maxA - p0a) * inv;
			if (tNear > tFar)
				std::swap(tNear, tFar);

			t0 = max(t0, tNear);
			t1 = min(t1, tFar);
			return t0 <= t1;
			};

		if (!axis(p0.x, d.x, bmin.x, bmax.x)) return false;
		if (!axis(p0.y, d.y, bmin.y, bmax.y)) return false;
		if (!axis(p0.z, d.z, bmin.z, bmax.z)) return false;

		out_t0 = std::clamp(t0, 0.0f, 1.0f);
		out_t1 = std::clamp(t1, 0.0f, 1.0f);
		return out_t0 <= out_t1;
	}

	inline matrix3x4_t transform_to_matrix(const c_bone_data* in2)
	{
		matrix3x4_t matrix;
		vector4d rot = in2->rotation;
		float rot_w = rot.w;
		vector3d pos = in2->origin;

		matrix.data[0][0] = 1.0f - 2.0f * rot.y * rot.y - 2.0f * rot.z * rot.z;
		matrix.data[1][0] = 2.0f * rot.x * rot.y + 2.0f * rot_w * rot.z;
		matrix.data[2][0] = 2.0f * rot.x * rot.z - 2.0f * rot_w * rot.y;

		matrix.data[0][1] = 2.0f * rot.x * rot.y - 2.0f * rot_w * rot.z;
		matrix.data[1][1] = 1.0f - 2.0f * rot.x * rot.x - 2.0f * rot.z * rot.z;
		matrix.data[2][1] = 2.0f * rot.y * rot.z + 2.0f * rot_w * rot.x;

		matrix.data[0][2] = 2.0f * rot.x * rot.z + 2.0f * rot_w * rot.y;
		matrix.data[1][2] = 2.0f * rot.y * rot.z - 2.0f * rot_w * rot.x;
		matrix.data[2][2] = 1.0f - 2.0f * rot.x * rot.x - 2.0f * rot.y * rot.y;

		matrix.data[0][3] = pos.x;
		matrix.data[1][3] = pos.y;
		matrix.data[2][3] = pos.z;

		return matrix;
	}

	inline static vector3d matrix_transform(const matrix3x4_t& matrix, const vector3d& in) // wiem ze to PRAWIE to samo co na gorze przepraszam
	{
		vector3d out;
		out.x = matrix.data[0][0] * in.x + matrix.data[0][1] * in.y + matrix.data[0][2] * in.z + matrix.data[0][3];
		out.y = matrix.data[1][0] * in.x + matrix.data[1][1] * in.y + matrix.data[1][2] * in.z + matrix.data[1][3];
		out.z = matrix.data[2][0] * in.x + matrix.data[2][1] * in.y + matrix.data[2][2] * in.z + matrix.data[2][3];
		return out;
	}

	inline bool intersect_ray_capsule(const vector3d& ray_origin, const vector3d& ray_direction, const vector3d& capsule_start, const vector3d& capsule_end, float radius, vector3d* hit_point)
	{
		vector3d capsule_dir = capsule_end - capsule_start;
		vector3d to_ray = ray_origin - capsule_start;

		const float e = capsule_dir.dot(capsule_dir);
		if (e < 1e-8f)
		{
			float distSq = (ray_origin - capsule_start).length_sqr();
			return distSq <= radius * radius;
		}

		float b = ray_direction.dot(capsule_dir);
		float c = ray_direction.dot(to_ray);
		float f = capsule_dir.dot(to_ray);

		const float a = 1.0f;

		float denom = a * e - b * b;
		float s = 0.f, t = 0.f;

		if (denom != 0.f)
		{
			s = (b * f - c * e) / denom;
			s = max(s, 0.0f);
		}
		else
		{
			s = 0.0f;
		}

		t = (b * s + f) / e;
		t = std::clamp(t, 0.0f, 1.0f);

		vector3d closest_ray = ray_origin + ray_direction * s;
		vector3d closest_capsule = capsule_start + capsule_dir * t;

		if (hit_point)
			*hit_point = closest_ray;

		float distance_sq = (closest_ray - closest_capsule).length_sqr();
		return distance_sq <= radius * radius;
	}

	inline vector2d calc_spread( int seed, float inaccuracy, float spread, float recoil_index, 
		int item_def_idx, int num_bullets )
	{
		constexpr std::uint16_t revolver_id { 64 };
		constexpr std::uint16_t negev_id { 28 };
		constexpr float two_pi = 2.f * 3.14159265f;

		static c_uniform_random_stream rng;
		rng.set_seed( seed );

		float inac_r = rng.random_float( 0.f, 1.f );
		float inac_a = rng.random_float( 0.f, two_pi );

		if ( item_def_idx == revolver_id && num_bullets == 1 )
		{
			inac_r = 1.f - ( inac_r * inac_r );
		}
		else if ( item_def_idx == negev_id && recoil_index < 3.f )
		{
			auto v = inac_r; auto c = 3;
			do {
				--c; v *= v;
			} while ( ( float ) c > recoil_index );
			inac_r = 1.f - v;
		}
		inac_r *= inaccuracy;

		float spr_r = rng.random_float( 0.f, 1.f );
		float spr_a = rng.random_float( 0.f, two_pi );

		if ( item_def_idx == revolver_id && num_bullets == 1 )
		{
			spr_r = 1.f - ( spr_r * spr_r );
		}
		else if ( item_def_idx == negev_id && recoil_index < 3.f )
		{
			auto v = spr_r; auto c = 3;
			do {
				--c; v *= v;
			} while ( ( float ) c > recoil_index );
			spr_r = 1.f - v;
		}
		spr_r *= spread;

		return {
			std::cosf( spr_a ) * spr_r + std::cosf( inac_a ) * inac_r,
			std::sinf( spr_a ) * spr_r + std::sinf( inac_a ) * inac_r
		};
	}


	inline float remap_value(float val, float a, float b, float c, float d)
	{
		if (a == b)
			return val >= b ? d : c;
		return c + (d - c) * (val - a) / (b - a);
	}

	inline float remap_value_clamped(float val, float a, float b, float c, float d)
	{
		if (a == b)
			return val >= b ? d : c;

		float c_val = (val - a) / (b - a);
		c_val = std::clamp(c_val, 0.0f, 1.0f);

		return c + (d - c) * c_val;
	}

	inline static float normalize_angle( float a )
	{
		return a - std::floorf( a * 0.0027777778f + 0.5f ) * 360.0f;
	}

	inline static float quantize_angle( float a )
	{
		return std::floorf( normalize_angle( a ) * 2.0f ) * 0.5f;
	}

	inline std::uint32_t get_server_seed( const vector3d& viewangles, int tick )
	{
		struct
		{
			float pitch;
			float yaw;
			int player_render_tick;
		} buffer {};

		buffer.pitch = quantize_angle( viewangles.x );
		buffer.yaw = quantize_angle( viewangles.y );
		buffer.player_render_tick = tick;

		sha1 hash {};
		hash.reset( );
		hash.update( &buffer, 12 );
		hash.final( );

		return hash.get_first_uint32( );
	}

	inline int hitbox_to_bone_index( int hitbox )
	{
		switch ( hitbox )
		{
		case 0:  return 6;
		case 1:  return 5;
		case 2:  return 0;
		case 3:  return 1;
		case 4:  return 2;
		case 5:  return 3;
		case 6:  return 4;
		case 7:  return 22;
		case 8:  return 25;
		case 9:  return 23;
		case 10: return 26;
		case 11: return 24;
		case 12: return 27;
		case 13: return 10;
		case 14: return 15;
		case 15: return 8;
		case 16: return 9;
		case 17: return 13;
		case 18: return 14;
		default: return -1;
		}
	}
}