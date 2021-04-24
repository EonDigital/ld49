/**
 * Copyright Carl Kelso, Apr 24, 2021.  All Rights Reserved.
 */


#include "sfx.h"
#include "logging.h"

ChannelLink::ChannelLink(Sfx * o) : owner(o) {}

void Sfx::load(const char * filename) {
    Mix_Chunk * c = Mix_LoadWAV( filename );
    if ( !c ) {
        LOG_ERROR( "Failed to load a sound effect: %s", Mix_GetError());
    } else {
        if ( m_chunk ) {
            Mix_FreeChunk(m_chunk);
        }
        m_chunk = c;
    }
}

void Sfx::play() {
    if ( m_chunk ) {
        int temp = Mix_PlayChannel( -1, m_chunk, 0 );
        if ( temp == -1 ) {
            LOG_ERROR("Failed to play sound: %s", Mix_GetError());
        }
    }
}

void Sfx::finished( int channel ) {
    // do nothing for now
}

void Sfx::channel_finished(int channel) {

    // Handle the case where we're modifying the top
    while ( links && links->channel == channel ) {
        ChannelLink *temp = links;
        links = temp->next; // This effectively removes the link quickly... potential race condition in multithreaded code.
        temp->next = nullptr; // Deny access to the callback
        temp->owner->finished(channel);
        temp->channel = -1;
    }

    // If we have nothing left, we're don
    if ( !links ) {
        return;
    }

    // Otherwise, we're still processing potentially
    ChannelLink * current = links;

    while ( current->next ) {
        if ( current->next->channel == channel ) {
            ChannelLink *temp = current->next;
            current->next = temp->next;
            temp->next->next = nullptr; // Deny access to the callback
            temp->owner->finished(channel);
            temp->next->channel = -1;
        } else {
            current = current->next;
        }
    }
}

Sfx::~Sfx() {
    if ( m_chunk ) {
        Mix_FreeChunk(m_chunk);
    }
}

ChannelLink * Sfx::links = nullptr;


