// #define OUTPUT

#ifdef OUTPUT
#include <stdio.h>
#endif

#include "conio.h"
#include "TXLib.h"
#include "config.h"

//TODO
inline void mm_set_int      ( int mm[4], int val )                                      ;
inline int  mm_gz_int_val   ( int mm[4], int val )                                      ;
inline void mm_sub_int_val  ( int mm[4], int val,          const int mask[4] )          ;
inline void mm_add_int_val  ( int mm[4], int val,          const int mask[4] )          ;
inline void mm_mul_int_val  ( int mm[4], int val,          const int mask[4] )          ;
inline void mm_mul_int_mm2  ( int mm[4], const int mm2[4], const int mask[4] )          ;
inline void mm_shr_int_val  ( int mm[4], int val )                                      ;

inline void mm_set_fl       ( float mm[4], float val )                                  ;
inline void mm_cpy_fl       ( float mm[4], const float mm2[4] )                         ;
inline void mm_add_fl_val   ( float mm[4], float val,          const int mask[4] )      ;
inline void mm_add_fl_mm2   ( float mm[4], const float mm2[4], const int mask[4] )      ;
inline void mm_mul_fl_val   ( float mm[4], float val,          const int mask[4] )      ;
inline void mm_mul_fl_mm2   ( float mm[4], const float mm2[4], const int mask[4] )      ;
inline void mm_sub_fl_val   ( float mm[4], float val,          const int mask[4] )      ;
inline void mm_sub_fl_mm2   ( float mm[4], const float mm2[4], const int mask[4] )      ;

void HandleKeyStrokes( float* xC, float* yC, float* scale );


int main(){

    txCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT);
    Win32::_fpreset();
    txBegin();

    float xC = 0;       // Actual x coordinate of the left top corner of the window
    float yC = 0;       // Actual y coordinate of the left top corner of the window
    float scale  = 0.025;   // Shows, how much user zoomed in/out

    int count[4]       = {};   // counter of iterations
    int color[4]       = {};   // current color
    bool mask[4]       = {};
    float Mald_x[4]    = {};   // X position in Mandelbrot iterations
    float Mald_y[4]    = {};   // Y position in Mandelbrot iterations
    float prev_x[4]    = {};   // previous X position in Mandelbrot iterations
    float prev_y[4]    = {};   // previous Y position in Mandelbrot iterations
    float start_x[4]   = {};   // X position before Mandelbrot iterations
    float start_y[4]   = {};   // X position before Mandelbrot iterations

    float tmp[4]       = {};

    for (;;) {

        HandleKeyStrokes( &xC, &yC, &scale );

        #ifdef OUTPUT
            printf( "-------------\nCURRENT STATE\n-------------" );
            printf( "x Left Corner: %lf\n"
                    "y Left Corner: %lf\n"
                    "scale: %lf\n\n",
                     xC, yC, scale);
        #endif

        txLock();

        RGBQUAD* window = txVideoMemory();
        for ( int yPos = 0; yPos < WINDOW_HEIGHT; yPos++ ) {
            for ( int xPos = 0; xPos < WINDOW_WIDTH; xPos+=4 ) {

                mm_set_int( color, DEFAULT_COLOR );
                mm_set_int( count, MAXIMUM_ITERATIONS );

                mm_set_fl( start_x, xC + xPos * scale );
                mm_set_fl( start_y, yC + yPos * scale );

                mm_cpy_fl( prev_x, start_x );
                mm_cpy_fl( prev_y, start_y );


                while ( mm_gz_int_val( count, 0 ) ) {
                    mm_cpy_fl    ( Mald_y, prev_x   );
                    mm_mul_fl_mm2( Mald_y, prev_y   );
                    mm_mul_fl_val( Mald_y, 2        );
                    mm_add_fl_mm2( Mald_y, start_y  );

                    mm_mul_fl_mm2( prev_x, prev_x   );
                    mm_mul_fl_mm2( prev_y, prev_y   );
                    mm_sub_fl_mm2( prev_x, prev_y   );
                    mm_cpy_fl    ( Mald_x, prev_y   );
                    mm_add_fl_mm2( Mald_x, start_x  );

                    mm_mul_int_mm2( color, color );
                    mm_add_int_val( color, START_COLOR );

                    mm_cpy_fl( prev_y, Mald_y );
                    mm_mul_fl_mm2( prev_y, Mald_y );

                    mm_cpy_fl( tmp, Mald_x );
                    mm_mul_fl_mm2( tmp, Mald_x );

                    mm_add_fl_mm2( tmp, prev_y );


                    mm_cpy_fl( prev_x, start_x );
                    mm_cpy_fl( prev_y, start_y );
                    mm_mul_fl_mm2( prev_x, prev_x );
                    mm_mul_fl_mm2( prev_y, prev_y );

                    mm_sub_fl_mm2( tmp, prev_y );
                    mm_sub_fl_mm2( tmp, prev_x );

                    for ( int i = 0; i < 4; i++ )
                        if ( tmp[i] > RADIUS * RADIUS || tmp[i] < -RADIUS * RADIUS )
                            count[i] = -1;

                    mm_cpy_fl( prev_x, Mald_x );
                    mm_cpy_fl( prev_y, Mald_y );

                    mm_sub_int_val( count, 1 );

                }

                if ( count[0] == 0 || count[1] == 0 )
                    mm_set_int( color, DEFAULT_COLOR );

                for ( int i = 0; i < 4; i++ ){
                    window[xPos + (-yPos + WINDOW_HEIGHT - 1) * WINDOW_WIDTH + i].rgbBlue    = color[i] % 256;
                    mm_shr_int_val( color, 8 );
                    window[xPos + (-yPos + WINDOW_HEIGHT - 1) * WINDOW_WIDTH + i].rgbGreen   = color[i] % 256;
                    mm_shr_int_val( color, 8 );
                    window[xPos + (-yPos + WINDOW_HEIGHT - 1) * WINDOW_WIDTH + i].rgbRed     = color[i] % 256;
                }
            }
        }

        txUnlock();

        txRedrawWindow();

        printf("%lf\n", txGetFPS());

        //------------------------------------------------------------------
        //| End of the keyStrokes handler
        //------------------------------------------------------------------

    }
}


inline void mm_set_int      ( int mm[4], int val )                  { for ( int i = 0; i < 4; i++ ) mm[i] = val;        }

inline int  mm_gz_int_val   ( int mm[4], int val )                  { int cnt = 0; for ( int i = 0; i < 4; i++ ) cnt += mm[i] > val; return cnt; }

inline void mm_sub_int_val  ( int mm[4], int val, const int mask[4] )                  { for ( int i = 0; i < 4; i++ ) mm[i] -= val;       }

inline void mm_add_int_val  ( int mm[4], int val )                  { for ( int i = 0; i < 4; i++ ) mm[i] += val;       }

inline void mm_mul_int_val  ( int mm[4], int val )                  { for ( int i = 0; i < 4; i++ ) mm[i] *= val;       }
inline void mm_mul_int_mm2  ( int mm[4], const int mm2[4] )         { for ( int i = 0; i < 4; i++ ) mm[i] *= mm2[i];    }

inline void mm_shr_int_val  ( int mm[4], int val )                  { for ( int i = 0; i < 4; i++ ) mm[i] >>= val;      }


inline void mm_set_fl       ( float mm[4], float val )              { for ( int i = 0; i < 4; i++ ) mm[i] = val;        }
inline void mm_cpy_fl       ( float mm[4], const float mm2[4] )     { for ( int i = 0; i < 4; i++ ) mm[i] = mm2[i];     }

inline void mm_add_fl_val   ( float mm[4], float val )              { for ( int i = 0; i < 4; i++ ) mm[i] += val;       }
inline void mm_add_fl_mm2   ( float mm[4], const float mm2[4] )     { for ( int i = 0; i < 4; i++ ) mm[i] += mm2[i];    }

inline void mm_mul_fl_val   ( float mm[4], float val )              { for ( int i = 0; i < 4; i++ ) mm[i] *= val;       }
inline void mm_mul_fl_mm2   ( float mm[4], const float mm2[4] )     { for ( int i = 0; i < 4; i++ ) mm[i] *= mm2[i];    }

inline void mm_sub_fl_val   ( float mm[4], float val )              { for ( int i = 0; i < 4; i++ ) mm[i] -= val;       }
inline void mm_sub_fl_mm2   ( float mm[4], const float mm2[4] )     { for ( int i = 0; i < 4; i++ ) mm[i] -= mm2[i];    }


void HandleKeyStrokes( float* xC, float* yC, float* scale ) {
    if ( txGetAsyncKeyState ( VK_RIGHT ) || txGetAsyncKeyState ( 'D' ) )
        *xC += *scale * ( MOVEMENT_SPEED + MOVEMENT_SPEED * txGetAsyncKeyState( VK_SHIFT ) * 9 );
    if ( txGetAsyncKeyState ( VK_LEFT ) || txGetAsyncKeyState ( 'A' ) )
        *xC -= *scale * ( MOVEMENT_SPEED + MOVEMENT_SPEED * txGetAsyncKeyState( VK_SHIFT ) * 9 );

    if ( txGetAsyncKeyState ( VK_DOWN ) || txGetAsyncKeyState ( 'S' ) )
        *yC += *scale * ( MOVEMENT_SPEED + MOVEMENT_SPEED * txGetAsyncKeyState( VK_SHIFT ) * 9 );
    if ( txGetAsyncKeyState ( VK_UP   ) || txGetAsyncKeyState ( 'W' ) )
        *yC -= *scale * ( MOVEMENT_SPEED + MOVEMENT_SPEED * txGetAsyncKeyState( VK_SHIFT ) * 9 );

    if ( txGetAsyncKeyState ( 'Z' ) ) {
        *xC /= SCALE_MODIFIER;
        *yC /= SCALE_MODIFIER;
        *scale /= SCALE_MODIFIER;
    }
    if ( txGetAsyncKeyState ( 'X' ) ) {
        *xC *= SCALE_MODIFIER;
        *yC *= SCALE_MODIFIER;
        *scale *= SCALE_MODIFIER;
    }
}
