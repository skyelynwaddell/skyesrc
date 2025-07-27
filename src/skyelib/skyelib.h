#ifndef SKYELIB_H
#define SKYELIB_H

/*
skyelib
This file should contain any feature or functionality
that could be reused across other Games, and projects.
Basically my ultimate "include 1 header file and have it all" 
file for making 3D games...
*/

// --- Game Settings ---
#define GAME_TITLE "skyesrc"
#define SKYESRC_VERSION_MAJOR 0 
#define SKYESRC_VERSION_MINOR 1
#define SKYESRC_VERSION_PATCH 0

// --- Toggles ---
#define PLATFORM_DESKTOP

#define OS_LINUX
//#define OS_MACOS
//#define OS_WINDOWS
//#define OS_ANDROID
//#define OS_WEB

#define DEV_MODE    // Enable dev mode for additional features
//#define DEBUG     // Enable debug mode for debug messages

#ifdef PLATFORM_DESKTOP
#define GLSL_VERSION 330
#else // PLATFORM_ANDROID, PLATFORM_WEB
    #define GLSL_VERSION 100
#endif

#define MODEL_DIR "gamedata/models/"
#define TEXTURE_DIR "gamedata/textures/"
#define MAP_DIR "gamedata/maps/"

extern char SERVER_IP[64];
extern int SERVER_PORT;
extern int SERVER_HEADLESS; // Run server without GUI

// --- Macros ---
#define Max(a, b) ((a) > (b) ? (a) : (b))
#define Min(a, b) ((a) < (b) ? (a) : (b))

// --- Raygui & Styles/Themes ---
#define STYLE_PATH "styles/"
#define STYLE_AMBER STYLE_PATH    "style_amber.rgs"
#define STYLE_ASHES STYLE_PATH    "style_ashes.rgs"
#define STYLE_BLUISH STYLE_PATH   "style_bluish.rgs"
#define STYLE_CANDY STYLE_PATH    "style_candy.rgs"
#define STYLE_CHERRY STYLE_PATH   "style_cherry.rgs"
#define STYLE_CYBER STYLE_PATH    "style_cyber.rgs"
#define STYLE_DARK STYLE_PATH     "style_dark.rgs"
#define STYLE_ENFETE STYLE_PATH   "style_enfete.rgs"
#define STYLE_GENESIS STYLE_PATH  "style_genesis.rgs"
#define STYLE_JUNGLE STYLE_PATH   "style_jungle.rgs"
#define STYLE_LAVANDA STYLE_PATH  "style_lavanda.rgs"
#define STYLE_RLTECH STYLE_PATH   "style_rltech.rgs"
#define STYLE_SUNNY STYLE_PATH    "style_sunny.rgs"
#define STYLE_TERMINAL STYLE_PATH "style_terminal.rgs"
#define STYLE_SKYE STYLE_PATH     "style_skye.rgs"

// --- Raylib ---
#include "raylib.h"
#include "rlgl.h"

#include <ctype.h>  // for isdigit

bool is_valid_number(const char *str);
extern bool show_fps;

// --- Implemented Globals ---
// You can define/implement any of these globals in your global.c file to be able to have access to them!
// Most will be needed to be be implemented.
extern int FPS;                 // max frame rate per second
extern int SCREEN_WIDTH;        // screen width
extern int SCREEN_HEIGHT;       // screen height
extern int GAME_SCREEN_WIDTH;   // game width
extern int GAME_SCREEN_HEIGHT;  // game height
extern int VSYNC;               // is v-sync enabled
extern int VIEWMODEL_POSITION_MODE;

typedef enum GUI_STATE {
    GUI_STATE_DEFAULT,
    GUI_STATE_OPTIONS,
} GUI_STATE;
extern GUI_STATE gui_state;

extern int FONT_SIZE_DEFAULT;
extern int fontsize;

void options_window_init();
extern RenderTexture2D target;
extern Vector2 mouse;
extern Vector2 virtualMouse;
extern float window_scale;
extern Vector2 window_position;
extern Vector2 window_size;
extern bool window_minimized;
extern bool window_moving;
extern bool window_resizing;
extern Vector2 window_scroll;

typedef enum SCREEN_SIZE {
    SCREEN_SIZE_640,
    SCREEN_SIZE_1280,
    SCREEN_SIZE_1920
} SCREEN_SIZE;
void set_screen_size(SCREEN_SIZE sz);
extern int raygui_windowbox_statusbar_height;
extern int raygui_window_closebutton_size;

#define CAMERA_HEIGHT_DEFAULT (Vector3){0.0f, 3.5f, 0.0f} 
#define CAMERA_HEIGHT_CROUCH  (Vector3){0.0f, -2.0f, 0.0f}
extern float global_cam_yaw;    // left/right
extern float global_cam_pitch;  // up/down
extern Vector3 global_camera_height_current;
extern Vector3 global_camera_height;

extern float weapon_bob;
extern float camera_tilt;
extern float camera_tilt_max; // max tilt in radians (~4.5 degrees)
extern float camera_tilt_speed; // how quickly tilt responds

extern int should_camera_tilt;
extern int should_weapon_bob;
extern int should_weapon_sway;

extern int global_quit_game;
extern int global_game_loading;

// --- Movement ---
extern int BUTTON_MOVE_FORWARD_KEY;
extern int BUTTON_MOVE_BACKWARD_KEY;
extern int BUTTON_MOVE_LEFT_KEY;
extern int BUTTON_MOVE_RIGHT_KEY;

extern int BUTTON_JUMP_KEY;
extern int BUTTON_JUMP_PAD;

extern int BUTTON_CROUCH_KEY;
extern int BUTTON_CROUCH_PAD; 

extern int BUTTON_SPRINT_KEY;
extern int BUTTON_SPRINT_PAD;  

// --- Combat ---
extern int BUTTON_SHOOT_KEY;
extern int BUTTON_SHOOT_PAD;

extern int BUTTON_RELOAD_KEY;
extern int BUTTON_RELOAD_PAD;

extern int BUTTON_SWITCH_WEAPON_KEY;
extern int BUTTON_SWITCH_WEAPON_PAD;

extern int BUTTON_FLASHLIGHT_KEY;
extern int BUTTON_FLASHLIGHT_PAD;

// --- DPAD ---
extern int BUTTON_DPAD_UP_KEY;
extern int BUTTON_DPAD_DOWN_KEY;
extern int BUTTON_DPAD_LEFT_KEY;
extern int BUTTON_DPAD_RIGHT_KEY;

extern int BUTTON_DPAD_UP_PAD;
extern int BUTTON_DPAD_DOWN_PAD;
extern int BUTTON_DPAD_LEFT_PAD;
extern int BUTTON_DPAD_RIGHT_PAD;

// --- UI / Menus ---
extern int BUTTON_PAUSE_KEY;
extern int BUTTON_PAUSE_PAD;

extern int BUTTON_INTERACT_KEY;
extern int BUTTON_INTERACT_PAD;

// --- Brush & Map Geometry ---
#define MAX_VERTICES_PER_FACE   128   // a brushface can have up to 128 vertices
#define MAX_LINE                1024  // max lines in a brush
#define MAX_BRUSHES             10000 // max brushes in a level
#define MAX_POLYGONS            10000 // max polygons per brush
#define MAX_VERTICES_PER_FACE   128   // max vertices per brush face
#define BRUSH_FACE_COUNT        64    // a brush can have up to 64 faces - must be convex
#define MAX_TEXTURES            10000 // maximum texture count a level can store

// --- Level Settings ---
#define MAX_ENTITIES 1000 // Maximum entities can be in a room
#define MAX_ENEMIES 128
#define MAX_LIGHTS   150  // Maximum lightobjects can be in a room
#define MAX_DARK     0.2  // How dark the room can get without lighting (0 = BLACK)

// --- Gamepads ---
#define GAMEPAD_P1 0
#define GAMEPAD_P2 1
#define GAMEPAD_P3 2
#define GAMEPAD_P4 3

// --- Raylib ---
#include "raymath.h"
#include "raygui.h"

// --- Libraries ---
#include <stdio.h>
#include "string.h"
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "float.h"
#include "enet.h"

#ifdef OS_MACOS
    #include <OpenGL/gl3.h>
    #include "SDL2/SDL.h"
#else
    #include "GL/gl.h"
    #include "SDL.h"
#endif

#define TEXTURE_FILTER TEXTURE_FILTER_BILINEAR
void update_virtual_mouse();

int GetGameScreenWidth();
int GetGameScreenHeight();

Vector2 SCREEN_CENTER();
Ray CENTER_RAY(Camera cam);

extern float gui_scale;

// --- Global 3D Camera ---
extern Camera camera;
extern int camera_mode;
extern int camera_move_spd;

// --- Map Collisions ---
typedef enum COLLISION_MASK {
    COLLISION_MASK_ALL,
    COLLISION_MASK_SOLID,
    COLLISION_MASK_ENEMY
} COLLISION_MASK;

typedef struct Triangle {
    Vector3 a,b,c;
} Triangle;

typedef struct Polygon {
    Vector3 vertices[MAX_VERTICES_PER_FACE];
    int vertex_count;
} Polygon;

typedef struct Plane {
    Vector3 normal; // plane normal (a,b,c)
    float distance; // plane distance from origin
} Plane;

typedef struct CollisionPolygon {
    Triangle *triangles;
    int count;
} CollisionPolygon;

typedef struct BrushFace {
    // Defines the position, size, and directions of the infinite plane
    Vector3 pos_1;    // [ x y z ] Origin Point of the plane --- The anchor point / position of the plane
    Vector3 pos_2;    // [ x y z ] with pos_1 defines the first infinite direction of the plane
    Vector3 pos_3;    // [ x y z ] with pos_1 defines the second infinite direction of the plane

    // with all the infinite planes in a brush we can clip all the geometry wherever 
    // any of the infinite planes intersect with eachother to form the convex polygon.
    
    // texture data for a brush face
    char texture[64]; // texture string name (not including filetype)

    Vector4 uv_s; // [ Ux Uy Uz Uoffset ]
    Vector4 uv_t; // [ Vx Vy Vz Voffset ]

    int uv_rotation; // texture rotation degrees
    int u_scale;     // horizontal texture scale
    int v_scale;     // vertical texture scale

} BrushFace;

typedef struct Brush {
    int brush_face_count;
    BrushFace brush_faces[BRUSH_FACE_COUNT];
    Polygon polys[BRUSH_FACE_COUNT];
} Brush;

typedef struct Frustum {
    Vector4 left, right, top, bottom, near, far;
} Frustum;

typedef struct Geometry {
    Model model;
    BoundingBox bounds;
    CollisionPolygon collision;
    Vector3 position;
    int visible;
    float bounding_radius; // for sphere collision checks
} Geometry;

typedef struct CollisionBox {
    Vector3 position;
    Vector3 size;
    int scale;
    BoundingBox bounding_box;
} CollisionBox;

typedef struct Raycast {
    Ray ray;
    bool has_hit;
    float blocked_distance;
} Raycast;
extern Raycast global_raycast;

typedef struct GameObject {
    int id;
    Vector3 position;
    Vector3 velocity;
    CollisionBox collision_box;
    float speed;
    float gravity;
    int visible;
    int is_hit;
} GameObject;

typedef struct Vector3Double {
    double x;
    double y;
    double z;
} Vector3Double;

#include "lights.h" // dont move
typedef struct Entity {
    char classname[64];
    Vector3 origin;

    // light properties
    Color color;
    float brightness;
    float radius;
} Entity;

typedef struct TextureCacheEntry {
    char name[64];
    Texture2D texture;
} TextureCacheEntry;

#define MAX_CONSOLE_LINES 128
#define MAX_LINE_LENGTH 256

extern int global_console_open;
void console_init();
void console_update();
void console_draw();
void console_log(const char *text);
void console_line();
void console_br();

// Texture Manager
extern int texture_cache_count;
extern TextureCacheEntry texture_cache[MAX_TEXTURES];
extern Texture2D default_texture;
Texture2D texture_get_cached(char *texture_name);
Texture2D texture_get_default();
void texture_cache_cleanup();

// sModel
extern int ANIM_SPEED;
typedef struct sModel {
    Model model;
    char model_filepath[255];
    Texture textures[64];
    int anim_count;
    int current_anim;
    int current_anim_finished;
    int anim_speed;
    float current_frame;
    Vector3 position;
    Vector3 rotation;
    float scale;
    LightObject viewmodel_light;
    ModelAnimation *anims;
} sModel;

sModel smodel_create(
    char model_filepath[255], 
    Texture textures[64],
    int anim_count,
    int current_anim,
    Vector3 position,
    Vector3 rotation,
    float scale);
void smodel_animation_change(sModel *model, int new_anim);
void smodel_draw(sModel *model, int excluded_mesh[255], int excluded_count);
void smodel_update_animation(sModel *model);
void smodel_update_position(GameObject *obj, sModel *model, Vector3 offset);
void smodel_animate(sModel *model, int loop);

// GameObject
int place_meeting_solid(GameObject *object, COLLISION_MASK mask);
int CheckCollisionBoxesExt(BoundingBox, CollisionPolygon shape);
int check_AABB_triangle_SAT(BoundingBox box, Triangle tri);
void apply_gravity(GameObject *obj);
void check_collisions(GameObject *obj, int is_player, COLLISION_MASK mask);
void raycast_start();
void raycast_update();
int raycast_check_bb(BoundingBox box);
int raycast_check(GameObject *obj);
void raycast_reset(GameObject *obj);
int distance_to_player(GameObject *obj1, float distance);
int distance_to(GameObject *obj1, GameObject *obj2, float distance);
int distance_to_pos(Vector3 pos1, Vector3 pos2, float distance);

// Camera
void camera_init();
void camera_follow_player(Camera3D *camera, GameObject *target);
Vector3 camera_get_forward(Camera *camera);
Vector3 camera_get_up(Camera *camera);
Vector3 camera_get_right(Camera *camera);
void camera_yaw(Camera *camera, float angle, bool rotateAroundTarget);
void camera_pitch(Camera *camera, float angle, bool lockView, bool rotateAroundTarget, bool rotateUp);
void camera_roll(Camera *camera, float angle);
void camera_update_fovy(Camera3D *camera);

extern float camera_bob_phase;
extern float camera_bob_amp;
extern float camera_bob_speed;
extern float bob_offset;

// Polygon
bool polygon_get_intersection(
    Vector3 n1,
    Vector3 n2,
    Vector3 n3,
    double d1,
    double d2,
    double d3,
    Vector3* out
);
void polygon_generate_from_brush(Brush *brush);
void polygon_sort_vertices(Polygon *poly, Vector3 normal);
void polygon_normalize_uv(Vector2 *uvs, int count);
bool polygon_has_vertex(Polygon *poly, Vector3 v);
Vector2 polygon_project_to_uv_standard(Vector3 point, BrushFace *face);
Vector2 polygon_project_to_uv_valve220(Vector3 point, BrushFace *face);

// Collision Box
void collisionbox_set(CollisionBox *box, Vector3 _position, Vector3 _size, int _scale);
void collisionbox_update_aabb(CollisionBox *box);
void collisionbox_set_position(CollisionBox *box, Vector3 _position);

// Brushface
void brushface_print(BrushFace b, int face_index);
Plane brushface_to_plane(BrushFace face);

// Frustum
Frustum frustum_get_from_camera(Camera3D cam);
int frustum_check_boundingbox(BoundingBox box, Frustum f);
int frustum_check_sphere(Vector3 center, float radius, Frustum f);
extern Frustum global_frustum;

// Vector3Double
Vector3Double Vector3DoubleCrossProduct(Vector3Double v1, Vector3Double v2);
Vector3Double Vector3DoubleScale(Vector3Double v, float scalar);
double Vector3DoubleDotProduct(Vector3Double v1, Vector3Double v2);
Vector3Double Vector3DoubleAdd(Vector3Double v1, Vector3Double v2);
Vector3Double Vector3DoubleSubtract(Vector3Double v1, Vector3Double v2);
Vector3Double Vector3DoubleNormalize(Vector3Double v);

// View Model
typedef struct Viewmodel {
    sModel model;
    Camera camera;
    Vector3 position;
    Vector3 rotation_axis;
    float rotation;
} Viewmodel;
extern Viewmodel viewmodel;
void viewmodel_init();
void viewmodel_draw();

// Utility Functions
Vector3 rotate_vector_around_axis(Vector3 vec, Vector3 axis, float angle);
Vector3 trench_to_raylib_origin(Vector3 v);
int string_equals(char* string, char* string_to_compare_to);
float to_delta(float value);
const char *rlGetVersionString();
void stats_draw();
void font_set_size(int size);
void window_check_for_resize();
int game_is_running();
char *trim(char *str);

#include "map.h" // dont move

#endif // SKYELIB_H