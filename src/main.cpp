#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <stdio.h>
#include "logging.h"
#include "strings.h"

enum {
    DEFAULT_HEIGHT = 768,
    DEFAULT_WIDTH  = 1024,

    MAX_EVENT_COUNT = 10000, ///< Something obscenely large.  No spinlocks.
};

struct SystemState_s;

typedef void (*action_fp)( struct SystemState_s &s );
static void action_noop( struct SystemState_s &s ) {
}

typedef struct {
    SDL_Keysym key;
    action_fp action_press = action_noop;
    action_fp action_release = action_noop;
} KeyMapping;

enum {
    ACT_QUIT,
    ACT_UP,
    ACT_DOWN,
    ACT_LEFT,
    ACT_RIGHT,

    ACT_count, // The number of actions
};

typedef struct {
    KeyMapping keys[ACT_count];
} KeyMap;

typedef struct SystemState_s {
    int width;
    int height;
    Uint32 w_flags;
    Uint32 r_flags;
    SDL_Window * win;
    SDL_Renderer * r;
    bool running;
    KeyMap km;
} SystemState;

SystemState s = {0};

bool operator==( const SDL_Keysym & a, const SDL_Keysym & b ) {
    return ( a.sym == b.sym ) && ( a.mod == b.mod );
}

void process_keypress( SystemState &s, const SDL_Keysym & keysym ) {
    if ( keysym.sym == SDLK_q || keysym.sym == SDLK_ESCAPE ) {
            s.running = false;
        return;
    }

    // Trivial search
    for ( size_t i = 0; i < ACT_count; ++i ) {
        if ( s.km.keys[i].key == keysym ) {
            s.km.keys[i].action_press( s );
        }
    }
}

void process_keyrelease( SystemState &s, const SDL_Keysym & keysym ) {
    if ( keysym.sym == SDLK_q || keysym.sym == SDLK_ESCAPE ) {
            s.running = false;
        return;
    }

    // Trivial search
    for ( size_t i = 0; i < ACT_count; ++i ) {
        if ( s.km.keys[i].key == keysym ) {
            s.km.keys[i].action_release( s );
        }
    }
}

void prep( SystemState &s ) {
}

void process_input( SystemState &s ) {
    SDL_Event e;
    for ( size_t i = 0; i < MAX_EVENT_COUNT && SDL_PollEvent( &e ); ++i ) {
        switch ( e.type ) { // See SDL_events.h for reference
        case SDL_QUIT:
        case SDL_APP_TERMINATING:
        case SDL_APP_LOWMEMORY:
            s.running = false;
            return;

        case SDL_KEYUP:
        case SDL_KEYDOWN:
            process_keypress(s, e.key.keysym);
            break;

        // Motion support
        case SDL_MOUSEMOTION:
        case SDL_JOYAXISMOTION:
        case SDL_JOYBALLMOTION:
        case SDL_JOYHATMOTION:
        case SDL_MOUSEWHEEL:
        case SDL_CONTROLLERAXISMOTION:
            break;

        // Non-keyboard button-down support
        case SDL_MOUSEBUTTONDOWN:
        case SDL_JOYBUTTONDOWN:
        case SDL_CONTROLLERBUTTONDOWN:
            break;

        // Non-keyboard button-up support
        case SDL_MOUSEBUTTONUP:
        case SDL_JOYBUTTONUP:
        case SDL_CONTROLLERBUTTONUP:
            break;

        // Consider pausing a game during unexpected removal.
        case SDL_JOYDEVICEADDED:
        case SDL_JOYDEVICEREMOVED:
            break;

        // Suppress chatter from window events for now
        case SDL_RENDER_TARGETS_RESET:
        case SDL_RENDER_DEVICE_RESET:
        case SDL_WINDOWEVENT:
            break;

        // Not sure what to do with these yet
        case SDL_AUDIODEVICEADDED:
        case SDL_AUDIODEVICEREMOVED:
            break;

        // I may want to use this... save for later.
        case SDL_USEREVENT:
            break;

        // In case I want to interact with live text.
        case SDL_TEXTEDITING:
        case SDL_TEXTINPUT:
            break;

        default:
            LOG_INFO("E %4x", e.type);
            break;
        }
    }
}

void render( SystemState &s ) {
}

void throttle( SystemState &s ) {
    // TODO - Instead of rough 60hz, determine how much time we lost processing the loop and balance.
    SDL_Delay(16);
}


int main( int argc, char ** argv ) {

    s.height = DEFAULT_HEIGHT;
    s.width = DEFAULT_WIDTH;
    s.w_flags = SDL_WINDOW_RESIZABLE;
    s.r_flags = SDL_RENDERER_ACCELERATED;
    s.running = true;

    SDL_SetMainReady();
    if ( SDL_Init( SDL_INIT_EVERYTHING ) != 0 ) {
        LOG_ERROR( "Failed to get SDL running %s", SDL_GetError() );
        s.running = false;
    }

    s.win = SDL_CreateWindow( str_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, s.width, s.height, s.w_flags );

    if ( s.win == nullptr ) {
        LOG_ERROR( "Failed to create SDL Window %s", SDL_GetError() );
        s.running = false;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    s.r = SDL_CreateRenderer( s.win, -1, s.r_flags );

    if ( s.r == nullptr ) {
        LOG_ERROR( "Failed to create Renderer %s", SDL_GetError() );
        s.running = false;
    }

    printf("Hello World\n");

    while ( s.running ) {
        prep( s );
        process_input( s );
        render( s );
        throttle( s );
    }

    if ( s.r ) {
        SDL_DestroyRenderer( s.r );
        s.r = nullptr;
    }

    if ( s.win ) {
        SDL_DestroyWindow(s.win);
        s.win = nullptr;
    }
    SDL_Quit();

    return 0;
}
