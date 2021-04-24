/**
 * Copyright Carl Kelso, Apr 24, 2021.  All Rights Reserved.
 */

#ifndef SRC_SPRITE_H_
#define SRC_SPRITE_H_

#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_render.h>

class Sprite {
protected:
    SDL_Renderer * m_r;
    SDL_Texture * m_sheet;
public:
    Sprite( SDL_Renderer * r, const char name[] );

    void load_sheet( const char name[] );

    virtual void render( size_t index, int x, int y ) = 0;

    virtual ~Sprite();
};

class UniformSprite : public Sprite {
private:
    int sprite_width;
    int sprite_height;
public:
    UniformSprite( SDL_Renderer * r, const char name[], int width, int height );
    void render( size_t index, int x, int y ) override;
    virtual ~UniformSprite();
};






#endif /* SRC_SPRITE_H_ */
