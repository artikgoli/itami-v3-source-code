#pragma once
class frame_limiter
{
public:
    explicit frame_limiter( int target_fps )
    {
        timeBeginPeriod( 1 );

        LARGE_INTEGER f;
        QueryPerformanceFrequency( &f );
        freq = double( f.QuadPart );
        target_ticks = freq / target_fps;

        QueryPerformanceCounter( &f );
        last = double( f.QuadPart );
    }

    ~frame_limiter( )
    {
        timeEndPeriod( 1 );
    }

    void limit( )
    {
        LARGE_INTEGER li;
        QueryPerformanceCounter( &li );
        double now = double( li.QuadPart );

        last += target_ticks;

        double remaining = last - now;
        if ( remaining > 0.0 )
        {
            if ( remaining > freq * 0.002 )
                Sleep( DWORD( ( remaining / freq ) * 1000.0 ) - 1 );

            do {
                QueryPerformanceCounter( &li );
                now = double( li.QuadPart );
            } while ( now < last );
        }
        else
        {
            last = now;
        }
    }

    void set_fps( int fps )
    {
        target_ticks = freq / fps;
    }

private:
    double freq {};
    double target_ticks {};
    double last {};
};
