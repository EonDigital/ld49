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

    virtual void render( size_t index, int x, int y ) = 0;

    virtual ~Sprite() = default;
};

class UniformSprite : public Sprite {
private:
    int sprite_width;
    int sprite_height;
public:
    UniformSprite( SpriteAtlas * p_atlas, int width, int height );
    void render( size_t index, int x, int y ) override;
    virtual ~UniformSprite() = default;
};






#endif /* SRC_SPRITE_H_ */
