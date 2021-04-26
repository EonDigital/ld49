/**
 * Copyright Carl Kelso, Apr 26, 2021.  All Rights Reserved.
 */

#ifndef SRC_PHYSPOINT_H_
#define SRC_PHYSPOINT_H_


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

class PhysPoint {
public:
    iv2_t p;
    iv2_t v;
    iv2_t a;

    iv2_t get_delta() {
        return v + a / 2;
    }

    iv2_t get_destination() {
        return p + v + a / 2;
    }

    void advance() {
        p += v + a / 2;
        v += a;
    }
};


#endif /* SRC_PHYSPOINT_H_ */
