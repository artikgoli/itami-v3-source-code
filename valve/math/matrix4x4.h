#pragma once

class matrix4x4
{
public:
	float value[ 4 ][ 4 ];

	matrix4x4( ) = default;

	static matrix4x4 identity( )
	{
		matrix4x4 mat {};
		mat.value[ 0 ][ 0 ] = 1.f;
		mat.value[ 1 ][ 1 ] = 1.f;
		mat.value[ 2 ][ 2 ] = 1.f;
		mat.value[ 3 ][ 3 ] = 1.f;
		return mat;
	}

	static matrix4x4 from_trs( const transform_4d& t )
	{
		// rotation matrix from quaternion
		float xx = t.rotation.x * t.rotation.x;
		float yy = t.rotation.y * t.rotation.y;
		float zz = t.rotation.z * t.rotation.z;
		float xy = t.rotation.x * t.rotation.y;
		float xz = t.rotation.x * t.rotation.z;
		float yz = t.rotation.y * t.rotation.z;
		float wx = t.rotation.w * t.rotation.x;
		float wy = t.rotation.w * t.rotation.y;
		float wz = t.rotation.w * t.rotation.z;

		matrix4x4 mat {};

		// column 0 (right)
		mat.value[ 0 ][ 0 ] = ( 1.f - 2.f * ( yy + zz ) ) * t.scale;
		mat.value[ 1 ][ 0 ] = ( 2.f * ( xy + wz ) ) * t.scale;
		mat.value[ 2 ][ 0 ] = ( 2.f * ( xz - wy ) ) * t.scale;
		mat.value[ 3 ][ 0 ] = 0.f;

		// column 1 (up)
		mat.value[ 0 ][ 1 ] = ( 2.f * ( xy - wz ) ) * t.scale;
		mat.value[ 1 ][ 1 ] = ( 1.f - 2.f * ( xx + zz ) ) * t.scale;
		mat.value[ 2 ][ 1 ] = ( 2.f * ( yz + wx ) ) * t.scale;
		mat.value[ 3 ][ 1 ] = 0.f;

		// column 2 (forward)
		mat.value[ 0 ][ 2 ] = ( 2.f * ( xz + wy ) ) * t.scale;
		mat.value[ 1 ][ 2 ] = ( 2.f * ( yz - wx ) ) * t.scale;
		mat.value[ 2 ][ 2 ] = ( 1.f - 2.f * ( xx + yy ) ) * t.scale;
		mat.value[ 3 ][ 2 ] = 0.f;

		// column 3 (translation)
		mat.value[ 0 ][ 3 ] = t.position.x;
		mat.value[ 1 ][ 3 ] = t.position.y;
		mat.value[ 2 ][ 3 ] = t.position.z;
		mat.value[ 3 ][ 3 ] = 1.f;

		return mat;
	}

	vector3d transform_point( const vector3d& p ) const
	{
		return vector3d
		{
			value[ 0 ][ 0 ] * p.x + value[ 0 ][ 1 ] * p.y + value[ 0 ][ 2 ] * p.z + value[ 0 ][ 3 ],
			value[ 1 ][ 0 ] * p.x + value[ 1 ][ 1 ] * p.y + value[ 1 ][ 2 ] * p.z + value[ 1 ][ 3 ],
			value[ 2 ][ 0 ] * p.x + value[ 2 ][ 1 ] * p.y + value[ 2 ][ 2 ] * p.z + value[ 2 ][ 3 ]
		};
	}
};