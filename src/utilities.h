/**
 * Copyright Carl Kelso, Apr 25, 2021.  All Rights Reserved.
 */

#ifndef SRC_UTILITIES_H_
#define SRC_UTILITIES_H_

template <typename E> void clean_delete( E ** pp_val ) {
    E * p_val = *pp_val;
    if ( p_val ) {
        *pp_val = nullptr;
        delete( p_val );
    }
}

#endif /* SRC_UTILITIES_H_ */
