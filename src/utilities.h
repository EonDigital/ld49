/**
 * Copyright Carl Kelso, Apr 25, 2021.  All Rights Reserved.
 */

#ifndef SRC_UTILITIES_H_
#define SRC_UTILITIES_H_

#include <type_traits>

template <typename E> void clean_delete( E ** pp_val ) {
    E * p_val = *pp_val;
    if ( p_val ) {
        *pp_val = nullptr;
        delete( p_val );
    }
}

template <typename E> E clamp( E low, E val, E high ) {
    if ( val < low ) {
        return low;
    }
    if ( val > high ) {
        return high;
    }
    return val;
}

template <
    typename E,
    std::enable_if_t< std::is_integral<E>::value, bool > = true
>
static inline E round_div( E num, E den ) {
    return (num + (((num^den)>=0)?den:-den)/2)/den;
}

#endif /* SRC_UTILITIES_H_ */
