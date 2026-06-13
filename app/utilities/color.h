#pragma once

struct color
{
	int r, g, b, a;

    color( ) : r( 0 ), g( 0 ), b( 0 ), a( 255 )
    {
        // method
        r = g = b = 1;
    }
    color( int r, int g, int b, int a = 255 ) : r( r ), g( g ), b( b ), a( a )
    {
        if ( !r && !g && !b )
            this->r = this->g = this->b = 1;
    }

    unsigned long to_d3d( ) const
    {
        return ( ( ( static_cast< unsigned char >( a ) & 0xff ) << 24 ) |
            ( ( static_cast< unsigned char >( r ) & 0xff ) << 16 ) |
            ( ( static_cast< unsigned char >( g ) & 0xff ) << 8 ) |
            ( static_cast< unsigned char >( b ) & 0xff ) );
    }

    color lerp( const color& other, float t ) const
    {
        return color(
            r + ( other.r - r ) * t,
            g + ( other.g - g ) * t,
            b + ( other.b - b ) * t,
            a + ( other.a - a ) * t
        );
    }

	// stack overflow final boss
	static void rgb_to_hsv( const color& c, float* hsv )
	{
		float r = c.r / 255.f;
		float g = c.g / 255.f;
		float b = c.b / 255.f;

		float cmax = max( r, max( g, b ) );
		float cmin = min( r, min( g, b ) );
		float diff = cmax - cmin;

		float h = 0.f, s = 0.f, v = cmax;

		if ( diff > 0.f )
		{
			if ( cmax == r )
				h = fmod( 60 * ( ( g - b ) / diff ) + 360, 360 );
			else if ( cmax == g )
				h = fmod( 60 * ( ( b - r ) / diff ) + 120, 360 );
			else if ( cmax == b )
				h = fmod( 60 * ( ( r - g ) / diff ) + 240, 360 );

			if ( cmax > 0.f )
				s = diff / cmax;
		}

		hsv[ 0 ] = h;
		hsv[ 1 ] = s;
		hsv[ 2 ] = v;
	}

    static color hsv_to_rgb( float h, float s, float v, int alpha = 255 )
    {
		h = fmodf( h, 360.0f );
		if ( h < 0.0f )
			h += 360.0f;

		s = std::clamp( s, 0.0f, 1.0f );
		v = std::clamp( v, 0.0f, 1.0f );

		float r1 {}, g1 {}, b1 {};

		float f = h / 60.0f;
		int hi = static_cast< int >( f ) % 6;
		f = f - static_cast< int >( f );

		float p = v * ( 1.0f - s );
		float q = v * ( 1.0f - f * s );
		float t = v * ( 1.0f - ( 1.0f - f ) * s );

		switch ( hi ) {
		case 0: r1 = v; g1 = t; b1 = p; break;
		case 1: r1 = q; g1 = v; b1 = p; break;
		case 2: r1 = p; g1 = v; b1 = t; break;
		case 3: r1 = p; g1 = q; b1 = v; break;
		case 4: r1 = t; g1 = p; b1 = v; break;
		case 5: r1 = v; g1 = p; b1 = q; break;
		}

		return color(
			static_cast< int >( std::round( r1 * 255.f ) ),
			static_cast< int >( std::round( g1 * 255.f ) ),
			static_cast< int >( std::round( b1 * 255.f ) ),
			alpha
		);
    }

	static color hsl_to_rgb( float h, float s, float l )
	{
		float q;

		if ( l < 0.5f )
			q = l * ( s + 1.f );

		else
			q = l + s - ( l * s );

		float p = 2 * l - q;

		float rgb[ 3 ] {};
		rgb[ 0 ] = h + ( 1.f / 3.f );
		rgb[ 1 ] = h;
		rgb[ 2 ] = h - ( 1.f / 3.f );

		for ( int i = 0; i < 3; ++i ) {
			if ( rgb[ i ] < 0 )
				rgb[ i ] += 1.f;

			if ( rgb[ i ] > 1 )
				rgb[ i ] -= 1.f;

			if ( rgb[ i ] < ( 1.f / 6.f ) )
				rgb[ i ] = p + ( ( q - p ) * 6 * rgb[ i ] );
			else if ( rgb[ i ] < 0.5f )
				rgb[ i ] = q;
			else if ( rgb[ i ] < ( 2.f / 3.f ) )
				rgb[ i ] = p + ( ( q - p ) * 6 * ( ( 2.f / 3.f ) - rgb[ i ] ) );
			else
				rgb[ i ] = p;
		}

		return 
		{
			static_cast<int> ( rgb[ 0 ] * 255.f ),
			static_cast<int> ( rgb[ 1 ] * 255.f ),
			static_cast<int> ( rgb[ 2 ] * 255.f )
		};
	}
};