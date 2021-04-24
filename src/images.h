/**
 * Copyright Carl Kelso, Apr 24, 2021.  All Rights Reserved.
 */

#ifndef IMAGES_H_
#define IMAGES_H_

#include <SDL2/SDL_surface.h>

/**
 * Initialize the images module.
 */
int init_sdl_images();

void cleanup_sdl_images();

SDL_Surface * load_image( const char name[] );

SDL_Surface * load_optimized_image( const char name[], SDL_Surface * target_surface );

#endif // IMAGES_H_
