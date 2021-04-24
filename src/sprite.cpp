/**
 * Copyright Carl Kelso, Apr 24, 2021.  All Rights Reserved.
 */


#include "sprite.h"
#include "images.h"

Sprite::Sprite( SDL_Renderer * r, const char name[] )
    : m_r(r), m_sheet(nullptr)
{
    load_sheet( name );
}

void Sprite::load_sheet( const char * name ) {
    SDL_Surface * sheet = load_image(name);
    if ( !sheet ) {
        return;
    }
    SDL_Texture * tex = SDL_CreateTextureFromSurface(m_r, sheet);
    if ( tex ) {
        if ( m_sheet ) {
            SDL_DestroyTexture(m_sheet);
        }
        m_sheet = tex;
    }
}

Sprite::~Sprite() {
    if (m_sheet) {
        SDL_DestroyTexture(m_sheet);
    }
}

UniformSprite::UniformSprite( SDL_Renderer * r, const char * name, int width, int height ) :
    Sprite(r, name), sprite_width(width), sprite_height(height) {
}

void UniformSprite::render( size_t index, int x, int y ) {

    int s_x = index & 0xFFFF;
    int s_y = (index >> 16) & 0xFFFF;
    SDL_Rect clip;
    clip.x = s_x * sprite_width;
    clip.y = s_y * sprite_height;
    clip.w = sprite_width;
    clip.h = sprite_height;

    SDL_Rect dest = { x, y, clip.w, clip.h };

    SDL_RenderCopy(m_r, m_sheet, &clip, &dest);
}

UniformSprite::~UniformSprite() {}

