#pragma once


class matrix3x4
{
public:
	float* operator[]( int i )
	{
		return _this[ i ];
	}
	const float* operator[]( int i ) const
	{
		return _this[ i ];
	}
	float* base( )
	{
		return &_this[ 0 ][ 0 ];
	}
	const float* base( ) const
	{
		return &_this[ 0 ][ 0 ];
	}

	float _this[ 3 ][ 4 ] {};
};

class matrix3x4_t {
public:
	float data[3][4];

	matrix3x4_t() {
		for (int i = 0; i < 3; ++i)
			for (int j = 0; j < 4; ++j)
				data[i][j] = 0.0f;
	}
};