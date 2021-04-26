/**
 * Copyright Carl Kelso, Apr 26, 2021.  All Rights Reserved.
 */

#ifndef SRC_POINT_H_
#define SRC_POINT_H_

typedef struct iv2_s {
    int x, y;
} iv2_t;

static inline iv2_t operator+( const iv2_t & a, const iv2_t & b ) {
    return iv2_t{ a.x+b.x, a.y+b.y };
}

static inline iv2_t operator*( const iv2_t & a, int b ) {
    return iv2_t{ a.x*b, a.y*b };
}

static inline iv2_t operator/( const iv2_t & a, int b ) {
    return iv2_t{ a.x/b, a.y/b };
}

static inline iv2_t & operator+=( iv2_t & a, const iv2_t & b ) {
    a.x += b.x;
    a.y += b.y;
    return a;
}

#endif /* SRC_POINT_H_ */
