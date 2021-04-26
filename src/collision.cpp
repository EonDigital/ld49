/**
 * Copyright Carl Kelso, Apr 26, 2021.  All Rights Reserved.
 */

#include "collision.h"

bool collision_aabb_aabb( SDL_Rect & one, SDL_Rect & two ) {
    bool test_x = ( one.x < two.x )
        ? ( two.x < one.x + one.w )
            : one.x < two.x + two.w;
    bool test_y = ( one.y < two.y )
            ? ( two.y < one.y + one.h )
                : one.y < two.y + two.h;
    return test_x && test_y;
}

bool collision_aabb_aabb_dt( SDL_Rect & a, SDL_Rect & b, iv2_s & va_rel_vb ) {
    bool collision = false;
    iv2_s vt = va_rel_vb;
    if ( vt.x < 0 ) {
        // a is moving left
        int dx = (b.x + b.w - 1) - a.x;
        if ( ( dx < 0 ) && ( dx > vt.x ) ) {
            collision = true;
            vt.y = vt.y * (dx+1) / vt.x;
            vt.x = (dx+1);
        }
    } else if ( vt.x > 0 ) {
        // a is moving right
        int dx = b.x - (a.x + a.w - 1);
        if ( ( dx > 0 ) && ( dx < vt.x ) ) {
            collision = true;
            vt.y = vt.y * (dx-1) / vt.x;
            vt.x = (dx-1);
        }
    }

    if ( va_rel_vb.y < 0 ) {
        // a is moving up
        int dy = (b.y + b.h - 1) - a.y;
        if ( ( dy < 0 ) && ( dy > vt.y ) ) {
            collision = true;
            vt.x = vt.x * (dy+1) / vt.y;
            vt.y = (dy+1);
        }
    } else if ( va_rel_vb.y > 0 ) {
        // a is moving down
        int dy = b.y - (a.y + a.h - 1);
        if ( ( dy > 0 ) && ( dy < vt.y ) ) {
            collision = true;
            vt.x = vt.x * (dy-1) / vt.y;
            vt.y = (dy-1);
        }
    }

    va_rel_vb = vt;
    return collision;
}
