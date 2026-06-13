#pragma once

enum key_state : char
{
	NONE,
	PRESSED,
	HELD,
	RELEASED
};

class c_input_handler
{
public:
	void on_wndproc( UINT msg, WPARAM w_param, LPARAM l_param );
	void handle_key_menu_input( UINT msg, WPARAM w_param, LPARAM l_param );

	char key_state( int key );
	char keybind_state( int key );

	std::string get_key_name( int code );
	void update_keybinds( );
	void poll_keybinding( const float& global_time );
	bool poll_text_input( const float& global_time );
	void add_keybind( int key )
	{
		bound_keys.emplace_back( key );
	}

	void remove_keybind( int key )
	{
		bool found = false;
		for ( auto& k : bound_keys )
		{
			if ( k == key )
			{
				found = true;
				break;
			}
		}

		if ( found )
			bound_keys.erase( std::remove( bound_keys.begin( ), bound_keys.end( ), key ), bound_keys.end( ) );
	}

	vector2d cursor_pos = {};

	void update( )
	{
		previous_state = current_state;
		scroll_delta = 0;
	}

	void update_keybind_state( )
	{
		previous_keybinds_state = current_keybinds_state;
	}

	int get_scroll_delta( ) const
	{
		return scroll_delta;
	}

private:
	std::array<bool, 256> current_state {};
	std::array<bool, 256> previous_state {};

	std::vector<int> bound_keys {};

	std::array<bool, 256> current_keybinds_state {};
	std::array<bool, 256> previous_keybinds_state {};

	int scroll_delta = 0;
};

inline auto input_handler = std::make_unique<c_input_handler>( );