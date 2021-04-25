/**
 * Copyright Carl Kelso, Apr 24, 2021.  All Rights Reserved.
 */


#include "sprite.h"


// Set the size of an artistic pixel, in real pixels.
enum { SPRITE_SF = 4 };

Sprite::Sprite( SDL_Renderer * r, SpriteAtlas * p_atlas )
    : m_r(r), mp_atlas(p_atlas)
{

}

Sprite::~Sprite() {
}

UniformSprite::UniformSprite( SDL_Renderer * r, SpriteAtlas * p_atlas, int width, int height ) :
    Sprite(r, p_atlas), sprite_width(width), sprite_height(height) {
}

void UniformSprite::render( size_t index, int x, int y ) {
    if ( !mp_atlas || !mp_atlas->m_sheet ) { return; }

    int s_x = index & 0xF;
    int s_y = (index >> 4) & 0xF;
    SDL_Rect clip;
    clip.x = s_x * sprite_width;
    clip.y = s_y * sprite_height;
    clip.w = sprite_width;
    clip.h = sprite_height;

    int sf = SPRITE_SF;

    SDL_Rect dest = { x, y, clip.w * sf, clip.h * sf};

    SDL_RenderCopy(m_r, mp_atlas->m_sheet, &clip, &dest);
}

UniformSprite::~UniformSprite() {}

