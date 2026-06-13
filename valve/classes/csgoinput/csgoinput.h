#pragma once

/*
	reversing notes:
	buttons and secondary_buttons always match
	0x298, if any movement button is pressed, this is 1
	 - this includes the mouse buttons, like IN_ATTACK
*/

class c_csgoinput
{
public:
	int& buttons( )
	{
		return *( int* ) ( uintptr_t( this ) + 0x250 );
	}

	int& secondary_buttons( )
	{
		return *( int* ) ( uintptr_t( this ) + 0x258 );
	}

	void add_to_buttons( int addition )
	{
		buttons( ) |= addition;
		secondary_buttons( ) = buttons( ); // ? always sync this ?
	}

	// yaw = 0x68C
	
};