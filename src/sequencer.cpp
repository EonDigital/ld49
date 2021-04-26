/**
 * Copyright Carl Kelso, Apr 25, 2021.  All Rights Reserved.
 */
#include "sequencer.h"
#include "utilities.h"

PatternSequencer::PatternSequencer( uint8_t base, uint8_t step, int8_t count )
        : m_base(base), m_step(step), m_count(count), m_index(0) {}

uint8_t PatternSequencer::get_current() {
    return m_base + m_step * m_index;
}

uint8_t PatternSequencer::reset() {
    m_index = 0;
    return get_current();
}

OneShotSequencer::OneShotSequencer( uint8_t base, uint8_t step, int8_t count )
        : PatternSequencer(base, step, count ) {}


uint8_t OneShotSequencer::get_next() {
    if ( m_index + 1 < m_count ) {
        ++m_index;
    }
    return m_index;
}

LoopSequencer::LoopSequencer( uint8_t base, uint8_t step, int8_t count )
        : PatternSequencer(base, step, count ) {}

uint8_t LoopSequencer::get_next() {
    m_index = ( m_index + 1 < m_count ) ? ( ++m_index ) : 0;
    return m_index;
}


PingPongSequencer::PingPongSequencer( uint8_t base, uint8_t step, int8_t count )
        : PatternSequencer(base, step, count ) {}

uint8_t PingPongSequencer::get_next() {
    if ( ++m_index > m_count ) {
        m_index = 1 - m_count;
    }
    return ( m_index < 0 ) ? -m_index : m_index;
}

SlowSequencer::SlowSequencer( uint8_t delay, Sequencer * child )
    : m_delay(delay), m_child(child), m_index(0) {}

uint8_t SlowSequencer::get_current() {
    return m_child ? m_child->get_current() : 0;
}

uint8_t SlowSequencer::get_next() {
    if ( !m_child ) { return 0; }
    if ( ++m_index >= m_delay ) {
        m_index = 0;
        return m_child->get_next();
    } else {
        return m_child->get_current();
    }
}

uint8_t SlowSequencer::reset() {
    m_index = 0;
    return m_child ? m_child->reset() : 0;
}

SlowSequencer::~SlowSequencer() {
    clean_delete( &m_child );
}


