/**
 * Copyright Carl Kelso, Apr 25, 2021.  All Rights Reserved.
 */

#include "stage.h"

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_rect.h>

#include "utilities.h"

Stage::Stage( Sprite * p_sprite, int x, int y )
        : m_x(x), m_y(y), mp_sprite(p_sprite)
{
    recalculate_view();
}

int Stage::x() {
    return m_x;
}

int Stage::y() {
    return m_y;
}

SDL_Rect & Stage::view() {
    return m_view;
}

void Stage::recalculate_view() {
    SDL_Renderer * r = mp_sprite->atlas().renderer();

    SDL_Rect rect;
    if ( SDL_RenderIsClipEnabled(r) ) {
        SDL_RenderGetClipRect(r, &rect);
    } else {
        rect.x = 0;
        rect.y = 0;
        SDL_RenderGetLogicalSize(r, &rect.w, &rect.h);
    }

    if (rect.w == 0 && rect.h == 0) {
        SDL_RenderGetViewport(r, &rect);
    }

    m_view.h = rect.h;
    m_view.w = rect.w;
    m_view.y = m_y - rect.h/2;
    m_view.x = m_x - rect.w/2;
}

CompleteStage::CompleteStage( Sprite * p_sprite, size_t width, size_t height, const char * data, int x, int y )
        : Stage(p_sprite, x,y), m_height(height), m_width(width), m_data(data)
{}

void CompleteStage::move_camera( int delta_x, int delta_y ) {
    m_x += delta_x;
    m_view.x += delta_x;
    m_y += delta_y;
    m_view.y += delta_y;

//    if ( m_x < 0 ) {
//        m_x = 0;
//    }
//    if ( m_x >= m_width ) {
//        m_x = m_width - 1;
//    }
//
//    if ( m_x < 0 ) {
//        m_x = 0;
//    }
//    if ( m_x >= m_width ) {
//        m_x = m_width - 1;
//    }
}

void CompleteStage::render() {
    SDL_Renderer * r = mp_sprite->atlas().renderer();

    const int tile_height = mp_sprite->height(0);
    const int tile_width = mp_sprite->width(0);

    const int view_c_y = m_view.h / 2;
    const int view_c_x = m_view.w / 2;

    // Special handling required for integer truncation with negative values
    const int trunc_y = (view_c_y > m_y) ? tile_height - 1 : 0;
    const int trunc_x = (view_c_x > m_x) ? tile_width - 1 : 0;
    const int world_s_yi = ( m_y - view_c_y - trunc_y ) / tile_height;
    const int world_s_xi = ( m_x - view_c_x - trunc_x ) / tile_width;

    const int world_s_y = world_s_yi * tile_height;
    const int world_s_x = world_s_xi * tile_width;

    const int max_height = m_height - 1;
    const int max_width = m_width - 1;

    const int view_s_y = view_c_y + world_s_y - m_y;
    const int view_s_x = view_c_x + world_s_x - m_x;

    const int view_e_y = view_s_y + m_view.h + tile_height - 1;
    const int view_e_x = view_s_x + m_view.w + tile_width - 1;

    for ( int y = view_s_y, yi = world_s_yi; y < view_e_y; y += tile_height, ++yi ) {
        int yn = clamp( 0, yi, max_height );
        for ( int x = view_s_x, xi = world_s_xi; x < view_e_x; x += tile_width, ++xi ) {
            int xn = clamp( 0, xi, max_width );
            mp_sprite->render(m_data[yn * m_width + xn], x, y, SDL_FLIP_NONE);
        }
    }
}
