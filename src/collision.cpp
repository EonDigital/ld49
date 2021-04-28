/**
 * Copyright Carl Kelso, Apr 26, 2021.  All Rights Reserved.
 */

#include "collision.h"
#include "utilities.h"

bool collision_aabb_aabb( SDL_Rect & one, SDL_Rect & two ) {
    bool test_x = ( one.x < two.x )
        ? ( two.x < one.x + one.w )
            : one.x < two.x + two.w;
    bool test_y = ( one.y < two.y )
            ? ( two.y < one.y + one.h )
                : one.y < two.y + two.h;
    return test_x && test_y;
}

bool collision_aabb_aabb_dt( SDL_Rect & a, SDL_Rect & b, iv2_t & va_rel_vb, iv2_t * vb_normal ) {
    iv2_t vt = va_rel_vb;

    // Row can collide
    const int dy = b.y - a.y;
    const int dx = b.x - a.x;
    const int sx = vt.x < 0 ? -1 : 1;
    const int sy = vt.y < 0 ? -1 : 1;
    int cx = 0, cy = 0;
    if ( ( vt.x != 0 ) && ( ( (unsigned) dy < (unsigned) a.h ) || ( (unsigned) -dy < (unsigned) b.h ) ) ) {
        const int dxp = dx + ( ( vt.x < 0 ) ? b.w : -a.w );
        if ( (unsigned) (dxp * sx) <= (unsigned) (vt.x * sx) ) {
            cx = sx;
            vt.x = dxp;
        }
    }

    if ( ( vt.y != 0 ) && ( ( (unsigned) dx < (unsigned) a.w ) || ( (unsigned) -dx < (unsigned) b.w ) ) ) {
        volatile int dyp = dy + ( ( vt.y < 0 ) ? b.h : -a.h );
        if ( (unsigned) (dyp * sy) <= (unsigned) (vt.y * sy) ) {
            cy = sy;
            vt.y = dyp;
        }
    }

    va_rel_vb = vt;
    if ( vb_normal ) { *vb_normal += iv2_t{ cx, cy }; }
    return (cx != 0) || (cy != 0);
}


//bool collision_aabb_aabb_dt_1( SDL_Rect & a, SDL_Rect & b, iv2_t & va_rel_vb, iv2_t * vb_normal ) {
//    int cx = 0, cy = 0;
//    iv2_s vt = va_rel_vb;
//    if ( va_rel_vb.x < 0 ) {
//        // a is moving left
//        int dx = (b.x + b.w - 1) - a.x;
//        if ( ( dx < 0 ) && ( dx >= vt.x ) ) {
//            cx = 1;
////            vt.y = round_div( vt.y * (dx+cx), vt.x );
//            vt.x = (dx+cx);
//        }
//    } else if ( va_rel_vb.x > 0 ) {
//        // a is moving right
//        int dx = b.x - (a.x + a.w - 1);
//        if ( ( dx > 0 ) && ( dx <= vt.x ) ) {
//            cx = -1;
////            vt.y = round_div( vt.y * (dx-cx), vt.x );
//            vt.x = (dx+cx);
//        }
//    }
//
//    if ( va_rel_vb.y < 0 ) {
//        // a is moving up
//        int dy = (b.y + b.h - 1) - a.y;
//        if ( ( dy < 0 ) && ( dy >= vt.y ) ) {
//            cy = 1;
////            vt.x = round_div( vt.x * (dy+cy), vt.y );
//            vt.y = (dy+cy);
//        }
//    } else if ( va_rel_vb.y > 0 ) {
//        // a is moving down
//        int dy = b.y - (a.y + a.h - 1);
//        if ( ( dy > 0 ) && ( dy <= vt.y ) ) {
//            cy = -1;
////            vt.x = round_div( vt.x * (dy-cy), vt.y );
//            vt.y = (dy+cy);
//        }
//    }
//
//    va_rel_vb = vt;
//    if ( vb_normal ) { *vb_normal += iv2_t{ cx, cy }; }
//    return (cx != 0) || (cy != 0);
//}
