/**
 * Copyright Carl Kelso, Apr 25, 2021.  All Rights Reserved.
 */

#include "spriteatlas.h"

#include <SDL2/SDL_surface.h>

#include "images.h"

SpriteAtlas::SpriteAtlas( SDL_Renderer * p_r, const char * p_name ) : mp_r(p_r) {
    if ( p_name && p_r ) {
        load_sheet(p_name);
    }
}

void SpriteAtlas::load_sheet( const char * name ) {
    SDL_Surface * surface = load_image(name);
    if ( !surface ) {
        return;
    }
    // Choose 1 1 1 as a magic invisible color we're unlikely to use anywhere else
    SDL_SetColorKey(surface, true, SDL_MapRGB(surface->format, 1, 1, 1));
    SDL_Texture * tex = SDL_CreateTextureFromSurface(mp_r, surface);
    if ( tex ) {
        if ( m_sheet ) {
            SDL_DestroyTexture(m_sheet);
        }
        m_sheet = tex;
    }
}

SpriteAtlas::~SpriteAtlas() {
    // We do not own m_r.
    if (m_sheet) {
        SDL_DestroyTexture(m_sheet);
    }
}

void SpriteAtlas::render( SDL_Rect & clip, SDL_Rect & dest, SDL_RendererFlip flip ) {
    if ( mp_r && m_sheet ) {
        if ( flip == SDL_FLIP_NONE ) {
            SDL_RenderCopy(mp_r, m_sheet, &clip, &dest);
        } else {
            SDL_RenderCopyEx(mp_r, m_sheet, &clip, &dest, 0.0, NULL, flip );
        }
    }
}

SDL_Renderer * SpriteAtlas::renderer() {
    return mp_r;
}

SpriteAtlas SpriteAtlas::null_atlas = SpriteAtlas(nullptr, nullptr);
