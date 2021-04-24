#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <stdio.h>
#include "logging.h"
#include "strings.h"

enum {
    DEFAULT_HEIGHT = 768,
    DEFAULT_WIDTH  = 1024,
};

typedef struct {
    int width;
    int height;
    Uint32 flags;
    SDL_Window * win;
} SystemState;

SystemState s;

int main( int argc, char ** argv ) {

    s.height = DEFAULT_HEIGHT;
    s.width = DEFAULT_WIDTH;
    s.flags = SDL_WINDOW_RESIZABLE;

    SDL_SetMainReady();
    if ( SDL_Init( SDL_INIT_EVERYTHING ) != 0 ) {
        LOG_ERROR( "Failed to get SDL running %s", SDL_GetError() );
    }

    s.win = SDL_CreateWindow( str_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, s.width, s.height, s.flags );

    if ( s.win == nullptr ) {
        LOG_ERROR( "Failed to create SDL Window %s", SDL_GetError() );
    }

    printf("Hello World\n");

    if ( s.win ) {
        SDL_DestroyWindow(s.win);
    }
    SDL_Quit();

    return 0;
}
