/**
 * Copyright Carl Kelso, Apr 25, 2021.  All Rights Reserved.
 */

#include "spriteatlas.h"

#include <SDL2/SDL_surface.h>

#include "images.h"

SpriteAtlas::SpriteAtlas( SDL_Renderer * r, const char * name ) : m_r(r) {
    load_sheet(name);
}

void SpriteAtlas::load_sheet( const char * name ) {
    SDL_Surface * surface = load_image(name);
    if ( !surface ) {
        return;
    }
    // Choose 1 1 1 as a magic invisible color we're unlikely to use anywhere else
    SDL_SetColorKey(surface, true, SDL_MapRGB(surface->format, 1, 1, 1));
    SDL_Texture * tex = SDL_CreateTextureFromSurface(m_r, surface);
    if ( tex ) {
        if ( m_sheet ) {
            SDL_DestroyTexture(m_sheet);
        }
        m_sheet = tex;
    }
}

SpriteAtlas::~SpriteAtlas() {
    if (m_sheet) {
        SDL_DestroyTexture(m_sheet);
    }
}
