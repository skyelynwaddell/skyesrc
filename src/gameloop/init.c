#include "gameloop.h"
#include "skyelib.h"
#include "global.h"
#include "player.h"
#include "weapons.h"

static void reinit()
{
    global_game_loading = true;
    global_paused = true;

    console_log("Creating camera...");
    camera_init();

    map_parse("test.map");

    player_init();
    viewmodel_init();

    console_line();
    global_paused = false;
    global_game_loading = false;
}

/*
init
-- the start of the game loop ~~~ 
-- Called ONCE when the game starts!!
*/
void init()
{
    printf("\n \n ### SKYESRC ### \n \n");

    global_game_loading = true;
    global_paused = true;

    SetConfigFlags(FLAG_MSAA_4X_HINT); // Multi Sampling Anti Aliasing 4X
    SetWindowMinSize(320, 240);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);


    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, GAME_TITLE);
    DisableCursor(); // Limit cursor to relative movement inside the window
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);  // Texture scale filter to use
    SetTargetFPS(FPS);

        sh_water = LoadShader(TextFormat("shaders/glsl%i/water.vs", GLSL_VERSION),
                        TextFormat("shaders/glsl%i/water.fs", GLSL_VERSION));



    console_init();


    for (int i=0; i<MAX_BULLET_HOLES; i++)
    {
        bullet_holes[i].active = false;
    }

    char raylib_version_string[64];
    snprintf(raylib_version_string, sizeof(raylib_version_string),
         "Raylib: v%d.%d.%d", 
         RAYLIB_VERSION_MAJOR, 
         RAYLIB_VERSION_MINOR, 
         RAYLIB_VERSION_PATCH);

    SDL_version compiled;
    SDL_VERSION(&compiled);  // Get SDL version the program was compiled against
    char sdl_version_str[64];
    snprintf(sdl_version_str, sizeof(sdl_version_str), "SDL: v%d.%d.%d", compiled.major, compiled.minor, compiled.patch);

    int glsl_major = GLSL_VERSION / 100;
    int glsl_minor = (GLSL_VERSION / 10) % 10;
    int glsl_patch = GLSL_VERSION % 10;
    char glsl_version_str[64];
    snprintf(glsl_version_str, sizeof(glsl_version_str), "GLSL: v%d.%d.%d\n", glsl_major, glsl_minor, glsl_patch);

    console_log(raylib_version_string);
    console_log(sdl_version_str);
    console_log(rlGetVersionString());
    console_log(glsl_version_str);

    console_line();
    console_log("Initializing engine...");

    weapons_init();
    options_window_init();

    console_log("Adding lights to level...");
    lights_init();

    reinit();
    GuiLoadStyle(STYLE_SKYE);

    global_raycast.ray = CENTER_RAY(camera);
    global_raycast.blocked_distance = FLT_MAX;
    global_raycast.has_hit = false;

    set_screen_size(SCREEN_SIZE_1920);
    font_set_size((int)(FONT_SIZE_DEFAULT * gui_scale));

    // Render texture initialization, used to hold the rendering result so we can easily resize it
    target = LoadRenderTexture(GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT);
    update_virtual_mouse();

}