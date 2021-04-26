/**
 * Copyright Carl Kelso, Apr 24, 2021.  All Rights Reserved.
 */

#ifndef SRC_SPRITE_H_
#define SRC_SPRITE_H_

#include "spriteatlas.h"

class Sprite {
protected:
    class SpriteAtlas * mp_atlas;

public:
    Sprite( SpriteAtlas * p_atlas );

    virtual void render( size_t index, int x, int y, SDL_RendererFlip flip ) = 0;
    virtual int height( size_t index ) = 0;
    virtual int width( size_t index ) = 0;
    virtual ~Sprite() = default;

    SpriteAtlas & atlas();


};

class UniformSprite : public Sprite {
private:
    int sprite_width;
    int sprite_height;
public:
    UniformSprite( SpriteAtlas * p_atlas, int width, int height );
    void render( size_t index, int x, int y, SDL_RendererFlip flip ) override;
    int height( size_t index ) override;
    int width( size_t index ) override;
    virtual ~UniformSprite() = default;
};






#endif /* SRC_SPRITE_H_ */
