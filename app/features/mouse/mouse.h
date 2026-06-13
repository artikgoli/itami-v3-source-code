#pragma once

class c_mouse_accumulator
{
public:
	void add_movement( float x, float y )
	{
		accumulated_x += x;
		accumulated_y += y;
	}

	void reset( )
	{
		accumulated_x = accumulated_y = 0;
	}

	void apply_movement( )
	{
		if ( fabsf( accumulated_x ) < 1.f && fabsf( accumulated_y ) < 1.f )
			return;

		mouse_event( MOUSEEVENTF_MOVE, 
			static_cast< long >( std::lround( accumulated_x ) ),
			static_cast< long >( std::lround( accumulated_y ) ), 0, 0 );

		reset( );
	}

private:
	float accumulated_x = 0;
	float accumulated_y = 0;
};

inline auto mouse_accumulator = std::make_unique<c_mouse_accumulator>( );