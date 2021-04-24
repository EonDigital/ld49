#include "images.h"

#include <SDL2/SDL_image.h>
#include "logging.h"

int init_sdl_images() {
    int flags = IMG_INIT_PNG;

    // Make sure all requested entries are supported.
    if ( IMG_Init( flags ) & flags != flags ) {
        LOG_ERROR( "Image Init: Unable to load support for all image types %4x", flags );
        LOG_ERROR( "Image Init: %s", IMG_GetError() );
        IMG_Quit();
        return 1;
    }
    return 0;
}

void cleanup_sdl_images() {
    IMG_Quit();
}

SDL_Surface * load_image( const char name[] ) {
    SDL_Surface * temp = IMG_Load( name );
    if ( !temp ) {
        LOG_ERROR( "Unable to load %s", name );
        LOG_ERROR( "Image Init: %s", IMG_GetError() );
    }
    return temp;
}

SDL_Surface * load_optimized_image( const char name[], SDL_Surface * target_surface ) {
    SDL_Surface * temp = IMG_Load( name );
    SDL_Surface * optimized = nullptr;
    if ( !temp ) {
        LOG_ERROR( "Unable to load %s", name );
        LOG_ERROR( "Image Init: %s", IMG_GetError() );
    } else {
        optimized = SDL_ConvertSurface( temp, target_surface->format, 0 );
        if ( !optimized ) {
            LOG_ERROR( "Unable to optimize %s", name );
            return temp;
        } else {
            SDL_FreeSurface( temp );
        }
    }
    return optimized;
}
