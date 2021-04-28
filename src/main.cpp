#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <stdio.h>
#include "logging.h"
#include "strings.h"
#include "images.h"
#include "sprite.h"
#include "sfx.h"
#include "spriteatlas.h"
#include "sequencer.h"
#include "utilities.h"
#include "stage.h"
#include "physpoint.h"

enum {
    DEFAULT_HEIGHT = 768,
    DEFAULT_WIDTH  = 1024,

    SCREEN_RESIZE_FACTOR = 4,

    MAX_EVENT_COUNT = 10000, ///< Something obscenely large.  No spinlocks.

    GRAVITY = 160, /// 16 pixels ~= 1 meter, 10 meters per second, down is positive.
    FRAMERATE = 60,
    FR_GRAV = GRAVITY / FRAMERATE,
    FG_PARALLAX = 2,
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
    Sequencer * current_animation;
    Sequencer * walk;
    Sequencer * jump;
    Sequencer * stand;
    PhysPoint phys;
} Char;

typedef uint8_t char_flags_t;
enum : char_flags_t {
    FACE_CURRENT = 0,
    FACE_RIGHT = 1 << 0,
    FACE_LEFT  = 1 << 1,
    FACE_UP    = 1 << 2,
    FACE_DOWN  = 1 << 3,
    FACE_HORIZ = FACE_LEFT | FACE_RIGHT,
    FACE_VERT = FACE_UP | FACE_DOWN,
    FACE_MASK  = FACE_RIGHT | FACE_LEFT | FACE_UP | FACE_DOWN,

    FLAGS_ON_GROUND = 1 << 4,
    FLAGS_STANDARD_GRAVITY = 1 << 5,
    FLAGS_GRAV_MASK = FLAGS_ON_GROUND | FLAGS_STANDARD_GRAVITY,

    FLAGS_CAMERA_TRACKING_X = 1 << 6,
    FLAGS_CAMERA_TRACKING_Y = 1 << 7,
    FLAGS_CAMERA_TRACKING_MASK = FLAGS_CAMERA_TRACKING_X | FLAGS_CAMERA_TRACKING_Y,
};

typedef struct SystemState_s {
    int width;
    int height;
    Uint32 w_flags;
    Uint32 r_flags;
    SDL_Window * win;
    SDL_Renderer * r;
    bool running;
    SpriteAtlas * atlas;
    Sprite * world_sprites;
    Sprite * char_sprites;
    Stage * stage;
    Stage * fg;

    KeyMap km;
    SDL_RendererFlip c_f;
    char_flags_t c_flags;
    Char c;
    struct {
       Sequencer * growth_animation;
    } f; // Just a flower for now.
    Sfx jump_sfx;
} SystemState;

// Actions

static void action_quit( struct SystemState_s &s, int val ) {
    (void) val;
    s.running = false;
}

static void action_start_down( SystemState &s, int val ) {
    s.c.phys.v.y += val;
    s.c_flags |= ( ( val > 0 ) ? FACE_DOWN : ( ( val < 0 ) ? FACE_UP : FACE_CURRENT ) );
}

static void action_stop_down( SystemState &s, int val ) {
    s.c.phys.v.y -= val;
    s.c_flags &= ~( ( val > 0 ) ? FACE_DOWN : ( ( val < 0 ) ? FACE_UP : FACE_CURRENT ) );
}

static void action_start_right( SystemState &s, int val ) {
    s.c.phys.v.x += val;
    s.c_flags |= ( ( val > 0 ) ? FACE_RIGHT : ( ( val < 0 ) ? FACE_LEFT : FACE_CURRENT ) );
}

static void action_stop_right( SystemState &s, int val ) {
    s.c.phys.v.x -= val;
    s.c_flags &= ~( ( val > 0 ) ? FACE_RIGHT : ( ( val < 0 ) ? FACE_LEFT : FACE_CURRENT ) );
}

static void action_start_jump( SystemState &s, int val ) {
    s.c.phys.v.y += val;
    s.c_flags &= ~(FLAGS_ON_GROUND);
    s.jump_sfx.play();
}

static void action_stop_jump( SystemState &s, int val ) {
//    s.c.phys.v.y = 0;
//    s.c_flags |= FLAGS_ON_GROUND;
}

static void action_land( SystemState &s, int val ) {
    s.c.phys.a.y = 0;
}

void setup_default_actions( SystemState &s ) {
    KeyMap & km = s.km;
    km.keys[ACT_QUIT].key.sym = SDLK_q;
    km.keys[ACT_QUIT].action_release = action_quit;

    km.keys[ACT_UP].key.sym = SDLK_UP;
    km.keys[ACT_UP].action_press = action_start_down;
    km.keys[ACT_UP].action_release = action_stop_down;
    km.keys[ACT_UP].val = -1;

    km.keys[ACT_DOWN].key.sym = SDLK_DOWN;
    km.keys[ACT_DOWN].action_press = action_start_down;
    km.keys[ACT_DOWN].action_release = action_stop_down;
    km.keys[ACT_DOWN].val = 1;

    km.keys[ACT_RIGHT].key.sym = SDLK_RIGHT;
    km.keys[ACT_RIGHT].action_press = action_start_right;
    km.keys[ACT_RIGHT].action_release = action_stop_right;
    km.keys[ACT_RIGHT].val = 1;

    km.keys[ACT_LEFT].key.sym = SDLK_LEFT;
    km.keys[ACT_LEFT].action_press = action_start_right;
    km.keys[ACT_LEFT].action_release = action_stop_right;
    km.keys[ACT_LEFT].val = -1;

    km.keys[ACT_JUMP].key.sym = SDLK_SPACE;
    km.keys[ACT_JUMP].action_press = action_start_jump;
    km.keys[ACT_JUMP].action_release = action_stop_jump;
    km.keys[ACT_JUMP].val = -15;
}

void cleanup_default_actions( SystemState &s ) {
    // No-op.  Symmetry to prevent stupid
}

void setup_default_sprites( SystemState &s ) {
    s.f.growth_animation = new SlowSequencer( 16, new LoopSequencer( 0x61, 0x1, 15 ) );
    s.c.walk = new SlowSequencer( 6, new LoopSequencer( 0x8, 0x1, 4 ) );
    s.c.jump = new SlowSequencer( 16, new OneShotSequencer( 0xD, 0x1, 3 ) );
    s.c.stand = new SlowSequencer( 16, new OneShotSequencer( 0x3, 0x1, 5 ) );
    s.c.current_animation = s.c.walk;
    s.c_flags = FLAGS_STANDARD_GRAVITY | FLAGS_CAMERA_TRACKING_X; // | FLAGS_ON_GROUND
}

void cleanup_default_sprites( SystemState &s ) {
    clean_delete( &s.c.stand );
    clean_delete( &s.c.jump );
    clean_delete( &s.c.walk );
    clean_delete( &s.f.growth_animation );
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

const char map[] =
  // 012345678901234567890123456789
    "                              " // 0
    "                              " // 1
    "                              " // 2
    "                              " // 3
    "abcdefghiSSSDDDDD\\        /DDD" // 4
    "abcdefghiSXXXXXXXXXDDDDDDDXXXX" // 5
    "abcdefghiXXXXXXXXXXXXXXXXXXXXX"; // 6

const char fg[] =
  // 012345678901234567890123456789
    "             \xAA\xAB\xAC\xAA\xAB            \xAA\xAB\xAC\xAA\xAB " // 0
    "              \xAD\xAE\xAF              \xAD\xAE\xAF  "; // 1

void load_resources( SystemState &s ) {
    s.atlas = new SpriteAtlas( s.r, "res/sprites.png" );
    s.world_sprites = new UniformSprite( s.atlas, 16, 16 );
    s.char_sprites = new UniformSprite( s.atlas, 16, 32 );

    s.stage = new CompleteStage( s.world_sprites, 30, 7, map, 0, 8 );
    s.fg = new CompleteStage( s.world_sprites, 36, 2, fg, 0, 6 );

    s.jump_sfx.load("res/jump1.ogg");
}

void free_resources( SystemState &s ) {
    clean_delete( &s.fg );
    clean_delete( &s.stage );
    clean_delete( &s.char_sprites );
    clean_delete( &s.world_sprites );
    clean_delete( &s.atlas );
}

/**
 * Setup anything that should render before checking keyboard input to accelerate reaction time.
 */
void prepare_scene( SystemState &s ) {

    // TODO - move into PhysPoint
    switch ( s.c_flags & FLAGS_GRAV_MASK ) {
    case FLAGS_STANDARD_GRAVITY:
        s.c.phys.a.x = 0;
        s.c.phys.a.y = FR_GRAV;
        break;
    case FLAGS_STANDARD_GRAVITY | FLAGS_ON_GROUND:
        s.c.phys.a.x = 0;
        s.c.phys.a.y = 0;
        break;
    default:
        // Do nothing.
        break;
    }

//    int dt = 256 / FRAMERATE;
    int dt = 256;

    s.c.current_animation->get_next();
    iv2_t delta = s.c.phys.get_delta( dt ); // To check for collisions and control the camera

    // Handle collision detection here
    SDL_Rect char_rect = { s.c.phys.p.x, s.c.phys.p.y, s.char_sprites->width(0), s.char_sprites->height(0) };
    bool collision = s.stage->test_collision( char_rect, delta );

    // To prep for the next frame.
    if ( collision ) {
        action_land(s, 0); // XXX - nominally, we reduce velocity relative to our impact direction.  We have artifacts when moving and jumping.
        s.c.phys.v.x = 0;
        s.c.phys.v.y = 0;
        s.c.phys.p += delta;
        s.c_flags |= FLAGS_ON_GROUND;
    } else {
        s.c.phys.advance( dt );
    }

    // Camera Tracking
    if ( s.c_flags & FLAGS_CAMERA_TRACKING_MASK ) {
        int cdelta_x = 0;
        int cdelta_y = 0;

        SDL_Rect & view = s.stage->view();
        int x = s.c.phys.p.x - view.x;
        int y = s.c.phys.p.x - view.y;

        // if x * SF > 3*W/4
        if ( ( s.c.phys.v.x > 0 ) && ( x * SCREEN_RESIZE_FACTOR * 4 > DEFAULT_WIDTH * 3 ) ) {
            cdelta_x = delta.x;
        }
        // if x * SF < W/4
        if ( ( s.c.phys.v.x < 0 ) && ( x * SCREEN_RESIZE_FACTOR * 4 < DEFAULT_WIDTH ) ) {
            cdelta_x = delta.x;
        }

        // if y * SF > 3*H/4
        if ( ( s.c.phys.v.y > 0 ) && ( y * SCREEN_RESIZE_FACTOR * 4 > DEFAULT_HEIGHT * 3 ) ) {
            cdelta_y = delta.y;
        }
        // if y * SF < H/4
        if ( ( s.c.phys.v.y < 0 ) && ( y * SCREEN_RESIZE_FACTOR * 4 < DEFAULT_HEIGHT ) ) {
            cdelta_y = delta.y;
        }

        s.stage->move_camera(
            ( s.c_flags & FLAGS_CAMERA_TRACKING_X ) ? cdelta_x : 0,
            ( s.c_flags & FLAGS_CAMERA_TRACKING_Y ) ? cdelta_y : 0);

        s.fg->move_camera(
            ( s.c_flags & FLAGS_CAMERA_TRACKING_X ) ? cdelta_x * FG_PARALLAX: 0,
            ( s.c_flags & FLAGS_CAMERA_TRACKING_Y ) ? cdelta_y * FG_PARALLAX : 0);
    }
}



void render( SystemState &s ) {
    SDL_RenderClear(s.r);
    // Render the current character sprite.
    Uint32 char_idx = s.c.current_animation->get_current();
    s.stage->render();
    SDL_Rect & view = s.stage->view();

    switch ( s.c_flags & FACE_HORIZ ) {
    case FACE_LEFT: s.c_f = SDL_FLIP_HORIZONTAL; break;
    case FACE_RIGHT: s.c_f = SDL_FLIP_NONE;      break;
    }
    s.char_sprites->render(char_idx, s.c.phys.p.x - view.x, s.c.phys.p.y - view.y, s.c_f );
    // Other sprites
    s.fg->render();
    // Any foreground effects
    SDL_RenderPresent(s.r);
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
    } else if( SDL_RenderSetScale( s.r, SCREEN_RESIZE_FACTOR, SCREEN_RESIZE_FACTOR ) ) {
        LOG_ERROR( "Failed to set the renderer scale %s", SDL_GetError() );
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
    setup_default_actions(s);
    setup_default_sprites(s);

    while ( s.running ) {
        Uint32 start = SDL_GetTicks();
        prepare_scene( s );
        process_input( s );
        render( s );
        Uint32 stop = SDL_GetTicks();
        throttle( s, stop - start );
    }

    cleanup_default_sprites( s );
    cleanup_default_actions( s );
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
