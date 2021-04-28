/**
 * Copyright Carl Kelso, Apr 26, 2021.  All Rights Reserved.
 */

#ifndef SRC_COLLISION_H_
#define SRC_COLLISION_H_

#include <SDL2/SDL_rect.h>

#include "point.h"

/**
 * Find out if two boxes overlap.
 * @param one
 * @param two
 * @return
 */
bool collision_aabb_aabb( SDL_Rect & one, SDL_Rect & two );

/**
 * Find a collision between two boxes moving relative to each other, and where it is.
 * @param a
 * @param b
 * @param va_rel_vb in: the distance a moves relative to b, out: the distance a can move before a collision.
 * @param vb_normal optional, the orientation of the impact surface
 * @return true if there is a collision within va_rel_vb
 */
bool collision_aabb_aabb_dt( SDL_Rect & a, SDL_Rect & b, iv2_t & va_rel_vb, iv2_t * vb_normal = NULL );

#endif /* SRC_COLLISION_H_ */
