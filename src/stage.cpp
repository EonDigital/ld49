/**
 * Copyright Carl Kelso, Apr 25, 2021.  All Rights Reserved.
 */

#include "stage.h"

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_rect.h>

#include "utilities.h"

Stage::Stage( Sprite * p_sprite, int x, int y )
        : m_x(x), m_y(y), mp_sprite(p_sprite)
{}

CompleteStage::CompleteStage( Sprite * p_sprite, size_t width, size_t height, const char * data, int x, int y )
        : Stage(p_sprite, x,y), m_height(height), m_width(width), m_data(data)
{}

void CompleteStage::move_camera( int delta_x, int delta_y ) {
    m_x += delta_x;
    m_y += delta_y;

    if ( m_x < 0 ) {
        m_x = 0;
    }
    if ( m_x >= m_width ) {
        m_x = m_width - 1;
    }

    if ( m_x < 0 ) {
        m_x = 0;
    }
    if ( m_x >= m_width ) {
        m_x = m_width - 1;
    }
}

void CompleteStage::render() {
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

    const int tile_height = mp_sprite->height(0);
    const int tile_width = mp_sprite->height(0);

    const int start_row = rect.y % tile_height;
    const int start_col = rect.x % tile_width;
    const int start_row_px = start_row * tile_height;
    const int start_col_px = start_col * tile_width;
    const int end_row_px = start_row_px + rect.h;
    const int end_col_px = start_col_px + rect.w;

    const int max_height = m_height - 1;
    const int max_width = m_width - 1;

    for ( int y = start_row_px, yi = 0; y < end_row_px; y += tile_height, ++yi ) {
        int yn = clamp( 0, yi + m_y, max_height );
        for ( int x = start_col_px, xi = 0; x < end_col_px; x += tile_width, ++xi ) {
            int xn = clamp( 0, xi + m_x, max_width );
            mp_sprite->render(m_data[yn * m_width + xn], x, y);
        }
    }
}
