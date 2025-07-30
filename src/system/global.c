#include "global.h"
#include "skyelib.h"

// game/system properties
int global_paused = false; // Game pause state
int global_quit_game = false;
int global_game_loading = true;
int global_console_open = false;

// --- SKYELIB GLOBAL IMPLEMENTATIONS ---
// --------------------------------------------------------------------

// --- Shaders ---
Shader sh_light;
Shader sh_water;
Shader sh_viewmodel;

// --- Shader Properties ---
float radiusLoc; // light radius location in shader

// --- Camera ---
Vector3 global_camera_height_current = {0};
Vector3 global_camera_height = (Vector3){0.0f, 3.5f, 0.0f}; // adjust for player height
float global_cam_yaw   = 0.0f;   // left/right
float global_cam_pitch = 0.0f;   // up/down

// --- Raycast ---
int global_raycast_has_target = false;
Raycast global_raycast = {0};

Viewmodel viewmodel = {0};
Frustum global_frustum = {0};

// --- Movement ---
int BUTTON_MOVE_FORWARD_KEY    = KEY_W;
int BUTTON_MOVE_BACKWARD_KEY   = KEY_S;
int BUTTON_MOVE_LEFT_KEY       = KEY_A;
int BUTTON_MOVE_RIGHT_KEY      = KEY_D;

int BUTTON_JUMP_KEY            = KEY_SPACE;
int BUTTON_JUMP_PAD            = GAMEPAD_BUTTON_RIGHT_FACE_DOWN;  

int BUTTON_CROUCH_KEY          = KEY_LEFT_CONTROL;
int BUTTON_CROUCH_PAD          = GAMEPAD_BUTTON_RIGHT_FACE_RIGHT; 

int BUTTON_SPRINT_KEY          = KEY_LEFT_SHIFT;
int BUTTON_SPRINT_PAD          = GAMEPAD_AXIS_LEFT_TRIGGER;    

// --- Combat ---
int BUTTON_SHOOT_KEY           = MOUSE_BUTTON_LEFT;
int BUTTON_SHOOT_PAD           = GAMEPAD_BUTTON_RIGHT_TRIGGER_2;

int BUTTON_RELOAD_KEY          = KEY_R;
int BUTTON_RELOAD_PAD          = GAMEPAD_BUTTON_RIGHT_FACE_LEFT;

int BUTTON_SWITCH_WEAPON_KEY   = KEY_Q;
int BUTTON_SWITCH_WEAPON_PAD   = GAMEPAD_BUTTON_RIGHT_FACE_UP;

int BUTTON_FLASHLIGHT_KEY      = KEY_F;
int BUTTON_FLASHLIGHT_PAD      = GAMEPAD_AXIS_RIGHT_TRIGGER;

// --- DPAD ---
int BUTTON_DPAD_UP_KEY         = KEY_UP;
int BUTTON_DPAD_DOWN_KEY       = KEY_DOWN;
int BUTTON_DPAD_LEFT_KEY       = KEY_LEFT;
int BUTTON_DPAD_RIGHT_KEY      = KEY_RIGHT;

int BUTTON_DPAD_UP_PAD         = GAMEPAD_BUTTON_LEFT_FACE_UP;
int BUTTON_DPAD_DOWN_PAD       = GAMEPAD_BUTTON_LEFT_FACE_DOWN;
int BUTTON_DPAD_LEFT_PAD       = GAMEPAD_BUTTON_LEFT_FACE_LEFT;
int BUTTON_DPAD_RIGHT_PAD      = GAMEPAD_BUTTON_LEFT_FACE_RIGHT;

// --- UI / Menus ---
int BUTTON_PAUSE_KEY           = KEY_ESCAPE;
int BUTTON_PAUSE_PAD           = GAMEPAD_BUTTON_MIDDLE_RIGHT;

int BUTTON_INTERACT_KEY        = KEY_E;
int BUTTON_INTERACT_PAD        = GAMEPAD_BUTTON_RIGHT_FACE_LEFT;
// --------------------------------------------------------------------

// player globals
const char global_player_name[64] = "Player";
Vector3 global_player_spawn = (Vector3){ 8.0f, 3.0f, 7.0f };
int global_player_onground = false;
int global_player_crouching = false;
int global_player_shooting = false;
int global_player_moving = false;
int global_player_onXwall = false;
int global_player_onZwall = false;
int global_player_crouchboost = false;
int global_player_inwater = false;
float PLAYER_STEP_HEIGHT = 2.0f;

int player_on_wall() { return global_player_onXwall || global_player_onZwall; }

/*
pause_toggle
This function toggles the pause state of the game.
*/
void pause_toggle()
{
    global_paused = !global_paused;
    if (global_paused) {
        EnableCursor();
        global_console_open = false;
    }
    else 
    { 
        if (global_console_open == false)
        {
            DisableCursor();
        }
    }
}