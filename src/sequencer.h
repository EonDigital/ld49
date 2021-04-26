/**
 * Copyright Carl Kelso, Apr 25, 2021.  All Rights Reserved.
 */

#ifndef SRC_SEQUENCER_H_
#define SRC_SEQUENCER_H_

#include <cstdint>

class Sequencer {
public:
    virtual uint8_t get_next() = 0;
    virtual uint8_t get_current() = 0;
    virtual uint8_t reset() = 0;
    virtual ~Sequencer() = default;
};

class PatternSequencer : public Sequencer {
protected:
    uint8_t m_base;
    uint8_t m_step;
    int8_t m_count;
    int8_t m_index;
public:
    PatternSequencer( uint8_t base, uint8_t step, int8_t count );
    uint8_t get_current() override;
    uint8_t reset() override;
    virtual ~PatternSequencer() = default;
};

class OneShotSequencer : public PatternSequencer {
public:
    OneShotSequencer( uint8_t base, uint8_t step, int8_t count );
    uint8_t get_next() override;
    virtual ~OneShotSequencer() = default;
};

class LoopSequencer : public PatternSequencer {
public:
    LoopSequencer( uint8_t base, uint8_t step, int8_t count );
    uint8_t get_next() override;
    virtual ~LoopSequencer() = default;
};

class PingPongSequencer : public PatternSequencer {
public:
    PingPongSequencer( uint8_t base, uint8_t step, int8_t count );
    uint8_t get_next() override;
    virtual ~PingPongSequencer() = default;
};

class SlowSequencer : public Sequencer {
private:
    uint8_t m_delay;
    uint8_t m_index;
    Sequencer * m_child;
public:
    /**
     * @param child A sequencer owned by this.  Will be cleaned up by this.
     */
    SlowSequencer( uint8_t delay, Sequencer * child );
    uint8_t get_next() override;
    uint8_t get_current() override;
    uint8_t reset() override;
    ~SlowSequencer() override;
};

#endif /* SRC_SEQUENCER_H_ */
