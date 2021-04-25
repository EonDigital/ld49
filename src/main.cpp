#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <stdio.h>
#include "logging.h"
#include "strings.h"
#include "images.h"
#include "sprite.h"
#include "sfx.h"
#include "spriteatlas.h"

enum {
    DEFAULT_HEIGHT = 768,
    DEFAULT_WIDTH  = 1024,

    MAX_EVENT_COUNT = 10000, ///< Something obscenely large.  No spinlocks.

    GRAVITY = 160, /// 16 pixels ~= 1 meter, 10 meters per second, down is positive.
    FRAMERATE = 60,
    FR_GRAV = GRAVITY / FRAMERATE,
};

struct SystemState_s;

typedef void (*action_fp)( struct SystemState_s &s, int val );
static void action_noop( struct SystemState_s &s, int val ) {
    (void)s;
    (void)val;
}

typedef struct {
    SDL_Keysym key;
    action_fp action_press = action_noop;
    action_fp action_release = action_noop;
    int val = 0;
} KeyMapping;

enum {
    ACT_QUIT,
    ACT_UP,
    ACT_DOWN,
    ACT_LEFT,
    ACT_RIGHT,
    ACT_JUMP,

    ACT_count, // The number of actions
};

typedef struct {
    KeyMapping keys[ACT_count];
} KeyMap;

enum CharMode : Uint32 {
    CM_ONE_SHOT,
    CM_LOOP,
    CM_PING_PONG_UP,
    CM_PING_PONG_DOWN,
};

typedef struct {
    Uint8 base;
    Uint8 step;

    Uint32 count : 10; ///< The
    Uint32 idx : 10; ///< The current frame index
    Uint32 delay : 5; ///< The amount to slow the animation
    Uint32 ss : 5; ///< The number of steps
    CharMode mode : 2; ///< The current frame mode
} Char;

typedef struct SystemState_s {
    int width;
    int height;
    Uint32 w_flags;
    Uint32 r_flags;
    SDL_Window * win;
    SDL_Renderer * r;
    bool running;
    SpriteAtlas * atlas;
    Sprite * sprites;
    KeyMap km;
    int c_x;
    int c_y;
    int c_vx;
    int c_vy;
    int c_ax;
    int c_ay;
    Char c;
    Sfx jump_sfx;
} SystemState;

// Actions

static void action_quit( struct SystemState_s &s, int val ) {
    (void) val;
    s.running = false;
}

static void action_start_down( SystemState &s, int val ) {
    s.c_vy += val;
}

static void action_stop_down( SystemState &s, int val ) {
    s.c_vy -= val;
}

static void action_start_right( SystemState &s, int val ) {
    s.c_vx += val;
}

static void action_stop_right( SystemState &s, int val ) {
    s.c_vx -= val;
}

static void action_start_jump( SystemState &s, int val ) {
    s.c_vy = val;
    s.c_ay = FR_GRAV;
    s.jump_sfx.play();
}

static void action_stop_jump( SystemState &s, int val ) {
    s.c_vy = 0;
    s.c_ay = 0;
}

static void action_land( SystemState &s, int val ) {
    s.c_ay = 0;
}

void setup_default_actions( SystemState &s ) {
    KeyMap & km = s.km;
    km.keys[ACT_QUIT].key.sym = SDLK_q;
    km.keys[ACT_QUIT].action_release = action_quit;

    km.keys[ACT_UP].key.sym = SDLK_UP;
    km.keys[ACT_UP].action_press = action_start_down;
    km.keys[ACT_UP].action_release = action_stop_down;
    km.keys[ACT_UP].val = -5;

    km.keys[ACT_DOWN].key.sym = SDLK_DOWN;
    km.keys[ACT_DOWN].action_press = action_start_down;
    km.keys[ACT_DOWN].action_release = action_stop_down;
    km.keys[ACT_DOWN].val = 5;

    km.keys[ACT_RIGHT].key.sym = SDLK_RIGHT;
    km.keys[ACT_RIGHT].action_press = action_start_right;
    km.keys[ACT_RIGHT].action_release = action_stop_right;
    km.keys[ACT_RIGHT].val = 5;

    km.keys[ACT_LEFT].key.sym = SDLK_LEFT;
    km.keys[ACT_LEFT].action_press = action_start_right;
    km.keys[ACT_LEFT].action_release = action_stop_right;
    km.keys[ACT_LEFT].val = -5;

    km.keys[ACT_JUMP].key.sym = SDLK_SPACE;
    km.keys[ACT_JUMP].action_press = action_start_jump;
    km.keys[ACT_JUMP].action_release = action_stop_jump;
    km.keys[ACT_JUMP].val = -25;
}

void setup_default_sprites( SystemState &s ) {
    s.c.base = 0x61;
    s.c.step = 0x1;
    s.c.count = 15;
    s.c.delay = 16;
    s.c.mode = CM_LOOP;
}

SystemState s = {0};

// Convenience

bool operator==( const SDL_Keysym & a, const SDL_Keysym & b ) {
    int mod_mask = KMOD_CTRL | KMOD_ALT | KMOD_SHIFT;
    return ( a.sym == b.sym ) && ( ( a.mod & mod_mask ) == ( b.mod & mod_mask ) );
}

// Basic handling

void process_keypress( SystemState &s, const SDL_Keysym & keysym ) {
    // Trivial search
    for ( size_t i = 0; i < ACT_count; ++i ) {
        if ( s.km.keys[i].key == keysym ) {
            s.km.keys[i].action_press( s, s.km.keys[i].val );
        }
    }
}

void process_keyrelease( SystemState &s, const SDL_Keysym & keysym ) {
    if ( keysym.sym == SDLK_ESCAPE ) {
        s.running = false;
        return;
    }

    // Trivial search
    for ( size_t i = 0; i < ACT_count; ++i ) {
        if ( s.km.keys[i].key == keysym ) {
            s.km.keys[i].action_release( s, s.km.keys[i].val );
        }
    }
}

void load_resources( SystemState &s ) {
    s.atlas = new SpriteAtlas(s.r, "res/sprites.png");
    s.sprites = new UniformSprite( s.r, s.atlas, 16, 16 );

    s.jump_sfx.load("res/jump1.ogg");
}

void free_resources( SystemState &s ) {

    if ( s.sprites ) {
        delete( s.sprites );
        s.sprites = nullptr;
    }

    if ( s.atlas ) {
        delete( s.atlas );
        s.atlas = nullptr;
    }
}

void step_animation( Char & c ) {
    if ( ++c.ss < c.delay ) {
        return;
    }
    c.ss = 0;
    switch ( c.mode ) {
    case CM_LOOP:
        if ( ++c.idx >= c.count ) {
            c.idx = 0;
        } break;

    case CM_ONE_SHOT:
        if ( c.idx + 1 < c.count ) {
            ++c.idx;
        } break;

    case CM_PING_PONG_UP:
        if ( c.idx + 1 < c.count ) {
            ++c.idx;
        } else {
            c.mode = CM_PING_PONG_DOWN;
        } break;

    case CM_PING_PONG_DOWN:
        if ( c.idx - 1 > 0 ) {
            --c.idx;
        } else {
            --c.idx;
            c.mode = CM_PING_PONG_UP;
        }
    }
}

/**
 * Setup anything that should render before checking keyboard input to accelerate reaction time.
 */
void prep( SystemState &s ) {
    step_animation( s.c );
    s.c_x += s.c_vx + s.c_ax / 2;
    s.c_y += s.c_vy + s.c_ay / 2;
    s.c_vx += s.c_ax;
    s.c_vy += s.c_ay;
}

/**
 * Handle all user input
 */
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
            if ( !e.key.repeat ) {
                process_keyrelease(s, e.key.keysym);
            }
            break;

        case SDL_KEYDOWN:
            if ( !e.key.repeat ) {
                process_keypress(s, e.key.keysym);
            }
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
    SDL_RenderClear(s.r);
    // Render the current character sprite.
    Uint32 char_idx = s.c.base + s.c.step * s.c.idx;
    s.sprites->render(char_idx, s.c_x, s.c_y);
    SDL_RenderPresent(s.r);
}

void throttle( SystemState &s, Uint32 ticks ) {
    if ( ticks < 15 ) {
        SDL_Delay(16 - ticks);
    }
}


int main( int argc, char ** argv ) {

    s.height = DEFAULT_HEIGHT;
    s.width = DEFAULT_WIDTH;
    s.w_flags = SDL_WINDOW_RESIZABLE;
    s.r_flags = SDL_RENDERER_ACCELERATED; // SDL_RENDERER_PRESENTVSYNC - consider this
    s.running = true;
    setup_default_actions(s);
    setup_default_sprites(s);

    SDL_SetMainReady();
    if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS | SDL_INIT_AUDIO ) != 0 ) {
        LOG_ERROR( "Failed to get SDL running %s", SDL_GetError() );
        s.running = false;
    }

    s.win = SDL_CreateWindow( str_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, s.width, s.height, s.w_flags );
    if ( s.win == nullptr ) {
        LOG_ERROR( "Failed to create SDL Window %s", SDL_GetError() );
        s.running = false;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    s.r = SDL_CreateRenderer( s.win, -1, s.r_flags );
    if ( s.r == nullptr ) {
        LOG_ERROR( "Failed to create Renderer %s", SDL_GetError() );
        s.running = false;
    }

    int images_failed = init_sdl_images();

    // TODO - move to its own module.
    bool audio_setup = true;
    if ( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048) ) {
        LOG_ERROR( "Sound failed to load %s", Mix_GetError() );
        s.running = false;
        audio_setup = false;
    }

    load_resources( s );

    while ( s.running ) {
        Uint32 start = SDL_GetTicks();
        prep( s );
        process_input( s );
        render( s );
        Uint32 stop = SDL_GetTicks();
        throttle( s, stop - start );
    }

    free_resources( s );

    if ( audio_setup ) {
        Mix_CloseAudio();
        Mix_Quit();
    }

    if ( !images_failed ) {
        cleanup_sdl_images();
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
