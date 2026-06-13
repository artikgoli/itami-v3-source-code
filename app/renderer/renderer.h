#pragma once
#include "../utilities/daisy.h"

using font_t = daisy::c_fontwrapper;

struct c_last_viewport
{
	int x {}, y { }, w { }, h { };
};

class c_single_buffer
{
public:
	daisy::c_renderqueue queue;
	daisy::c_texatlas atlas;
	c_last_viewport last_viewport = {};

	void set_viewport( int x, int y, int w, int h );

	void circle( int x, int y, float radius, const color& clr, int segments = 30 );
	void circle_aa( int x, int y, float radius, const color& clr, int segments = 30 );
	void circle_filled( int x, int y, float radius, const color& clr, int segments = 30 );
	void line( float x, float y, float x1, float y1, const color& clr, float size = 1.f );
	void line_aa( float x, float y, float x1, float y1, const color& clr, float size = 1.f );
	void arrow_aa( float cx, float cy, float angle, float radius, float size, const color& tip, const color& base_col );
	void rectangle( int x, int y, int w, int h, const color& clr, bool fill = false );
	void push_text( int x, int y, font_t& font, color clr, daisy::daisy_text_align aligment, bool outline, const char* buffer );
	void push_text( int x, int y, font_t& font, color clr, daisy::daisy_text_align aligment, bool outline, color outline_clr, const char* buffer );
	void push_text_w( int x, int y, font_t& font, color clr, daisy::daisy_text_align alignment, bool outline, color outline_clr, const wchar_t* buffer );
	void push_text_w( int x, int y, font_t& font, color clr, daisy::daisy_text_align alignment, bool outline, const wchar_t* buffer );
	void texture( int x, int y, int w, int h, color clr, unsigned long id, int tex_w, int tex_h );
	void gradient_rectangle( int x, int y, int w, int h, const color& clr1, const color& clr2, bool vertical );
};

struct c_texture
{
	unsigned int id = 0;
	int w = 0, h = 0;

	c_texture( ) = default;
};

class c_draw_pool
{
public:
	void flush_buffers( );
	void load_texture( unsigned char* buffer, int size, c_texture& texture );
	void create_textures( );
	std::vector<uint8_t> decode_png( unsigned char* data, int size, int& out_w, int& out_h );
	ID3D11ShaderResourceView* create_srv_from_rgba( unsigned char* rgba, int w, int h );
	ID3D11ShaderResourceView* create_srv_from_bcn( const unsigned char* data, int w, int h, DXGI_FORMAT fmt );
	ID3D11ShaderResourceView* create_srv_from_png( unsigned char* data, int size, int& out_w, int& out_h );

	void pre_reset( ) const
	{
		buffer->queue.reset( true );
		buffer->atlas.reset( true );

	}

	void post_reset( )
	{
		buffer->queue.reset( false );
		buffer->atlas.reset( false );

		// re-create the textures
		create_textures( );
	}

	c_texture color_picker_gradient {};

	std::unique_ptr< c_single_buffer > buffer = nullptr;
	font_t font_default = {};
	font_t font_debug = {};
	font_t font_esp = {};
	font_t font_esp_s = {};
	font_t font_esp_pixel = {};
	font_t font_smooth = {};
	font_t font_smooth_big = {};

	unsigned int max_texture_id = 0;

};

inline auto draw_pool = std::make_unique<c_draw_pool>( );

class c_renderer
{
public:
	void save_viewport( int x, int y, int w, int h )
	{
		draw_pool->buffer->last_viewport.x = x;
		draw_pool->buffer->last_viewport.y = y;
		draw_pool->buffer->last_viewport.w = w;
		draw_pool->buffer->last_viewport.h = h;
	}

	void set_viewport( int x, int y, int w, int h )
	{
		save_viewport( draw_pool->buffer->last_viewport.x, draw_pool->buffer->last_viewport.y,
			draw_pool->buffer->last_viewport.w, draw_pool->buffer->last_viewport.h );

		draw_pool->buffer->set_viewport( x, y, w, h );
		// if something breaks tell mad
		// draw_pool->buffer->rectangle( x, y, w, h, color( 255, 0, 0 ), false );
	}

	void restore_viewport( ) const
	{
		draw_pool->buffer->set_viewport( 
			draw_pool->buffer->last_viewport.x,
			draw_pool->buffer->last_viewport.y, 
			draw_pool->buffer->last_viewport.w, 
			draw_pool->buffer->last_viewport.h );
	}

	void line( float x1, float y1, float x2, float y2, color clr, float size )
	{
		draw_pool->buffer->line( x1, y1, x2, y2, clr, size );
	}

	void line_aa( float x1, float y1, float x2, float y2, color clr, float size )
	{
		draw_pool->buffer->line_aa( x1, y1, x2, y2, clr, size );
	}

	void arrow_aa( float cx, float cy, float angle, float radius, float size, color tip, color base )
	{
		draw_pool->buffer->arrow_aa( cx, cy, angle, radius, size, tip, base );
	}

	void textured_rect( int x, int y, int w, int h, color clr, unsigned long id, int tex_w, int tex_h )
	{
		draw_pool->buffer->texture( x, y, w, h, clr, id, tex_w, tex_h );
	}

	void circle( int x, int y, int radius, int segments, color clr, bool outline = true, color outline_clr = color( 0, 0, 0 ) )
	{
		draw_pool->buffer->circle( x, y, ( float ) radius, clr, segments );
	}

	void circle_aa( int x, int y, float radius, int segments, color clr )
	{
		draw_pool->buffer->circle_aa( x, y, radius, clr, segments );
	}

	void circle_filled( int x, int y, float radius, int segments, color clr )
	{
		draw_pool->buffer->circle_filled( x, y, radius, clr, segments );
	}

	void checkmark( int x, int y, color clr )
	{
		int x1 = x + 3;
		int y1 = y + 7;

		int x2 = x + 5;
		int y2 = y + 11;

		int x3 = x + 14;
		int y3 = y + 1;

		line( x1, y1, x2, y2, clr, 2.f );
		line( x2, y2, x3, y3, clr, 2.f );
	}

	void gradient_line( int x1, int y1, int x2, int y2, color first_clr, color middle_clr, color second_clr, float size )
	{
		const int h = max( 1, ( int ) size );
		const int mx = ( x1 + x2 ) / 2;
		const int my = ( y1 + y2 ) / 2;
		if ( mx > x1 )
			draw_pool->buffer->gradient_rectangle( x1, y1, mx - x1, h, first_clr, middle_clr, false );
		if ( x2 > mx )
			draw_pool->buffer->gradient_rectangle( mx, my, x2 - mx, h, middle_clr, second_clr, false );
	}

	void rect( int x, int y, int w, int h, color clr, bool fill )
	{
		draw_pool->buffer->rectangle( x, y, w, h, clr, fill );
	}

	void gradient_rect( int x, int y, int w, int h, color clr1, color clr2, bool vertical )
	{
		draw_pool->buffer->gradient_rectangle( x, y, w, h, clr1, clr2, vertical );
	}

	void gradient_rect_filled( int x, int y, int w, int h, color clr1, color clr2 )
	{
		draw_pool->buffer->gradient_rectangle( x, y, w, h, clr1, clr2, false );
	}

	void outlined_filled_rect( int x, int y, int w, int h, color clr, color outline_clr )
	{
		draw_pool->buffer->rectangle( x, y, w, h, clr, true );
		draw_pool->buffer->rectangle( x, y, w, h, outline_clr, false );
	}

	void depth_rect( int x, int y, int w, int h, color clr, bool fill, color outline_clr, int depth_size = 1 )
	{
		draw_pool->buffer->rectangle( x, y, w, h, clr, fill );
		
		for ( int i = 0; i < depth_size; i++ )
		{
			int alpha = outline_clr.a - ( ( outline_clr.a / depth_size ) * i );
			color temp = outline_clr;
			temp.a = alpha;

			draw_pool->buffer->rectangle( x - i, y - i, w + ( i * 2 ), h + ( i * 2 ), temp, false );
		}
	}

	void text( int x, int y, font_t& font, color clr, daisy::daisy_text_align aligment, bool outline, color outline_clr, const char* fmt, ... )
	{
		va_list args {};
		char buff[ 1024 ] = "\0";
		va_start( args, fmt );
		vsnprintf_s( buff, sizeof( buff ), fmt, args );
		va_end( args );
		draw_pool->buffer->push_text( x, y, font, clr, aligment, outline, outline_clr, buff );
	}

	void text( int x, int y, font_t& font, color clr, daisy::daisy_text_align aligment, bool outline, const char* fmt, ... )
	{
		va_list args {};
		char buff[ 1024 ] = "\0";
		va_start( args, fmt );
		vsnprintf_s( buff, sizeof( buff ), fmt, args );
		va_end( args );
		draw_pool->buffer->push_text( x, y, font, clr, aligment, outline, buff );
	}

	void text_w( int x, int y, font_t& font, color clr, daisy::daisy_text_align alignment, bool outline, const wchar_t* fmt, ... )
	{
		va_list args {};
		wchar_t buff[ 1024 ] {};

		va_start( args, fmt );
		vswprintf_s( buff, fmt, args );
		va_end( args );

		draw_pool->buffer->push_text_w(
			x, y, font, clr, alignment, outline, buff );
	}

	void arc( int cx, int cy, float radius, float thickness, float start, float end, color clr )
	{
		float arc_length = std::fabs( end - start );

		int segments = std::clamp( ( int ) ( arc_length * radius * 1.6f ), 64, 1024 );

		float step = ( end - start ) / segments;

		daisy::color_t solid( clr.r, clr.g, clr.b, clr.a );
		daisy::color_t fade( clr.r, clr.g, clr.b, 0 );

		float outer = radius;
		float inner = radius - thickness;

		float prev_cos = std::cos( start );
		float prev_sin = std::sin( start );

		for (int i = 1; i <= segments; i++)
		{
			float angle = start + step * i;

			float cos_a = std::cos( angle );
			float sin_a = std::sin( angle );

			daisy::point_t o0 { cx + prev_cos * outer, cy + prev_sin * outer };
			daisy::point_t o1 { cx + cos_a * outer,    cy + sin_a * outer };

			daisy::point_t i0 { cx + prev_cos * inner, cy + prev_sin * inner };
			daisy::point_t i1 { cx + cos_a * inner,    cy + sin_a * inner };

			draw_pool->buffer->queue.push_filled_triangle( o0, o1, i0, solid, solid, solid );
			draw_pool->buffer->queue.push_filled_triangle( i0, o1, i1, solid, solid, solid );

			draw_pool->buffer->queue.push_filled_triangle(
				{ o0.x + prev_cos * 0.5f, o0.y + prev_sin * 0.5f },
				{ o1.x + cos_a * 0.5f,    o1.y + sin_a * 0.5f },
				o0,
				fade, fade, solid );

			draw_pool->buffer->queue.push_filled_triangle(
				o0,
				{ o1.x + cos_a * 0.5f, o1.y + sin_a * 0.5f },
				o1,
				solid, fade, solid );

			draw_pool->buffer->queue.push_filled_triangle(
				i0,
				i1,
				{ i0.x - prev_cos * 0.5f, i0.y - prev_sin * 0.5f },
				solid, solid, fade );

			draw_pool->buffer->queue.push_filled_triangle(
				{ i0.x - prev_cos * 0.5f, i0.y - prev_sin * 0.5f },
				i1,
				{ i1.x - cos_a * 0.5f, i1.y - sin_a * 0.5f },
				fade, solid, fade );

			prev_cos = cos_a;
			prev_sin = sin_a;
		}
	}

	bool world_to_screen( vector3d pos, vector2d& screen );
	void update_view_matrix( );
	matrix4x4 view_matrix = {};
};

inline auto render = std::make_unique<c_renderer>( );