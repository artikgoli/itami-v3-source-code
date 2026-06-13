#pragma once
#include "vector3d.h"

class vector2d
{
public:
	float x, y;
	vector2d( ) : x { 0.f }, y { 0.f }
	{
	}

	vector2d( float x, float y ) : x { x }, y { y }
	{
	}

	vector2d( int x, int y ) : x { static_cast< float >( x ) }, y { static_cast< float >( y ) }
	{
	}

	vector2d( const vector2d& other ) : x { other.x }, y { other.y } { }
	vector2d operator+( const vector2d& other ) const
	{
		return vector2d( x + other.x, y + other.y );
	}
	vector2d operator-( const vector2d& other ) const
	{
		return vector2d( x - other.x, y - other.y );
	}
	vector2d operator*( float scalar ) const
	{
		return vector2d( x * scalar, y * scalar );
	}
	vector2d operator/( float scalar ) const
	{
		return vector2d( x / scalar, y / scalar );
	}
	vector2d operator/=( float scalar )
	{
		this->x /= scalar;
		this->y /= scalar;

		return vector2d( this->x, this->y );
	}
	vector2d operator-=( const vector2d& other )
	{
		this->x -= other.x;
		this->y -= other.y;
		return vector2d( this->x, this->y );
	}
	vector2d operator-( ) const
	{
		return vector2d( -x, -y );
	}

	float length( ) const
	{
		return sqrtf( x * x + y * y );
	}

	vector2d normalized( ) const
	{
		float len = length( );
		if ( len > 0.f )
			return { x / len, y / len };
		return { 0.f, 0.f };
	}

	bool is_zero( ) const
	{
		return x == 0.f && y == 0.f;
	}

	static vector2d from_3d( const vector3d& angle )
	{
		auto sensitivity_ptr = memory->read<uintptr_t>( sdk->client_dll( ).base + cs2_dumper::offsets::client_dll::dwSensitivity );
		auto sensitivity = memory->read<float>( sensitivity_ptr + cs2_dumper::offsets::client_dll::dwSensitivity_sensitivity );

		auto m_yaw = 0.022f * sensitivity;
		auto m_pitch = 0.022f * sensitivity;
		return vector2d( -angle.y / m_yaw, angle.x / m_pitch );
	}
};