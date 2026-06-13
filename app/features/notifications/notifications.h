#pragma once
#include "../../renderer/renderer.h"

/* 
siema to jest mega pojebane wiec szybki info drop:

notify ktore dzialaja na krutki czas
- notif->info
- notif->success
- notif->error

notify kture czekaja az cos sie zrobi no

auto cwel = notif->begin_info("abc");
notif->finish_success(cwel, "def");

lub finish_error jak kto lubi

pozdro - gruby

*/


struct notif_entry_t
{
	std::string text;
	color col;
	float end_time;
	bool persistent = false;
};

namespace notif_colors
{
	inline const color info = color( 255, 251, 0 );
	inline const color success = color( 120, 255, 120 );
	inline const color error = color( 255, 90, 90 );
}

class c_notification
{
public:
	void info( const std::string& text, float duration = 2.5f )
	{
		push( text, notif_colors::info, duration, false );
	}

	void success( const std::string& text, float duration = 2.5f )
	{
		push( text, notif_colors::success, duration, false );
	}

	void error( const std::string& text, float duration = 4.0f )
	{
		push( text, notif_colors::error, duration, false );
	}

	notif_entry_t* begin_info( const std::string& text )
	{
		notifs.push_back( {
			text,
			notif_colors::info,
			0.f,
			true
			} );

		return &notifs.back( );
	}

	void finish( notif_entry_t* n, const std::string& text, const color& col, float duration)
	{
		if ( !n || col.r < 0 )
		{
			n->end_time = global_vars->get_time( );
			return;
		}

		n->text = text;
		n->col = col;
		n->persistent = false;
		n->end_time = global_vars->get_time( ) + duration;
	}

	void finish_success( notif_entry_t* n, const std::string& text, float duration = 2.5f )
	{
		finish( n, text, notif_colors::success, duration );
	}

	void finish_error( notif_entry_t* n, const std::string& text, float duration = 4.0f )
	{
		finish( n, text, notif_colors::error, duration );
	}

	void draw( )
	{
		int y = 5;

		for (auto it = notifs.begin( ); it != notifs.end( ); )
		{
			if (!it->persistent && global_vars->get_time( ) > it->end_time)
			{
				it = notifs.erase( it );
				continue;
			}

			const auto& text = it->text;
			const auto& col = it->col;

			const auto size = draw_pool->font_smooth.text_extent( text );

			const int x = 5;

			render->text( x + 1, y + 1, 
				draw_pool->font_smooth, color( 0, 0, 0, 150 ), 
				daisy::TEXT_ALIGNX_LEFT, true,
				text.c_str( ) );

			render->text( x, y,
				draw_pool->font_smooth, col,
				daisy::TEXT_ALIGNX_LEFT, true,
				text.c_str( ) );

			y += static_cast< int >( size.y ) + 6;
			++it;
		}
	}

private:
	void push( const std::string& text, const color& col, float duration, bool persistent )
	{
		notifs.push_back( { text, col, global_vars->get_time( ) + duration, persistent } );
	}

private:
	std::vector<notif_entry_t> notifs;
};

inline auto notif = std::make_unique<c_notification>( );
