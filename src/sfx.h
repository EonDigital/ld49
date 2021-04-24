/**
 * Copyright Carl Kelso, Apr 24, 2021.  All Rights Reserved.
 */

#ifndef SRC_SFX_H_
#define SRC_SFX_H_

#include <SDL2/SDL_mixer.h>

class Sfx;

class ChannelLink {
public:
    ChannelLink( Sfx * owner );
    ChannelLink * next = nullptr;
    Sfx * owner;
    int channel = -1;

public:
    friend Sfx;

};

class Sfx {
private:
    static ChannelLink * links;
    Mix_Chunk * m_chunk;
    ChannelLink link = ChannelLink(this);
public:
    void load( const char * filename );
    void play();
    static void channel_finished( int channel );
    void finished( int channel );
    ~Sfx();
};




#endif /* SRC_SFX_H_ */
