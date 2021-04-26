/**
 * Copyright Carl Kelso, Apr 25, 2021.  All Rights Reserved.
 */

#ifndef SRC_SPRITEATLAS_H_
#define SRC_SPRITEATLAS_H_

#include <SDL2/SDL_render.h>
#include "sprite.h"

class SpriteAtlas {
private:
    SDL_Texture * m_sheet;
    SDL_Renderer * m_r;
public:
    friend class Sprite;
    friend class UniformSprite;
    SpriteAtlas( SDL_Renderer * r, const char * name );
    void load_sheet( const char * name );
    void render( SDL_Rect &clip, SDL_Rect &dest );
    ~SpriteAtlas();
};



#endif /* SRC_SPRITEATLAS_H_ */
