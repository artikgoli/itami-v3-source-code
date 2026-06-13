#include "renderer.h"
#include "window_handler.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_HDR
#define STBI_ONLY_PNG
#include "..\utilities\stb.h"

void c_draw_pool::flush_buffers( )
{
	buffer->queue.flush( );
	buffer->queue.clear( );
}

void c_single_buffer::circle( int x, int y, float radius, const color& clr, int segments )
{
	const float step = ( 2.0f * M_PI ) / ( float ) segments;
	const float cs = cosf( step ), ss = sinf( step );
	const daisy::color_t c( clr.r, clr.g, clr.b, clr.a );
	float px = radius, py = 0.f;
	for ( int i = 0; i < segments; i++ )
	{
		const float nx = px * cs - py * ss;
		const float ny = px * ss + py * cs;
		queue.push_line( { x + px, y + py }, { x + nx, y + ny }, c, 1.f );
		px = nx; py = ny;
	}
}

void c_single_buffer::circle_filled( int x, int y, float radius, const color& clr, int segments )
{
	const daisy::color_t c( clr.r, clr.g, clr.b, clr.a );
	const daisy::point_t center { ( float ) x, ( float ) y };
	const float step = ( 2.0f * M_PI ) / ( float ) segments;
	const float cs = cosf( step ), ss = sinf( step );
	float px = radius, py = 0.f;
	for ( int i = 0; i < segments; i++ )
	{
		const float nx = px * cs - py * ss;
		const float ny = px * ss + py * cs;
		queue.push_filled_triangle( center, { x + px, y + py }, { x + nx, y + ny }, c, c, c );
		px = nx; py = ny;
	}
}

void c_single_buffer::circle_aa( int x, int y, float radius, const color& clr, int segments )
{
	const float step = ( 2.0f * M_PI ) / ( float ) segments;
	const float cs = cosf( step ), ss = sinf( step );
	const daisy::color_t c( clr.r, clr.g, clr.b, clr.a );
	float px = radius, py = 0.f;
	for ( int i = 0; i < segments; i++ )
	{
		const float nx = px * cs - py * ss;
		const float ny = px * ss + py * cs;
		queue.push_line_aa( { x + px, y + py }, { x + nx, y + ny }, c, 1.f );
		px = nx; py = ny;
	}
}

void c_single_buffer::line( float x, float y, float x1, float y1, const color& clr, float size )
{
	queue.push_line( { x, y }, { x1, y1 },
		daisy::color_t( clr.r, clr.g, clr.b, clr.a ), size );
}

void c_single_buffer::line_aa( float x, float y, float x1, float y1, const color& clr, float size )
{
	queue.push_line_aa( { x, y }, { x1, y1 },
		daisy::color_t( clr.r, clr.g, clr.b, clr.a ), size );
}

void c_single_buffer::arrow_aa( float cx, float cy, float angle, float radius, float size, const color& tip, const color& base )
{
	queue.push_arrow_aa( cx, cy, angle, radius, size,
		daisy::color_t( tip.r, tip.g, tip.b, tip.a ),
		daisy::color_t( base.r, base.g, base.b, base.a ) );
}

void c_single_buffer::set_viewport( int x, int y, int w, int h )
{
	auto pos = daisy::point_t( x, y );
	auto size = daisy::point_t( w, h );
	queue.push_scissor( pos, size );
}

void c_single_buffer::rectangle( int x, int y, int w, int h, const color& clr, bool fill )
{
	if ( fill )
	{
		queue.push_filled_rectangle( { ( float ) x, ( float ) y }, { ( float ) w, ( float ) h },
			daisy::color_t( clr.r, clr.g, clr.b, clr.a ) );
	}
	else
	{
		line( x, y, x + w, y, clr );
		line( x, y + h, x + w + 1, y + h, clr );
		line( x, y + 1, x, y + h, clr );
		line( x + w, y, x + w, y + h, clr );
	}
}

void c_single_buffer::push_text( int x, int y, font_t& font, color clr, daisy::daisy_text_align aligment, bool outline, const char* buffer )
{
	auto text = std::string_view( buffer );
	if ( outline )
	{
		queue.push_text( font, { x - 1, y }, text, daisy::color_t( 1, 1, 1, clr.a ), aligment );
		queue.push_text( font, { x + 1, y }, text, daisy::color_t( 1, 1, 1, clr.a ), aligment );
		queue.push_text( font, { x, y + 1 }, text, daisy::color_t( 1, 1, 1, clr.a ), aligment );
		queue.push_text( font, { x, y - 1 }, text, daisy::color_t( 1, 1, 1, clr.a ), aligment );
	}

	queue.push_text( font, { x, y }, text, daisy::color_t( clr.r, clr.g, clr.b, clr.a ), aligment );
}

void c_single_buffer::push_text( int x, int y, font_t& font, color clr, daisy::daisy_text_align aligment, bool outline, color outline_clr, const char* buffer )
{
	auto text = std::string_view( buffer );
	auto daisy_clr_outline = daisy::color_t( outline_clr.r, outline_clr.g, outline_clr.b, outline_clr.a );
	if ( outline )
	{
		queue.push_text( font, { x - 1, y }, text, daisy_clr_outline, aligment );
		queue.push_text( font, { x + 1, y }, text, daisy_clr_outline, aligment );
		queue.push_text( font, { x, y + 1 }, text, daisy_clr_outline, aligment );
		queue.push_text( font, { x, y - 1 }, text, daisy_clr_outline, aligment );
	}

	queue.push_text( font, { x, y }, text, daisy::color_t( clr.r, clr.g, clr.b, clr.a ), aligment );
}

void c_single_buffer::push_text_w( int x, int y, font_t& font, color clr, daisy::daisy_text_align alignment, bool outline, const wchar_t* buffer )
{
	auto text = std::wstring_view( buffer );

	if (outline)
	{
		queue.push_text( font, { x - 1, y }, text, daisy::color_t( 1, 1, 1, clr.a ), alignment );
		queue.push_text( font, { x + 1, y }, text, daisy::color_t( 1, 1, 1, clr.a ), alignment );
		queue.push_text( font, { x, y + 1 }, text, daisy::color_t( 1, 1, 1, clr.a ), alignment );
		queue.push_text( font, { x, y - 1 }, text, daisy::color_t( 1, 1, 1, clr.a ), alignment );
	}

	queue.push_text( font, { x, y }, text, daisy::color_t( clr.r, clr.g, clr.b, clr.a ), alignment );
}

void c_single_buffer::push_text_w( int x, int y, font_t& font, color clr, daisy::daisy_text_align alignment, bool outline, color outline_clr, const wchar_t* buffer )
{
	auto text = std::wstring_view( buffer );
	auto daisy_clr_outline = daisy::color_t( outline_clr.r, outline_clr.g, outline_clr.b, outline_clr.a );

	if (outline)
	{
		queue.push_text( font, { x - 1, y }, text, daisy_clr_outline, alignment );
		queue.push_text( font, { x + 1, y }, text, daisy_clr_outline, alignment );
		queue.push_text( font, { x, y + 1 }, text, daisy_clr_outline, alignment );
		queue.push_text( font, { x, y - 1 }, text, daisy_clr_outline, alignment );
	}

	queue.push_text( font, { x, y }, text, daisy::color_t( clr.r, clr.g, clr.b, clr.a ), alignment );
}

// forgive me twin
#define SCALE_UV( TW, TH ) { coord[0] + ( float )TW / 4096.f, \
							coord[1] + ( float )TH / 4096.f }

void c_single_buffer::texture( int x, int y, int w, int h, color clr, unsigned long id, int tex_w, int tex_h )
{
	auto& coord = atlas.coords( id );
	queue.push_filled_rectangle( { ( float ) x, ( float ) y }, { ( float ) w, ( float ) h },
		daisy::color_t( clr.r, clr.g, clr.b, clr.a ), atlas.texture_srv( ), { coord[ 0 ], coord[ 1 ] },
		SCALE_UV( tex_w, tex_h ) );
}

void c_single_buffer::gradient_rectangle( int x, int y, int w, int h, const color& clr1, const color& clr2, bool vertical )
{
	auto& first = vertical ? clr1 : clr2;
	auto& second = vertical ? clr2 : clr1;

	queue.push_gradient_rectangle( { ( float ) x, ( float ) y }, { ( float ) w, ( float ) h },
		daisy::color_t( clr1.r, clr1.g, clr1.b, clr1.a ),
		daisy::color_t( first.r, first.g, first.b, first.a ),
		daisy::color_t( second.r, second.g, second.b, second.a ),
		daisy::color_t( clr2.r, clr2.g, clr2.b, clr2.a ) );
}

static std::unique_ptr<color[ ]> gradient = nullptr;
static color color_from_pos( int x, int y )
{
	return gradient[ x + y * 256 ];
}

static std::vector<uint8_t> load_texture_from_buffer( unsigned char* buffer, int size, int& w, int& h )
{
	std::vector<uint8_t> ret { };
	int chan;

	stbi_set_flip_vertically_on_load( false );
	stbi_set_flip_vertically_on_load_thread( false );
	unsigned char* tex_data = stbi_load_from_memory( buffer, size, &w, &h, &chan, 4 );
	if ( !tex_data )
		return ret;

	ret.resize( ( unsigned long long )( w * h * 4 ) );
	memcpy( ret.data( ), tex_data, ( unsigned long long )( w * h * 4 ) );
	stbi_image_free( tex_data );

	return ret;
}

void c_draw_pool::load_texture( unsigned char* _buffer, int size, c_texture& texture )
{
	int w {}, h {};
	auto tex_data = load_texture_from_buffer( _buffer, size, w, h );
	buffer->atlas.append( ++max_texture_id, { static_cast< float >( w ), static_cast< float >( h ) },
		tex_data.data( ), ( unsigned int ) tex_data.size( ) );

	texture.id = max_texture_id;
	texture.w = w;
	texture.h = h;
}

std::vector<uint8_t> c_draw_pool::decode_png( unsigned char* data, int size, int& out_w, int& out_h )
{
	int chan = 0;
	stbi_set_flip_vertically_on_load( false );
	unsigned char* pixels = stbi_load_from_memory( data, size, &out_w, &out_h, &chan, 4 );
	if ( !pixels )
		return {};
	std::vector<uint8_t> result( pixels, pixels + out_w * out_h * 4 );
	stbi_image_free( pixels );
	return result;
}

ID3D11ShaderResourceView* c_draw_pool::create_srv_from_rgba( unsigned char* rgba, int w, int h )
{
	if ( !rgba || !daisy::daisy_t::s_device )
		return nullptr;

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width            = w;
	desc.Height           = h;
	desc.MipLevels        = 1;
	desc.ArraySize        = 1;
	desc.Format           = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage            = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags        = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA init_data = {};
	init_data.pSysMem     = rgba;
	init_data.SysMemPitch = w * 4;

	ID3D11Texture2D* tex = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;

	if ( SUCCEEDED( daisy::daisy_t::s_device->CreateTexture2D( &desc, &init_data, &tex ) ) )
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
		srv_desc.Format              = DXGI_FORMAT_R8G8B8A8_UNORM;
		srv_desc.ViewDimension       = D3D11_SRV_DIMENSION_TEXTURE2D;
		srv_desc.Texture2D.MipLevels = 1;
		daisy::daisy_t::s_device->CreateShaderResourceView( tex, &srv_desc, &srv );
		tex->Release( );
	}
	return srv;
}

ID3D11ShaderResourceView* c_draw_pool::create_srv_from_bcn(
	const unsigned char* data, int w, int h, DXGI_FORMAT fmt )
{
	if ( !data || !daisy::daisy_t::s_device )
		return nullptr;

	const bool  small_block = ( fmt == DXGI_FORMAT_BC1_UNORM || fmt == DXGI_FORMAT_BC4_UNORM );
	const int   block_bytes = small_block ? 8 : 16;
	const bool  is_bcn      = ( fmt != DXGI_FORMAT_R8G8B8A8_UNORM &&
	                             fmt != DXGI_FORMAT_B8G8R8A8_UNORM );

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width            = (UINT)w;
	desc.Height           = (UINT)h;
	desc.MipLevels        = 1;
	desc.ArraySize        = 1;
	desc.Format           = fmt;
	desc.SampleDesc.Count = 1;
	desc.Usage            = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags        = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA init = {};
	init.pSysMem     = data;
	init.SysMemPitch = is_bcn ? ( ( ( w + 3 ) / 4 ) * block_bytes ) : ( w * 4 );

	ID3D11Texture2D*          tex = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;

	if ( SUCCEEDED( daisy::daisy_t::s_device->CreateTexture2D( &desc, &init, &tex ) ) )
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC sv = {};
		sv.Format              = fmt;
		sv.ViewDimension       = D3D11_SRV_DIMENSION_TEXTURE2D;
		sv.Texture2D.MipLevels = 1;
		daisy::daisy_t::s_device->CreateShaderResourceView( tex, &sv, &srv );
		tex->Release( );
	}
	return srv;
}

ID3D11ShaderResourceView* c_draw_pool::create_srv_from_png( unsigned char* data, int size, int& out_w, int& out_h )
{
	int chan = 0;
	stbi_set_flip_vertically_on_load( false );
	unsigned char* pixels = stbi_load_from_memory( data, size, &out_w, &out_h, &chan, 4 );
	if ( !pixels )
		return nullptr;

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width            = out_w;
	desc.Height           = out_h;
	desc.MipLevels        = 1;
	desc.ArraySize        = 1;
	desc.Format           = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage            = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags        = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA init_data = {};
	init_data.pSysMem     = pixels;
	init_data.SysMemPitch = out_w * 4;

	ID3D11Texture2D* tex = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;

	if ( SUCCEEDED( daisy::daisy_t::s_device->CreateTexture2D( &desc, &init_data, &tex ) ) )
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
		srv_desc.Format              = DXGI_FORMAT_R8G8B8A8_UNORM;
		srv_desc.ViewDimension       = D3D11_SRV_DIMENSION_TEXTURE2D;
		srv_desc.Texture2D.MipLevels = 1;
		daisy::daisy_t::s_device->CreateShaderResourceView( tex, &srv_desc, &srv );
		tex->Release( );
	}

	stbi_image_free( pixels );
	return srv;
}

void c_draw_pool::create_textures( )
{
	// NOT NEEDED FOR NOW
	//    !!! ALSO BROKEN IDK WHY THE TEXTURE DOESNT RENDER
	// the gradient needs to bypass the stb
	// because its a raw buffer of pixels

	// use load_texture for pngs, jpgs etc

	/*const int dimension = 256;
	gradient = std::make_unique<color[ ]>( static_cast<unsigned long long>( dimension ) * dimension * sizeof( color ) );

	float hue {}, sat { .99f }, lum { 1.f };
	for ( int i = 0; i < dimension; ++i ) 
	{
		for ( int j = 0; j < dimension; ++j )
		{
			gradient[ i + static_cast<size_t>( j ) * dimension ] = color::hsl_to_rgb( hue, sat, lum );
			hue += 1.f / dimension;
		}
		lum -= ( 1.f / dimension );
		hue = 0.f;
	}

	buffer->atlas.append( ++max_texture_id,
		{ ( float ) dimension, ( float ) dimension },
		( uint8_t* ) gradient.get( ),
		( unsigned int ) ( static_cast<unsigned long long>( dimension ) * dimension * sizeof( color ) )
	);

	color_picker_gradient.id = max_texture_id;
	color_picker_gradient.w = dimension;
	color_picker_gradient.h = dimension;*/
}

bool c_renderer::world_to_screen( vector3d pos, vector2d& screen )
{
	float x = window_handler->window_size.x * 0.5f;
	float y = window_handler->window_size.y * 0.5f;
	float view =
		view_matrix.value[ 3 ][ 0 ] * pos.x +
		view_matrix.value[ 3 ][ 1 ] * pos.y +
		view_matrix.value[ 3 ][ 2 ] * pos.z +
		view_matrix.value[ 3 ][ 3 ];

	if ( view <= 0.01f )
		return false;

	screen.x = x + (
		view_matrix.value[ 0 ][ 0 ] * pos.x +
		view_matrix.value[ 0 ][ 1 ] * pos.y +
		view_matrix.value[ 0 ][ 2 ] * pos.z +
		view_matrix.value[ 0 ][ 3 ] ) / view * x;

	screen.y = y - (
		view_matrix.value[ 1 ][ 0 ] * pos.x +
		view_matrix.value[ 1 ][ 1 ] * pos.y +
		view_matrix.value[ 1 ][ 2 ] * pos.z +
		view_matrix.value[ 1 ][ 3 ] ) / view * y;


	return true;
}

void c_renderer::update_view_matrix( )
{
	auto matrix = memory->read<matrix4x4>( sdk->view_matrix.address );
	memcpy_s( &view_matrix, sizeof( matrix4x4 ), &matrix, sizeof( matrix4x4 ) );
}
