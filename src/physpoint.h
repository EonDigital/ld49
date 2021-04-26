/**
 * Copyright Carl Kelso, Apr 26, 2021.  All Rights Reserved.
 */

#ifndef SRC_PHYSPOINT_H_
#define SRC_PHYSPOINT_H_

#include "point.h"

typedef int q8_t;

class PhysPoint {
public:
    iv2_t p;
    iv2_t v;
    iv2_t a;

    iv2_t get_delta( q8_t dt ) {
        return ( v + a * dt / ( 2 * 256 ) ) * dt / 256;
    }

    iv2_t get_destination( q8_t dt ) {
        return p + get_delta( dt );
    }

    void advance( q8_t dt ) {
        p += get_delta( dt );
        v += a * dt / 256;
    }
};


#endif /* SRC_PHYSPOINT_H_ */
