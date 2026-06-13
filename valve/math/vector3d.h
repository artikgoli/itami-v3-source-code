#pragma once

#define M_RADPI		57.295779513082f
#define M_PI		3.14159265358979323846f

#define deg_2_rad(x)	((float)(x) * (float)((float)(M_PI) / 180.0f))
#define rad_2_deg(x)	((float)(x) * (float)(180.0f / (float)(M_PI)))

class vector3d
{
public:
	float x {}, y {}, z {};

	bool is_zero( ) const
	{
		return x == 0.f && y == 0.f && z == 0.f;
	}

	vector3d operator*( float scale ) const
	{
		return vector3d{ x * scale, y * scale, z * scale };
	}

	vector3d operator-( const vector3d& other ) const
	{
		return vector3d{ x - other.x, y - other.y, z - other.z };
	}

	vector3d operator+( const vector3d& other ) const
	{
		return vector3d { x + other.x, y + other.y, z + other.z };
	}

	vector3d operator/=( const float& other )
	{
		( *this ).x /= other;
		( *this ).y /= other;
		( *this ).z /= other;
		return *this;
	}

	float operator[]( int index ) const
	{
		return ( &x )[ index ];
	}

	vector3d operator/( const float scalar )
	{
		return vector3d( this->x / scalar, this->y / scalar, this->z / scalar );
	}

	vector3d operator*=( const float scalar )
	{
		return vector3d( this->x *= scalar, this->y *= scalar, this->z *= scalar );
	}

	vector3d operator+=( const vector3d& other )
	{
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;
		return *this;
	}

	vector3d sanity_clamp( )
	{
		while ( this->y > 180.f )
			this->y -= 360.f;

		while ( this->y < -180.f )
			this->y += 360.f;

		this->x = std::clamp( this->x, -89.f, 89.f );
		this->z = 0.f;
		return *this;
	}

	vector3d angle_to( const vector3d& destination ) const
	{
		vector3d delta = destination - *this;
		float hyp = std::sqrtf( delta.x * delta.x + delta.y * delta.y );

		vector3d angles {};
		angles.x = std::atan2f( -delta.z, hyp ) * M_RADPI;
		angles.y = std::atan2f( delta.y, delta.x ) * M_RADPI;
		angles.z = 0.f;

		angles.sanity_clamp( );

		return angles;
	}

	// warning, this assumes you are calling it from a ANGLE
	// NOT from a POSITION
	float fov_to( const vector3d& angle ) const;

	float normalize_in_place( )
	{
		const float ln = length( );
		if ( isnan( ln ) || ln == 0.f )
			return 0;

		x /= ln;
		y /= ln;
		z /= ln;
		return ln;
	}

	float dist( const vector3d& other ) const
	{
		return ( other - *this ).length( );
	}

	float distance( const vector3d& v ) const
	{
		return sqrt( pow( x - v.x, 2 ) + pow( y - v.y, 2 ) + pow( z - v.z, 2 ) );
	}

	vector3d cross( const vector3d& other ) const
	{
		return {
			y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x
		};
	}

	static vector3d cross_product(const vector3d& a, const vector3d& b)
	{
		return vector3d
		{
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x
		};
	}

	float dot( const vector3d& v ) const
	{
		return ( x * v.x + y * v.y + z * v.z );
	}

	float length( ) const
	{
		return std::sqrtf( x * x + y * y + z * z );
	}

	float length_2d() const
	{
		return std::sqrt(x * x + y * y);
	}

	float length_sqr( ) const
	{
		return dot(*this);
	}

	vector3d normalize_vector()
	{
		vector3d vector;
		float length = this->length();

		if (length != 0)
		{
			vector.x = x / length;
			vector.y = y / length;
			vector.z = z / length;
		}
		else
		{
			vector.x = vector.y = 0.0f;
			vector.z = 1.0f;
		}

		return vector;
	}

	bool is_close_enough( const vector3d& other, float min_value = 0.001f )
	{
		return fabsf( other.x - this->x ) < min_value && fabsf( other.y - this->y ) < min_value && 
			fabsf( other.z - this->z ) < min_value;
	}
};

class quaternion
{
public:
	float x {}, y {}, z {}, w {};

	quaternion( float x, float y, float z, float w )
		: x( x ), y( y ), z( z ), w( w )
	{
	}

	quaternion( ) = default;
};

class transform_4d
{
public:
	vector3d position {};
	quaternion rotation {};
	float scale {};
};
