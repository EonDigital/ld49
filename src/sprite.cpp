/**
 * Copyright Carl Kelso, Apr 24, 2021.  All Rights Reserved.
 */


#include "sprite.h"

Sprite::Sprite( SpriteAtlas * p_atlas )
    : mp_atlas(p_atlas)
{}

UniformSprite::UniformSprite( SpriteAtlas * p_atlas, int width, int height ) :
    Sprite(p_atlas), sprite_width(width), sprite_height(height) {
}

void UniformSprite::render( size_t index, int x, int y ) {
    if ( !mp_atlas ) { return; }

    int s_x = index & 0xF;
    int s_y = (index >> 4) & 0xF;
    SDL_Rect clip;
    clip.x = s_x * sprite_width;
    clip.y = s_y * sprite_height;
    clip.w = sprite_width;
    clip.h = sprite_height;

    SDL_Rect dest = { x, y, clip.w, clip.h };
    mp_atlas->render(clip, dest);
}

SpriteAtlas & Sprite::atlas() {
    return *mp_atlas;
}

int UniformSprite::height( size_t index ) {
    return ( index <= 0xFF ) ? sprite_height : 0;
}

int UniformSprite::width( size_t index ) {
    return ( index <= 0xFF ) ? sprite_width : 0;
}
