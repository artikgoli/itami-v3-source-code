#include "vector3d.h"
#include "math.h"

float vector3d::fov_to( const vector3d& angle ) const
{
	vector3d src = {};
	vector3d dst = {};
	math::angle_vectors( *this, src );
	math::angle_vectors( angle, dst );

	float result = rad_2_deg( acosf( dst.dot( src ) / dst.length_sqr( ) ) );
	if ( !isfinite( result ) || isinf( result ) || isnan( result ) )
		result = 99999.f; // :troll:

	return fabsf( result );
}