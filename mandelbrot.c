// #define OUTPUT

#ifdef OUTPUT
#include <stdio.h>
#endif

#include "conio.h"
#include "TXLib.h"
#include "config.h"


int main(){

    txCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT);
    Win32::_fpreset();
    txBegin();

    float xC = 0;       // Actual x coordinate of the left top corner of the window
    float yC = 0;       // Actual y coordinate of the left top corner of the window
    float scale  = 1;   // Shows, how much user zoomed in/out

    int count       = 0;   // counter of iterations
    int color       = 0;
    float Mald_x    = 0;
    float Mald_y    = 0;
    float prev_x    = 0;
    float prev_y    = 0;
    float start_x   = 0;
    float start_y   = 0;

    float tmp = 0;

    for (;;) {
        //------------------------------------------------------------------
        //| Start of the keyStrokes handler
        //------------------------------------------------------------------
        if ( txGetAsyncKeyState ( VK_RIGHT ) || txGetAsyncKeyState ( 'D' ) )
            xC += scale * ( MOVEMENT_SPEED + MOVEMENT_SPEED * txGetAsyncKeyState( VK_SHIFT ) * 9 );
        if ( txGetAsyncKeyState ( VK_LEFT ) || txGetAsyncKeyState ( 'A' ) )
            xC -= scale * ( MOVEMENT_SPEED + MOVEMENT_SPEED * txGetAsyncKeyState( VK_SHIFT ) * 9 );

        if ( txGetAsyncKeyState ( VK_DOWN ) || txGetAsyncKeyState ( 'S' ) )
            yC += scale * ( MOVEMENT_SPEED + MOVEMENT_SPEED * txGetAsyncKeyState( VK_SHIFT ) * 9 );
        if ( txGetAsyncKeyState ( VK_UP   ) || txGetAsyncKeyState ( 'W' ) )
            yC -= scale * ( MOVEMENT_SPEED + MOVEMENT_SPEED * txGetAsyncKeyState( VK_SHIFT ) * 9 );

        if ( txGetAsyncKeyState ( 'Z' ) ) {
            xC /= SCALE_MODIFIER;
            yC /= SCALE_MODIFIER;
            scale /= SCALE_MODIFIER;
        }
        if ( txGetAsyncKeyState ( 'X' ) ) {
            xC *= SCALE_MODIFIER;
            yC *= SCALE_MODIFIER;
            scale *= SCALE_MODIFIER;
        }


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
            for ( int xPos = 0; xPos < WINDOW_WIDTH; xPos++ ) {

                color = START_COLOR;
                count = MAXIMUM_ITERATIONS;
                start_x = xC + xPos * scale;
                start_y = yC + yPos * scale;

                prev_x = start_x;
                prev_y = start_y;


                while (count--){
                    Mald_x = prev_x * prev_x - prev_y * prev_y + start_x;
                    Mald_y = 2 * prev_x * prev_y + start_y;
                    color = color * color + START_COLOR;

                    tmp = Mald_x * Mald_x + Mald_y * Mald_y - start_y * start_y - start_x * start_x;
                    if ( tmp >  RADIUS * RADIUS || tmp < -RADIUS * RADIUS ){

                        break;
                    }
                    prev_x = Mald_x;
                    prev_y = Mald_y;

                }

                window[xPos + (-yPos + WINDOW_HEIGHT - 1) * WINDOW_WIDTH].rgbBlue    = color % 256;
                color >>= 8;
                window[xPos + (-yPos + WINDOW_HEIGHT - 1) * WINDOW_WIDTH].rgbGreen   = color % 256;
                color >>= 8;
                window[xPos + (-yPos + WINDOW_HEIGHT - 1) * WINDOW_WIDTH].rgbRed     = color % 256;
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
