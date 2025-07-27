#include "skyelib.h"
#include "global.h"
#include "floatingwindow.h"

static int btnw = 100;
static int btnh = 25;

static int x = 0;
static int y = 0;
static int activeTab = 0;
static int gap = 5;

static void newline(){ y += 11*gui_scale; }
static void newcol(){ GuiLoadStyle(STYLE_SKYE); x+=btnw; }
static void reset_y() { y = raygui_windowbox_statusbar_height + gap; }
static int reset_x() { x = 20; return 20; }

static void button_highlight()
{
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(VIOLET));
    GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, ColorToInt(VIOLET));
}

static char input_playername[64] = "Player";
static bool input_player_editing = false;
static char input_fps[5] ="60";
static bool input_fps_editing = false;

static bool _viewmodel_position_mode = false;
static int _resolution_selected = 0;
static bool _resolution_active = false;
static bool _vsync = false;
static bool _show_fps = false;
static bool _window_fullscreen = false;
static bool _should_camera_tilt = false;
static bool _should_weapon_bob = false;
static bool _should_weapon_sway = false;

/*
switch_tab
Basically add any logic that would Refresh the options page here
i.e. refetch the saved options settings.
*/
static void switch_tab(int new_tab)
{
    /*
    Multiplayer
    */
    // Reset Playername
    strncpy(input_playername, global_player_name, sizeof(input_playername));
    input_playername[sizeof(input_playername) - 1] = '\0';

    // Target FPS
    snprintf(input_fps, sizeof(input_fps), "%d", FPS);
    if (is_valid_number(input_fps)) {
        FPS = atoi(input_fps);
        FPS = Clamp(FPS, 30, 1000);  // example clamp
    }

    /*
    Video
    */
    // View model position mode
    _vsync = VSYNC;
    _show_fps = show_fps;
    _window_fullscreen = IsWindowFullscreen();
    _should_camera_tilt = should_camera_tilt;
    _should_weapon_bob = should_weapon_bob;
    _should_weapon_sway = should_weapon_sway;
    _viewmodel_position_mode = VIEWMODEL_POSITION_MODE;

    activeTab = new_tab;
}


/*
Call this at the start to fetch the floating window
option properties
*/
void options_window_init()
{
    switch_tab(0);
}


/*
draw_options_menu
The gui options menu, including all settings for the game
*/
void draw_options_menu(Vector2 position, Vector2 scroll) {

    btnw = 60 * gui_scale;
    btnh = 25 * gui_scale;
    y = raygui_windowbox_statusbar_height + gap;
    x = 5;

    if (activeTab == 0) button_highlight();
    if (GuiButton((Rectangle) { position.x + x + scroll.x, position.y + y + scroll.y, btnw, btnh }, "Multiplayer")) switch_tab(0);
    newcol();

    if (activeTab == 1) button_highlight();
    if (GuiButton((Rectangle) { position.x + x + scroll.x, position.y + y + scroll.y, btnw, btnh }, "Keyboard")) switch_tab(1);
    newcol();

    if (activeTab == 2) button_highlight();
    if (GuiButton((Rectangle) { position.x + x + scroll.x, position.y + y + scroll.y, btnw, btnh }, "Mouse")) switch_tab(2);
    newcol();

    if (activeTab == 3) button_highlight();
    if (GuiButton((Rectangle) { position.x + x + scroll.x, position.y + y + scroll.y, btnw, btnh }, "Audio")) switch_tab(3);
    newcol();

    if (activeTab == 4) button_highlight();
    if (GuiButton((Rectangle) { position.x + x + scroll.x, position.y + y + scroll.y, btnw, btnh }, "Video")) switch_tab(4);
    newcol();

    reset_x();
    reset_y();

    newline();
    newline();
    newline();

    switch(activeTab)
    {
        case 0: // MULTIPLAYER

            DrawText("Player Name:",position.x + x + scroll.x, position.y + y + scroll.y, fontsize*gui_scale, WHITE);
            newline();

            if (GuiTextBox((Rectangle){ position.x + x + scroll.x, position.y + y + scroll.y, 100*gui_scale, 20*gui_scale }, input_playername, 64, input_player_editing)) {
                input_player_editing = !input_player_editing;  // toggle on click
            }
        break;

        case 4: // VIDEO    
            
            DrawText("Edit Viewmodel Mode:",position.x + x + scroll.x, position.y + y + scroll.y, fontsize*gui_scale, WHITE);
            newcol();
            newcol();

            if (GuiButton((Rectangle){ position.x + x + scroll.x, position.y + y + scroll.y, 15*gui_scale, 15*gui_scale }, _viewmodel_position_mode ? "#112#" : "" )) _viewmodel_position_mode = !_viewmodel_position_mode;

            newcol();

            DrawText("Resolution:",position.x + x + scroll.x, position.y + y + scroll.y, fontsize*gui_scale, WHITE);
            newline();

            if (GuiDropdownBox((Rectangle){ position.x + x + scroll.x, position.y + y + scroll.y, 70*gui_scale, 14*gui_scale },
                                            "640x480;1280x720;1920x1080",
                                            &_resolution_selected,
                                            _resolution_active)){
                                                _resolution_active = !_resolution_active;
                                            };
                                            

            /**/
            reset_x();
            newline();

            DrawText("Fullscreen:",position.x + x + scroll.x, position.y + y + scroll.y, fontsize*gui_scale, WHITE);
            newcol();
            newcol();

            if (GuiButton((Rectangle){ position.x + x + scroll.x, position.y + y + scroll.y, 15*gui_scale, 15*gui_scale }, _window_fullscreen ? "#112#" : "" )) {
                _window_fullscreen = !_window_fullscreen;
            }

            reset_x();
            newline();
            newline();
            

            DrawText("V-Sync:",position.x + x + scroll.x, position.y + y + scroll.y, fontsize*gui_scale, WHITE);
            newcol();
            newcol();

            if (GuiButton((Rectangle){ position.x + x + scroll.x, position.y + y + scroll.y, 15*gui_scale, 15*gui_scale }, _vsync ? "#112#" : "" )) 
            {
                _vsync = !_vsync;
            }


            /**/
            reset_x();
            newline();
            newline();

            DrawText("Display FPS:",position.x + x + scroll.x, position.y + y + scroll.y, fontsize*gui_scale, WHITE);
            newcol();
            newcol();

            if (GuiButton((Rectangle){ position.x + x + scroll.x, position.y + y + scroll.y, 15*gui_scale, 15*gui_scale }, _show_fps ? "#112#" : "" )) _show_fps = !_show_fps;

            /**/
            reset_x();
            newline();
            newline();

            DrawText("Target FPS:",position.x + x + scroll.x, position.y + y + scroll.y, fontsize*gui_scale, WHITE);
            newcol();
            newcol();


            if (GuiTextBox((Rectangle){ position.x + x + scroll.x, position.y + y + scroll.y, 50*gui_scale, 20*gui_scale }, input_fps, 5, input_fps_editing)) {
                input_fps_editing = !input_fps_editing;  // toggle on click
            }

            reset_x();
            newline();

            /**/
            reset_x();
            newline();
            newline();

            DrawText("Camera Tilt:",position.x + x + scroll.x, position.y + y + scroll.y, fontsize*gui_scale, WHITE);
            newcol();
            newcol();

            if (GuiButton((Rectangle){ position.x + x + scroll.x, position.y + y + scroll.y, 15*gui_scale, 15*gui_scale }, _should_camera_tilt ? "#112#" : "" )) _should_camera_tilt = !_should_camera_tilt;

            reset_x();
            newline();

            /**/
            reset_x();
            newline();

            DrawText("Weapon Bob:",position.x + x + scroll.x, position.y + y + scroll.y, fontsize*gui_scale, WHITE);
            newcol();
            newcol();

            if (GuiButton((Rectangle){ position.x + x + scroll.x, position.y + y + scroll.y, 15*gui_scale, 15*gui_scale }, _should_weapon_bob ? "#112#" : "" )) _should_weapon_bob = !_should_weapon_bob;

            reset_x();
            newline();

            /**/
            reset_x();
            newline();

            DrawText("Weapon Sway:",position.x + x + scroll.x, position.y + y + scroll.y, fontsize*gui_scale, WHITE);
            newcol();
            newcol();

            if (GuiButton((Rectangle){ position.x + x + scroll.x, position.y + y + scroll.y, 15*gui_scale, 15*gui_scale }, _should_weapon_sway ? "#112#" : "" )) _should_weapon_sway = !_should_weapon_sway;

            reset_x();
            newline();




            newline();
            newline();
            if (GuiButton((Rectangle){ position.x + x + scroll.x, position.y + y + scroll.y, 60*gui_scale, 20*gui_scale }, "#9#Cancel" )) {
                gui_state = GUI_STATE_DEFAULT;
            }

            x += 60 * gui_scale + 2;

            /*
            Save Current Video Settings
            */
            if (GuiButton((Rectangle){ position.x + x + scroll.x, position.y + y + scroll.y, 60*gui_scale, 20*gui_scale }, "#2#Apply")) {

                // Viewmodel
                VIEWMODEL_POSITION_MODE = _viewmodel_position_mode;

                // Resolution
                switch(_resolution_selected){
                    case 0: set_screen_size(SCREEN_SIZE_640); break;
                    case 1: set_screen_size(SCREEN_SIZE_1280); break;
                    case 2: set_screen_size(SCREEN_SIZE_1920); break;
                }

                // Target FPS
                if (is_valid_number(input_fps))
                {
                    FPS = atoi(input_fps);
                    FPS = Clamp(FPS, 30, 1000);
                    snprintf(input_fps, 5, "%d", FPS);  // Ensure it fits in 4 digits + null
                    SetTargetFPS(FPS);
                }

                // Fullscreen
                if ((_window_fullscreen && !IsWindowFullscreen()) || (!_window_fullscreen && IsWindowFullscreen()) )
                {
                    SetWindowSize(GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT);
                    ToggleFullscreen();
                }
                
                VSYNC = _vsync;
                show_fps = _show_fps;
                should_camera_tilt = _should_camera_tilt;
                should_weapon_bob = _should_weapon_bob;
                should_weapon_sway = _should_weapon_sway;
            }

        break;
        default: break;
    }
}


// its impossible to keep the gui code clean no matter what you do like i really fucking tried this time too
// i hate gui