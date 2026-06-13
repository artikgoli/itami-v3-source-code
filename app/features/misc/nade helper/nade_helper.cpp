#include "nade_helper.h"
#include "..\..\entity_cache\entity_cache.h"
#include "..\..\..\renderer\renderer.h"
#include "..\..\..\renderer\window_handler.h"
#include <chrono>
#include "..\..\aimbot\aimbot.h"
#include "../../menu/input/input.h"

static constexpr float kPi = 3.14159265f;

static const char* nade_type_name( int type )
{
	switch ( type )
	{
		case 43: return "flash";
		case 44: return "he";
		case 45: return "smoke";
		case 46: return "molotov";
		case 47: return "decoy";
		case 48: return "incendiary";
		default: return nullptr;
	}
}

void c_nade_helper::on_input( )
{
	if ( !config_variables->misc.nade_helper_auto_aim->value ||
		GetForegroundWindow( ) != window_handler->target_handle )
	{
		auto_aim_active = config_variables->misc.nade_helper_auto_aim_key->active = false;
		return;
	}

	auto* kb = config_variables->misc.nade_helper_auto_aim_key.get( );
	switch ( kb->activation_type )
	{
	case 0:
		auto_aim_active = true;
		break;
	case 1:
	{
		auto state = input_handler->keybind_state( kb->key );
		auto_aim_active = ( state == PRESSED || state == HELD );
		break;
	}
	case 2:
		if ( input_handler->keybind_state( kb->key ) == PRESSED )
			auto_aim_active = !auto_aim_active;
		break;
	}
	kb->active = auto_aim_active;
}

void c_nade_helper::on_render( )
{
	if ( !config_variables->misc.nade_helper_enabled->value || !has_nade )
		return;

	static auto last_time  = std::chrono::steady_clock::now( );
	static auto start_time = std::chrono::steady_clock::now( );
	const auto  now = std::chrono::steady_clock::now( );
	const float dt = std::clamp( std::chrono::duration<float>( now - last_time ).count( ), 0.f, 0.1f );
	const float elapsed = std::chrono::duration<float>( now - start_time ).count( );
	last_time = now;

	vector2d screen_size {};
	window_handler->actual_size( screen_size );
	const float cx = screen_size.x * 0.5f;
	const float cy = screen_size.y * 0.5f;

	auto rect_outline = [&]( int x, int y, int w, int h, color clr )
	{
		render->rect( x, y, w, 1, clr, true );
		render->rect( x, y + h - 1, w, 1,     clr, true );
		render->rect( x, y + 1, 1, h - 2, clr, true );
		render->rect( x + w - 1, y + 1, 1, h - 2, clr, true );
	};

	auto draw_box = [&]( int x, int y, int w, int h, float a )
	{
		render->rect( x - 2, y - 2, w + 4,  h + 4,  color( 16, 16, 16, ( int ) ( a * 150.f ) ), true );
		rect_outline( x - 3, y - 3, w + 6,  h + 6,  color( 16, 16, 16, ( int ) ( a * 170.f ) ) );
		rect_outline( x - 4, y - 4, w + 8,  h + 8,  color( 16, 16, 16, ( int ) ( a * 195.f ) ) );
		rect_outline( x - 5, y - 5, w + 10, h + 10, color( 16, 16, 16, ( int ) ( a *  40.f ) ) );
	};

	vector3d cur_origin, cur_angle;
	int      cur_item_def;
	{
		std::lock_guard guard( global_lock );

		cur_origin   = origin;
		cur_angle    = angle;
		cur_item_def = item_def;

		const uint32_t ver = entries_version.load( std::memory_order_relaxed );
		if ( render_version != ver )
		{
			render_entries = entries;
			render_version = ver;
		}
	}

	for ( auto& entry : render_entries )
	{
		if ( entry.nade_type != cur_item_def || entry.nade_type <= 0 )
		{
			entry.anim_range = max( 0.f, entry.anim_range - dt * 8.f );
			continue;
		}

		const float dx   = cur_origin.x - entry.origin.x;
		const float dy   = cur_origin.y - entry.origin.y;
		const float dist = sqrtf( dx * dx + dy * dy );

		if ( dist <= entry.max_origin_diff * 25.f )
			entry.anim_range = min( 1.f, entry.anim_range + dt * 8.f );
		else
			entry.anim_range = max( 0.f, entry.anim_range - dt * 8.f );

		if ( entry.anim_range <= 0.f )
			continue;

		const float range_mp = entry.anim_range;
		const bool  on_spot  = dist <= entry.max_origin_diff;

		const char* type_str = nade_type_name( entry.nade_type );
		char        name_buf[ 64 ];
		if ( type_str )
			snprintf( name_buf, sizeof( name_buf ), "%s [%s]", entry.name.empty( ) ? "spot" : entry.name.c_str( ), type_str );
		else
			snprintf( name_buf, sizeof( name_buf ), "%s", entry.name.empty( ) ? "spot" : entry.name.c_str( ) );

		const int   alpha_full = ( int ) ( range_mp * 255.f );
		const color col_white  = color( 255, 255, 255, alpha_full );
		const color col_grey   = color( 180, 180, 180, ( int ) ( range_mp * 180.f ) );
		const color col_green  = color(  60, 255, 100, alpha_full );

		const auto name_sz = draw_pool->font_esp.text_extent( std::string_view( name_buf ) );

		vector2d stand_sc {};
		if ( render->world_to_screen( entry.origin, stand_sc ) )
		{
			char dist_buf[ 16 ];
			snprintf( dist_buf, sizeof( dist_buf ), "%.0f u", dist );

			auto dist_sz = draw_pool->font_esp.text_extent( std::string_view( dist_buf ) );

			const int bw = max( name_sz.x, dist_sz.x ) + 4;
			const int bh = name_sz.y + dist_sz.y + 3;
			const int bx = ( int ) stand_sc.x - bw / 2;
			const int by = ( int ) stand_sc.y - bh - 8;

			draw_box( bx, by, bw, bh, range_mp );

			render->text( ( int ) stand_sc.x, by + 1,
				draw_pool->font_esp, col_white, daisy::TEXT_ALIGNX_CENTER, false, name_buf );
			render->text( ( int ) stand_sc.x, by + name_sz.y + 3,
				draw_pool->font_esp, on_spot ? col_green : col_grey,
				daisy::TEXT_ALIGNX_CENTER, false, dist_buf );
		}

		if ( !on_spot )
		{
			entry.anim_fov    = max( 0.f, entry.anim_fov    - dt * 5.5f );
			entry.anim_screen = max( 0.f, entry.anim_screen - dt * 4.5f );
			continue;
		}

		vector3d forward {};
		math::angle_vectors( entry.angle, forward );
		forward *= 2048.f;

		const vector3d aim_world = entry.origin + vector3d( 0.f, 0.f, 64.f ) + forward;

		vector2d aim_sc_raw {};
		const bool aim_on_screen = render->world_to_screen( aim_world, aim_sc_raw );

		const float fov_diff = cur_angle.fov_to( entry.angle );
		const bool  aimed    = fov_diff <= entry.max_angle_diff;

		entry.anim_fov    = aimed        ? min( 1.f, entry.anim_fov    + dt * 6.5f ) : max( 0.f, entry.anim_fov    - dt * 5.5f );
		entry.anim_screen = aim_on_screen ? min( 1.f, entry.anim_screen + dt * 3.5f ) : max( 0.f, entry.anim_screen - dt * 4.5f );

		const float visible_a = ( 0.5f + entry.anim_screen * 0.5f ) * range_mp;
		const int   vis_a_i   = ( int ) ( visible_a * 255.f );

		if ( vis_a_i > 0 && aim_on_screen )
		{
			char        fov_buf[ 24 ] {};
			const char* sub_str;
			if ( aimed )
				sub_str = entry.throw_type.empty( ) ? "THROW!" : entry.throw_type.c_str( );
			else
			{
				snprintf( fov_buf, sizeof( fov_buf ), "%.1f deg", fov_diff );
				sub_str = fov_buf;
			}

			constexpr int CIRC_R = 8;
			constexpr int PAD    = 4;

			const auto sub_sz = draw_pool->font_esp.text_extent( std::string_view( sub_str ) );

			const int box_w = name_sz.x + PAD * 2;
			const int box_h = name_sz.y + sub_sz.y + 3 + PAD * 2;
			const int ax    = ( int ) aim_sc_raw.x;
			const int ay    = ( int ) aim_sc_raw.y;
			const int bx    = ax - box_w / 2;
			const int by    = ay + CIRC_R + 5;

			draw_box( bx, by, box_w, box_h, visible_a );

			const int   circ_a = ( int ) ( range_mp * entry.anim_screen * 255.f );
			const float t_fov  = entry.anim_fov;

			const int ring_r = ( int ) ( 160 + ( col_green.r - 160 ) * t_fov );
			const int ring_g = ( int ) ( 160 + ( col_green.g - 160 ) * t_fov );
			const int ring_b = ( int ) ( 160 + ( col_green.b - 160 ) * t_fov );

			render->circle_filled( ax, ay, ( float ) ( CIRC_R - 1 ), 32,
				color( 0, 0, 0, circ_a / 4 ) );
			render->circle_aa( ax, ay, ( float ) CIRC_R, 32,
				color( ring_r, ring_g, ring_b, circ_a ) );

			const int ty = by + PAD;
			render->text( ax, ty, draw_pool->font_esp,
				color( 255, 255, 255, vis_a_i ), daisy::TEXT_ALIGNX_CENTER, false, name_buf );
			render->text( ax, ty + name_sz.y + 3, draw_pool->font_esp,
				color( ring_r, ring_g, ring_b, vis_a_i ),
				daisy::TEXT_ALIGNX_CENTER, false, sub_str );
		}

		const float arrow_alpha = ( 1.f - entry.anim_screen ) * range_mp;
		if ( arrow_alpha <= 0.f )
			continue;

		const float ddx = aim_sc_raw.x - cx;
		const float ddy = aim_sc_raw.y - cy;
		const float hw  = cx - 80.f;
		const float hh  = cy - 80.f;

		float arx = cx, ary = cy;
		if ( fabsf( ddx ) > 0.f || fabsf( ddy ) > 0.f )
		{
			const float s = fabsf( ddx ) * hh > fabsf( ddy ) * hw
				? hw / fabsf( ddx )
				: hh / fabsf( ddy );
			arx = cx + ddx * s;
			ary = cy + ddy * s;
		}

		const float pulse   = 0.2f + 0.8f * fabsf( sinf( elapsed * kPi * 0.8f ) );
		const int   arrow_a = ( int ) ( arrow_alpha * pulse * 255.f );

		const float ang = atan2f( ddy, ddx ) + kPi * 0.5f;
		const float cs  = cosf( ang ), sn = sinf( ang );

		auto rot = [&]( float px, float py, float& ox, float& oy ) {
			ox = arx + px * cs - py * sn;
			oy = ary + px * sn + py * cs;
		};

		float x0, y0, x1, y1, x2, y2;
		rot(   0.f, -10.f, x0, y0 );
		rot(  -6.f,  10.f, x1, y1 );
		rot(   6.f,  10.f, x2, y2 );

		const color shad = color(   0,   0,   0, arrow_a );
		const color fill = color( 255, 255, 255, arrow_a );

		render->line( ( int ) x0, ( int ) y0, ( int ) x1, ( int ) y1, shad, 3.f );
		render->line( ( int ) x1, ( int ) y1, ( int ) x2, ( int ) y2, shad, 3.f );
		render->line( ( int ) x2, ( int ) y2, ( int ) x0, ( int ) y0, shad, 3.f );
		render->line( ( int ) x0, ( int ) y0, ( int ) x1, ( int ) y1, fill, 1.5f );
		render->line( ( int ) x1, ( int ) y1, ( int ) x2, ( int ) y2, fill, 1.5f );
		render->line( ( int ) x2, ( int ) y2, ( int ) x0, ( int ) y0, fill, 1.5f );
	}
}

void c_nade_helper::on_entity_update( const float& t )
{
	const std::string map = global_vars->level_name( );
	if ( !map.empty( ) && map != current_map )
	{
		std::lock_guard guard( global_lock );
		current_map = map;
		entries.clear( );
		load_entries_for_map( current_map );
		rebuild_spot_names( );
	}

	auto& m = config_variables->misc;
	if ( m.selected_flash != prev_sel_flash ) {
		prev_sel_flash = m.selected_flash; m.nade_active_delete_type = 43;
	}
	if ( m.selected_he != prev_sel_he ) {
		prev_sel_he = m.selected_he; m.nade_active_delete_type = 44;
	}
	if ( m.selected_smoke != prev_sel_smoke ) {
		prev_sel_smoke = m.selected_smoke; m.nade_active_delete_type = 45;
	}
	if ( m.selected_molotov != prev_sel_molotov ) {
		prev_sel_molotov = m.selected_molotov; m.nade_active_delete_type = 46;
	}
	if ( m.selected_decoy != prev_sel_decoy ) {
		prev_sel_decoy = m.selected_decoy; m.nade_active_delete_type = 47;
	}
	if ( m.selected_incendiary != prev_sel_incend ) {
		prev_sel_incend = m.selected_incendiary; m.nade_active_delete_type = 48;
	}

	auto& local = entity_cache->get_local_player( );
	if ( !local.is_valid( ) )
	{
		has_nade = false;
		return;
	}

	auto* gsn = local.pawn->game_scene_node( );
	if ( !gsn )
	{
		has_nade = false;
		return;
	}

	origin = gsn->abs_origin( );
	angle = memory->read<vector3d>( sdk->client_dll( ).base + cs2_dumper::offsets::client_dll::dwViewAngles );

	has_nade = false;
	auto* ws = local.pawn->weapon_services( );
	if ( ws )
	{
		auto* weapon = ( c_base_weapon* ) entity_helper->get_from_handle( ws->active_weapon_handle( ) );
		auto* vdata  = weapon ? weapon->weapon_vdata( ) : nullptr;
		if ( vdata && vdata->weapon_type( ) == 9 )
		{
			item_def = weapon->item_definition_index( );
			has_nade = true;
		}
	}

	if ( !has_nade || !auto_aim_active )
		return;

	const int max_fov  = config_variables->misc.nade_helper_max_fov->value;
	const int smooth_x = config_variables->misc.nade_helper_smooth_x->value;
	const int smooth_y = config_variables->misc.nade_helper_smooth_y->value;

	nade_entry best {};
	float best_fov = FLT_MAX;

	{
		std::lock_guard guard( global_lock );
		for ( const auto& e : entries )
		{
			if ( e.nade_type != item_def || e.nade_type <= 0 )
				continue;
			const float fov = angle.fov_to( e.angle );
			if ( fov > max_fov || fov >= best_fov )
				continue;
			best_fov = fov;
			best     = e;
		}
	}

	if ( best_fov == FLT_MAX )
		return;

	vector3d forward {};
	math::angle_vectors( best.angle, forward );
	forward *= 2048.f;

	const vector3d aim_world = best.origin + vector3d( 0.f, 0.f, 64.f ) + forward;

	const vector3d eye_pos = local.pawn->get_eye_position( );

	vector3d target_angle = eye_pos.angle_to( aim_world );
	target_angle.sanity_clamp( );

	vector3d delta = angle - target_angle;
	delta.sanity_clamp( );

	const vector2d pixels = vector2d::from_3d( delta );
	if ( std::hypot( pixels.x, pixels.y ) < 2.f )
		return;

	aimbot->aim_at_safe( delta, nullptr, smooth_x, smooth_y );
}

void c_nade_helper::save_spot( )
{
	std::lock_guard guard( global_lock );

	auto& local = entity_cache->get_local_player( );
	if ( !local.is_valid( ) )
		return;

	auto* gsn = local.pawn->game_scene_node( );
	if ( !gsn )
		return;

	nade_entry entry {};
	entry.origin = gsn->abs_origin( );
	entry.angle = memory->read<vector3d>( sdk->client_dll( ).base + cs2_dumper::offsets::client_dll::dwViewAngles );
	entry.max_angle_diff = 15.f;
	entry.max_origin_diff = 30.f;

	auto* ws = local.pawn->weapon_services( );
	if ( ws )
	{
		auto* cur_weapon = ( c_base_weapon* ) entity_helper->get_from_handle( ws->active_weapon_handle( ) );
		auto* vdata      = cur_weapon ? cur_weapon->weapon_vdata( ) : nullptr;
		if ( vdata && vdata->weapon_type( ) == 9 )
			entry.nade_type = cur_weapon->item_definition_index( );
	}

	const std::string& custom_name = config_variables->misc.nade_helper_spot_name.value;
	entry.name = custom_name.empty( )
		? ( "spot_" + std::to_string( entries.size( ) + 1 ) )
		: custom_name;

	entry.throw_type = config_variables->misc.nade_throw_type_input.value;

	entries.push_back( entry );
	rebuild_spot_names( );
	config_variables->misc.nade_helper_spot_name.value.clear( );
	config_variables->misc.nade_throw_type_input.value.clear( );

	if ( !current_map.empty( ) )
		save_entries_for_map( current_map );
}

void c_nade_helper::rebuild_spot_names( )
{
	entries_version.fetch_add( 1, std::memory_order_relaxed );
	config_variables->misc.spot_names.clear( );
	config_variables->misc.spot_names.reserve( entries.size( ) );
	for ( const auto& e : entries )
	{
		std::string display = e.name.empty( ) ? "unnamed" : e.name;
		if ( const char* type = nade_type_name( e.nade_type ) )
		{
			display += " [";
			display += type;
			display += "]";
		}
		config_variables->misc.spot_names.push_back( std::move( display ) );
	}

	rebuild_typed_spot_lists( );
}

void c_nade_helper::rebuild_typed_spot_lists( )
{
	auto& m = config_variables->misc;
	m.flash_spot_names.clear( );      m.flash_spot_indices.clear( );
	m.he_spot_names.clear( );         m.he_spot_indices.clear( );
	m.smoke_spot_names.clear( );      m.smoke_spot_indices.clear( );
	m.molotov_spot_names.clear( );    m.molotov_spot_indices.clear( );
	m.decoy_spot_names.clear( );      m.decoy_spot_indices.clear( );
	m.incendiary_spot_names.clear( ); m.incendiary_spot_indices.clear( );

	for ( int i = 0; i < ( int ) entries.size( ); i++ )
	{
		const auto& e = entries[ i ];

		std::vector<std::string>* names   = nullptr;
		std::vector<int>*         indices = nullptr;

		switch ( e.nade_type )
		{
			case 43: names = &m.flash_spot_names;      indices = &m.flash_spot_indices;      break;
			case 44: names = &m.he_spot_names;         indices = &m.he_spot_indices;         break;
			case 45: names = &m.smoke_spot_names;      indices = &m.smoke_spot_indices;      break;
			case 46: names = &m.molotov_spot_names;    indices = &m.molotov_spot_indices;    break;
			case 47: names = &m.decoy_spot_names;      indices = &m.decoy_spot_indices;      break;
			case 48: names = &m.incendiary_spot_names; indices = &m.incendiary_spot_indices; break;
			default: continue;
		}

		names->push_back( e.name.empty( ) ? "unnamed" : e.name );
		indices->push_back( i );
	}

	if ( m.selected_flash      >= ( int ) m.flash_spot_names.size( )      ) m.selected_flash      = max( 0, ( int ) m.flash_spot_names.size( )      - 1 );
	if ( m.selected_he         >= ( int ) m.he_spot_names.size( )         ) m.selected_he         = max( 0, ( int ) m.he_spot_names.size( )         - 1 );
	if ( m.selected_smoke      >= ( int ) m.smoke_spot_names.size( )      ) m.selected_smoke      = max( 0, ( int ) m.smoke_spot_names.size( )      - 1 );
	if ( m.selected_molotov    >= ( int ) m.molotov_spot_names.size( )    ) m.selected_molotov    = max( 0, ( int ) m.molotov_spot_names.size( )    - 1 );
	if ( m.selected_decoy      >= ( int ) m.decoy_spot_names.size( )      ) m.selected_decoy      = max( 0, ( int ) m.decoy_spot_names.size( )      - 1 );
	if ( m.selected_incendiary >= ( int ) m.incendiary_spot_names.size( ) ) m.selected_incendiary = max( 0, ( int ) m.incendiary_spot_names.size( ) - 1 );
}

void c_nade_helper::remove_entry( int idx )
{
	std::lock_guard guard( global_lock );
	if ( idx < 0 || idx >= static_cast<int>( entries.size( ) ) )
		return;

	entries.erase( entries.begin( ) + idx );
	rebuild_spot_names( );
	if ( !current_map.empty( ) )
		save_entries_for_map( current_map );
}

void c_nade_helper::export_to_clipboard( )
{
	std::lock_guard guard( global_lock );

	std::ostringstream out;
	out << std::fixed << std::setprecision( 6 );
	out << "// itami | " << ( current_map.empty( ) ? "unknown" : current_map )
	    << " | " << entries.size( ) << " spots\n";

	for ( const auto& e : entries )
	{
		const char* type_str = nade_type_name( e.nade_type );
		if ( !type_str ) type_str = "any";

		std::string safe_name = e.name;
		for ( char& c : safe_name ) if ( c == '"' ) c = '\'';

		std::string safe_throw = e.throw_type;
		for ( char& c : safe_throw ) if ( c == '"' ) c = '\'';

		out << type_str << " \"" << safe_name << "\""
		    << " { pos=" << e.origin.x << " " << e.origin.y << " " << e.origin.z
		    << " ang=" << e.angle.x << " " << e.angle.y << " " << e.angle.z
		    << " fov=" << e.max_angle_diff << " range=" << e.max_origin_diff
		    << " throw=\"" << safe_throw << "\""
		    << " }\n";
	}

	const std::string text = out.str( );
	if ( !OpenClipboard( nullptr ) )
		return;

	EmptyClipboard( );
	HGLOBAL hmem = GlobalAlloc( GMEM_MOVEABLE, text.size( ) + 1 );
	if ( hmem )
	{
		memcpy( GlobalLock( hmem ), text.c_str( ), text.size( ) + 1 );
		GlobalUnlock( hmem );
		SetClipboardData( CF_TEXT, hmem );
	}
	CloseClipboard( );
}

void c_nade_helper::import_from_clipboard( )
{
	if ( !OpenClipboard( nullptr ) )
		return;

	HANDLE hdata = GetClipboardData( CF_TEXT );
	if ( !hdata ) { CloseClipboard( ); return; }

	const char* raw = static_cast<const char*>( GlobalLock( hdata ) );
	std::string data = raw ? raw : "";
	GlobalUnlock( hdata );
	CloseClipboard( );

	if ( data.empty( ) )
		return;

	std::lock_guard guard( global_lock );

	if ( data.find( "valid_entries" ) != std::string::npos )
	{
		std::istringstream in( data );
		std::string line;
		nade_entry current {};
		int  fields  = 0;
		bool started = false;

		while ( std::getline( in, line ) )
		{
			auto pos = line.find( '=' );
			if ( pos == std::string::npos ) continue;

			std::string key   = line.substr( 0, pos );
			std::string value = line.substr( pos + 1 );
			key.erase( key.find_last_not_of( " \t" ) + 1 );
			value.erase( 0, value.find_first_not_of( " \t" ) );
			std::istringstream val( value );

			if      ( key == "valid_entries"   ) { }
			else if ( key == "name"            ) { if ( started ) entries.push_back( current ); current = {}; current.name = value; started = true; fields = 1; }
			else if ( key == "throw_type"      ) { current.throw_type = value; }
			else if ( key == "nade_type"       ) { val >> current.nade_type; }
			else if ( key == "angle"           ) { val >> current.angle.x  >> current.angle.y  >> current.angle.z;  fields |= 2; }
			else if ( key == "origin"          ) { val >> current.origin.x >> current.origin.y >> current.origin.z; fields |= 4; }
			else if ( key == "max_angle_diff"  ) { val >> current.max_angle_diff; fields |= 8; }
			else if ( key == "max_origin_diff" ) { val >> current.max_origin_diff; entries.push_back( current ); current = {}; started = false; fields = 0; }
		}
		if ( started && ( fields & ( 2 | 4 | 8 ) ) )
			entries.push_back( current );
	}
	else
	{
		auto type_from_str = [] ( const std::string& s ) -> int
		{
			if ( s == "flash"      ) return 43;
			if ( s == "he"         ) return 44;
			if ( s == "smoke"      ) return 45;
			if ( s == "molotov"    ) return 46;
			if ( s == "decoy"      ) return 47;
			if ( s == "incendiary" ) return 48;
			if ( s == "any"        ) return 0;
			return -1;
		};

		std::istringstream in( data );
		std::string line;

		while ( std::getline( in, line ) )
		{
			size_t s = line.find_first_not_of( " \t\r" );
			if ( s == std::string::npos ) continue;
			line = line.substr( s );
			if ( line.size( ) >= 2 && line[ 0 ] == '/' && line[ 1 ] == '/' ) continue;

			std::istringstream ss( line );
			std::string type_str;
			ss >> type_str;

			int nade_type = type_from_str( type_str );
			if ( nade_type == -1 ) continue;

			char c;
			while ( ss.get( c ) && c != '"' );
			std::string name;
			while ( ss.get( c ) && c != '"' ) name += c;

			nade_entry entry {};
			entry.nade_type       = nade_type;
			entry.name            = name;
			entry.max_angle_diff  = 10.f;
			entry.max_origin_diff = 30.f;

			std::string token;
			while ( ss >> token )
			{
				if ( token == "{" || token == "}" ) continue;
				auto eq = token.find( '=' );
				if ( eq == std::string::npos ) continue;
				std::string key = token.substr( 0, eq );
				std::istringstream vss( token.substr( eq + 1 ) );
				if      ( key == "pos"   ) { vss >> entry.origin.x; ss >> entry.origin.y >> entry.origin.z; }
				else if ( key == "ang"   ) { vss >> entry.angle.x;  ss >> entry.angle.y  >> entry.angle.z;  }
				else if ( key == "fov"   ) { vss >> entry.max_angle_diff;  }
				else if ( key == "range" ) { vss >> entry.max_origin_diff; }
				else if ( key == "throw" )
				{
					std::string tv = token.substr( eq + 1 );
					if ( tv.size( ) >= 2 && tv.front( ) == '"' && tv.back( ) == '"' )
						entry.throw_type = tv.substr( 1, tv.size( ) - 2 );
				}
			}

			entries.push_back( entry );
		}
	}

	rebuild_spot_names( );

	if ( !current_map.empty( ) )
		save_entries_for_map( current_map );
}

void c_nade_helper::save_entries_for_map( const std::string_view& map )
{
	try
	{
		auto dir = storage_base( ) / "nades";
		if ( !std::filesystem::exists( dir ) )
			std::filesystem::create_directories( dir );

		std::ofstream out( dir / ( std::string( map ) + ".txt" ), std::ios::trunc );
		out.exceptions( std::ofstream::failbit | std::ofstream::badbit );

		out << std::fixed << std::setprecision( 6 );
		out << "valid_entries = " << entries.size( ) << '\n';
		for ( const auto& e : entries )
		{
			out << "name = "            << e.name            << '\n';
			out << "throw_type = "      << e.throw_type       << '\n';
			out << "nade_type = "       << e.nade_type        << '\n';
			out << "angle = "           << e.angle.x  << ' ' << e.angle.y  << ' ' << e.angle.z  << '\n';
			out << "origin = "          << e.origin.x << ' ' << e.origin.y << ' ' << e.origin.z << '\n';
			out << "max_angle_diff = "  << e.max_angle_diff   << '\n';
			out << "max_origin_diff = " << e.max_origin_diff  << '\n';
		}
	}
	catch ( const std::exception& e )
	{
		MessageBoxA( 0, e.what( ), "nade save", MB_OK | MB_ICONERROR );
	}
}

void c_nade_helper::load_entries_for_map( const std::string_view& map )
{
	try
	{
		auto full = storage_base( ) / "nades" / ( std::string( map ) + ".txt" );
		std::ifstream in( full );
		if ( !in.is_open( ) )
			return;

		entries.clear( );
		nade_entry current {};
		int fields = 0;

		std::string line;
		while ( std::getline( in, line ) )
		{
			auto pos = line.find( '=' );
			if ( pos == std::string::npos )
				continue;

			std::string key   = line.substr( 0, pos );
			std::string value = line.substr( pos + 1 );
			key.erase( key.find_last_not_of( " \t" ) + 1 );
			value.erase( 0, value.find_first_not_of( " \t" ) );

			std::istringstream val( value );

			if ( key == "valid_entries" )
			{
				int n = 0; val >> n; entries.reserve( n );
			}
			else if ( key == "name" )
			{
				if ( fields != 0 ) entries.push_back( current );
				current      = {};
				current.name = value;
				fields       = 1;
			}
			else if ( key == "throw_type"      ) { current.throw_type = value; }
			else if ( key == "nade_type"       ) { val >> current.nade_type; }
			else if ( key == "angle"           ) { val >> current.angle.x  >> current.angle.y  >> current.angle.z;  fields |= 2; }
			else if ( key == "origin"          ) { val >> current.origin.x >> current.origin.y >> current.origin.z; fields |= 4; }
			else if ( key == "max_angle_diff"  ) { val >> current.max_angle_diff;  fields |= 8; }
			else if ( key == "max_origin_diff" )
			{
				val >> current.max_origin_diff;
				entries.push_back( current );
				current = {};
				fields  = 0;
			}
		}

		if ( fields & ( 2 | 4 | 8 ) )
			entries.push_back( current );

		rebuild_spot_names( );
	}
	catch ( const std::exception& e )
	{
		MessageBoxA( 0, e.what( ), "nade load exception", MB_OK | MB_ICONERROR );
	}
}
