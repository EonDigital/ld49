/**
 * Copyright Carl Kelso, Apr 25, 2021.  All Rights Reserved.
 */

#ifndef SRC_STAGE_H_
#define SRC_STAGE_H_

#include "spriteatlas.h"
#include "point.h"

enum { chunk_height = 12, chunk_width = 12 };


class Stage {
protected:
    int m_x;
    int m_y;
    SDL_Rect m_view;
    int m_off_y;
    Sprite * mp_sprite;

public:
    Stage( Sprite * p_sprite, int x = 0, int y = 0 );
    virtual void move_camera( int delta_x, int delta_y ) = 0;
    virtual void render() = 0;
    virtual void recalculate_view();
    virtual ~Stage() = default;
    virtual int x();
    virtual int y();
    virtual SDL_Rect & view();
    virtual bool test_collision( SDL_Rect & rect, iv2_t & delta, iv2_t * vb_normal ) = 0;
};

/// Use this for now to hold the entire stage we build.
/// If this needs to be flexible later, we replace with a smaller subclass.
class CompleteStage : public Stage {
private:
    size_t m_height;
    size_t m_width;
    const char * m_data;

public:
    CompleteStage( Sprite * p_sprite, size_t width, size_t height, const char * data, int x = 0, int y = 0 );
    void move_camera( int delta_x, int delta_y ) override;
    void render() override;
    bool test_collision( SDL_Rect & rect, iv2_t & delta, iv2_t * vb_normal ) override;
    virtual ~CompleteStage() = default;
};


#endif /* SRC_STAGE_H_ */
