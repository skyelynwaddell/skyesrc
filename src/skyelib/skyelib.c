#define RAYGUI_IMPLEMENTATION
#include "skyelib.h"
#include "global.h"

// Server Properties
char SERVER_IP[64] = "127.0.0.1";
int SERVER_PORT = 25523;
int SERVER_HEADLESS = false;
int server_online = false;
// ----------------

RenderTexture2D target;
Vector2 mouse;
Vector2 virtualMouse;

int GAME_SCREEN_WIDTH = 1280;
int GAME_SCREEN_HEIGHT = 720;
int VIEWMODEL_POSITION_MODE = false;
GUI_STATE gui_state = GUI_STATE_DEFAULT;

int FPS           = 144;
int SCREEN_WIDTH  = 1920;
int SCREEN_HEIGHT = 1080;
int VSYNC         = false;

int FONT_SIZE_DEFAULT = 11;
int fontsize = 11;
float initial_gui_scale = 1.8;
float gui_scale = 1.8;

int MAX_BULLET_HOLES = 24;
Decal bullet_holes[24];
int decal_count = 0;

int should_camera_tilt = true;
int should_weapon_bob = true;
int should_weapon_sway = true;
bool show_fps = true;

// floating window properties
Vector2 window_position = { 10, 10 };
Vector2 window_size = { 450 , 450 };
bool window_minimized = false;
bool window_moving = false;
bool window_resizing = false;
Vector2 window_scroll;
float window_scale = 1;
int raygui_windowbox_statusbar_height = 24;
int raygui_window_closebutton_size = 24;
#define RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT 30
#define RAYGUI_WINDOW_CLOSEBUTTON_SIZE 30

void font_set_size(int size){ GuiSetStyle(DEFAULT, TEXT_SIZE, size); }
int GetGameScreenWidth(){ return GAME_SCREEN_WIDTH; }
int GetGameScreenHeight(){ return GAME_SCREEN_HEIGHT; }


/*
game_is_running
Returns TRUE or FALSE depending
if the window is open, and the main
game loop is running 
*/
int game_is_running()
{
    return global_quit_game == false && !WindowShouldClose();
}


/*
window_check_for_resize
Checks the window if it was resized and then
updates the mouse coords, and gamescreen texture size
*/
void window_check_for_resize()
{
    if (IsWindowResized())
    {
        target = LoadRenderTexture(GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT);
        update_virtual_mouse(); 
    }
}


/*
is_valid_number
Returns true or false if the string is a number
*/
bool is_valid_number(const char *str) {
    if (str[0] == '\0') return false; // empty string is invalid
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit((unsigned char)str[i])) return false;
    }
    return true;
}


/*
update_virtual_mouse
Must be called every game TICK to translate the mouse coords
to the scaled resolution GUI coords
*/
void update_virtual_mouse()
{
    // Update virtual mouse (clamped mouse value behind game screen)
    mouse = GetMousePosition();
    virtualMouse = (Vector2){ 0 };
    window_scale = Min((float)GetScreenWidth()/GAME_SCREEN_WIDTH, (float)GetScreenHeight()/GAME_SCREEN_HEIGHT);

    virtualMouse.x = (mouse.x - (GetScreenWidth() - (GAME_SCREEN_WIDTH*window_scale))*0.5f)/window_scale;
    virtualMouse.y = (mouse.y - (GetScreenHeight() - (GAME_SCREEN_HEIGHT*window_scale))*0.5f)/window_scale;
    virtualMouse = Vector2Clamp(virtualMouse, (Vector2){ 0, 0 }, (Vector2){ (float)GAME_SCREEN_WIDTH, (float)GAME_SCREEN_HEIGHT });

    // Apply the same transformation as the virtual mouse to the real mouse (i.e. to work with raygui)
    SetMouseOffset(-(GetScreenWidth() - (GAME_SCREEN_WIDTH*window_scale))*0.5f, -(GetScreenHeight() - (GAME_SCREEN_HEIGHT*window_scale))*0.5f);
    SetMouseScale(1/window_scale, 1/window_scale);

    window_position = (Vector2){ GetGameScreenWidth()/2-(window_size.x/2), GetGameScreenHeight()/2-(window_size.y/2) };
}


/*
set_screen_size
Sets the game screen size resolution to the enum of SCREEN_SIZE
*/
void set_screen_size(SCREEN_SIZE sz)
{
    switch(sz)
    {
        case SCREEN_SIZE_640:
            GAME_SCREEN_WIDTH = 640; 
            GAME_SCREEN_HEIGHT = 480;
            gui_scale = 1.1;
            window_size = (Vector2){ 450 , 450 };
            target = LoadRenderTexture(GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT);
            update_virtual_mouse(); 
            font_set_size((int)(fontsize * gui_scale));
        break;

        case SCREEN_SIZE_1280:
            GAME_SCREEN_WIDTH = 1280; 
            GAME_SCREEN_HEIGHT = 720;
            gui_scale = 1.8;
            window_size = (Vector2){ 600 , 600 };
            target = LoadRenderTexture(GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT);
            update_virtual_mouse(); 
            font_set_size((int)(fontsize * gui_scale));
        break;

        case SCREEN_SIZE_1920:
            GAME_SCREEN_WIDTH = 1920; 
            GAME_SCREEN_HEIGHT = 1080;
            gui_scale = 2.8;
            window_size = (Vector2){ 850 , 850 };
            target = LoadRenderTexture(GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT);
            update_virtual_mouse(); 
            font_set_size((int)(11 * gui_scale));
        break;
    }
}


/*
trim
Trims whitespace in a char[]
*/
char *trim(char *str) {
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;

    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    *(end + 1) = 0;
    return str;
}


/*
SCREEN_CENTER
Returns the X and Y coords of the current Screen Center
*/
Vector2 SCREEN_CENTER() 
{ 
    return (Vector2){ 
        GetScreenWidth() / 2.0f, 
        GetScreenHeight() / 2.0f
    };
}


/*
CENTER_RAY
Returns the Ray that points towards the center of the screen
*/
Ray CENTER_RAY(Camera cam) { return GetScreenToWorldRay(SCREEN_CENTER(), cam); }


/*
BeginModeViewModel()
Starts the 3D mode for the view model rendering.
*/
void BeginModeViewModel()
{
    rlDrawRenderBatchActive();
    glClear(GL_DEPTH_BUFFER_BIT);
    BeginMode3D(viewmodel.camera);
}


/*
EndModeViewModel()
Ends the 3D mode for the view model rendering.
*/
void EndModeViewModel()
{
    EndMode3D();
}


/*
to_delta
Returns value multiplied by Delta Time
*/
float to_delta(float value)
{
    return value * GetFrameTime();
}


/*
frustum_get_from_camera
Gets what the camera can SEE currently on the screen aka the frustum
*/
Frustum frustum_get_from_camera(Camera3D cam)
{
    Matrix view = MatrixLookAt(cam.position, cam.target, cam.up);

    #ifdef DEBUG
        float _fov = 0.9f; // reduce FOV by 10% in DEBUG to show Occlussion Culling is working
    #else
        float _fov = 1.0f;
    #endif
    
    float fov = cam.fovy * _fov;
    Matrix proj = MatrixPerspective(fov * DEG2RAD, (float)GAME_SCREEN_WIDTH / GAME_SCREEN_HEIGHT, 0.01f, 1000.0f);

    Matrix vp = MatrixMultiply(view, proj);

    Frustum frustum;

    frustum.left   = (Vector4){ vp.m3 + vp.m0,  vp.m7 + vp.m4,  vp.m11 + vp.m8,  vp.m15 + vp.m12 };
    frustum.right  = (Vector4){ vp.m3 - vp.m0,  vp.m7 - vp.m4,  vp.m11 - vp.m8,  vp.m15 - vp.m12 };
    frustum.bottom = (Vector4){ vp.m3 + vp.m1,  vp.m7 + vp.m5,  vp.m11 + vp.m9,  vp.m15 + vp.m13 };
    frustum.top    = (Vector4){ vp.m3 - vp.m1,  vp.m7 - vp.m5,  vp.m11 - vp.m9,  vp.m15 - vp.m13 };
    frustum.near   = (Vector4){ vp.m3 + vp.m2,  vp.m7 + vp.m6,  vp.m11 + vp.m10, vp.m15 + vp.m14 };
    frustum.far    = (Vector4){ vp.m3 - vp.m2,  vp.m7 - vp.m6,  vp.m11 - vp.m10, vp.m15 - vp.m14 };

    frustum.left   = Vector4Normalize(frustum.left);
    frustum.right  = Vector4Normalize(frustum.right);
    frustum.bottom = Vector4Normalize(frustum.bottom);
    frustum.top    = Vector4Normalize(frustum.top);
    frustum.near   = Vector4Normalize(frustum.near);
    frustum.far    = Vector4Normalize(frustum.far);

    return frustum;
}


/*
frustum_check_boundingbox
Returns TRUE if a BoundingBox is inside the Frustum
*/
int frustum_check_boundingbox(BoundingBox box, Frustum f)
{
    Vector3 corners[8] = {
        {box.min.x, box.min.y, box.min.z},
        {box.max.x, box.min.y, box.min.z},
        {box.min.x, box.max.y, box.min.z},
        {box.max.x, box.max.y, box.min.z},
        {box.min.x, box.min.y, box.max.z},
        {box.max.x, box.min.y, box.max.z},
        {box.min.x, box.max.y, box.max.z},
        {box.max.x, box.max.y, box.max.z},
    };

    Vector4 planes[6] = { f.left, f.right, f.top, f.bottom, f.near, f.far };

    for (int i = 0; i < 6; i++)
    {
        int outside = 0;
        for (int j = 0; j < 8; j++)
        {
            Vector3 p = corners[j];
            if ((planes[i].x * p.x + planes[i].y * p.y + planes[i].z * p.z + planes[i].w) < 0.0f)
                outside++;
        }

        // All 8 corners are outside 1 plane = not visible
        if (outside == 8)
            return false;
    }

    return true; // At least partially inside
}


/*
frustum_check_sphere
Returns TRUE if a sphere is inside or intersecting the frustum
*/
int frustum_check_sphere(Vector3 center, float radius, Frustum f)
{
    Vector4 planes[6] = { f.left, f.right, f.top, f.bottom, f.near, f.far };

    for (int i = 0; i < 6; i++)
    {
        float distance = planes[i].x * center.x + 
                         planes[i].y * center.y + 
                         planes[i].z * center.z + 
                         planes[i].w;

        // Completely outside this plane
        if (distance < -radius)
            return false;
    }

    return true; // Inside or intersecting
}


/*
string_equals
string[char* string] - The first string to be compared
string[char* string_to_compare_to] - the second string that is compared to the first string
-- compares two strings to see if they match
*/
int string_equals(char* string, char* string_to_compare_to)
{
    if (strcmp(string, string_to_compare_to) == 0) return 1;
    return 0;
}


/*
convert_trenchbroom_to_raylib_axis
-- raylib and trenchbroom dont use the same xyz axis, so we have to convert this here
-- so our stuff isnt sideways :P
*/
Vector3 trench_to_raylib_origin(Vector3 v)
{
    float s = 0.1; // scale
    Vector3 newpos = (Vector3) { v.x*s, v.y*s, -v.z*s };
    return newpos;
}


/*
rotate_vector_around_axis
-- takes in a vector3, a vector3 axis, and the angle degrees
-- and rotates the origin vec around the second vector3
*/
Vector3 rotate_vector_around_axis(Vector3 vec, Vector3 axis, float angle)
{
    axis = Vector3Normalize(axis);
    float cos_theta = cosf(angle);
    float sin_theta = sinf(angle);

    return Vector3Add(
        Vector3Add(
            Vector3Scale(vec, cos_theta),
            Vector3Scale(Vector3CrossProduct(axis, vec), sin_theta)
        ),
        Vector3Scale(axis, Vector3DotProduct(axis, vec) * (1 - cos_theta))
    );
}


/*
Vector3DoubleCrossProduct
// Calculate two vectors cross product
*/
Vector3Double Vector3DoubleCrossProduct(Vector3Double v1, Vector3Double v2)
{
    Vector3Double result = { v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x };
    return result;
}


/*
Vector3DoubleScale
// Multiply vector by scalar
*/
Vector3Double Vector3DoubleScale(Vector3Double v, float scalar)
{
    Vector3Double result = { v.x*scalar, v.y*scalar, v.z*scalar };
    return result;
}


// Calculate two vectors dot product
double Vector3DoubleDotProduct(Vector3Double v1, Vector3Double v2)
{
    float result = (v1.x*v2.x + v1.y*v2.y + v1.z*v2.z);
    return result;
}


// Add two vectors
Vector3Double Vector3DoubleAdd(Vector3Double v1, Vector3Double v2)
{
    Vector3Double result = { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
    return result;
}


// Subtract two vectors
Vector3Double Vector3DoubleSubtract(Vector3Double v1, Vector3Double v2)
{
    Vector3Double result = { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
    return result;
}


// Normalize provided vector
Vector3Double Vector3DoubleNormalize(Vector3Double v)
{
    Vector3Double result = v;

    float length = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
    if (length != 0.0f)
    {
        float ilength = 1.0f/length;

        result.x *= ilength;
        result.y *= ilength;
        result.z *= ilength;
    }

    return result;
}


/*
rlGetVersionString
Returns the current OpenGL version as formatted
string :)
*/
const char *rlGetVersionString(void)
{
    switch (rlGetVersion())
    {
        case RL_OPENGL_11: return "OpenGL: v1.1";
        case RL_OPENGL_21: return "OpenGL: v2.1";
        case RL_OPENGL_33: return "OpenGL: v3.3";
        case RL_OPENGL_43: return "OpenGL: v4.3";
        case RL_OPENGL_ES_20: return "OpenGL ES: v2.0";
        case RL_OPENGL_ES_30: return "OpenGL ES: v3.0";
        default: return "OpenGL";
    }
}


/*
raycast_check_triangle
Checks if the Raycast is intersecting with a triangle
for the Brush raycast detection
*/
int raycast_check_triangle(Ray ray, Vector3 v0, Vector3 v1, Vector3 v2, float *outDistance, Vector3 *outPoint) {
    Vector3 edge1 = Vector3Subtract(v1, v0);
    Vector3 edge2 = Vector3Subtract(v2, v0);
    Vector3 h = Vector3CrossProduct(ray.direction, edge2);
    float a = Vector3DotProduct(edge1, h);

    if (a > -EPSILON && a < EPSILON) return false;  // Ray is parallel

    float f = 1.0f / a;
    Vector3 s = Vector3Subtract(ray.position, v0);
    float u = f * Vector3DotProduct(s, h);
    if (u < 0.0f || u > 1.0f) return false;

    Vector3 q = Vector3CrossProduct(s, edge1);
    float v = f * Vector3DotProduct(ray.direction, q);
    if (v < 0.0f || u + v > 1.0f) return false;

    float t = f * Vector3DotProduct(edge2, q);
    if (t > EPSILON) {
        if (outDistance) *outDistance = t;
        if (outPoint) *outPoint = Vector3Add(ray.position, Vector3Scale(ray.direction, t));
        return true;
    }

    return false;
}


/*
raycast_check_poly
Checks if the raycast is intersecting with
a CollisionPolygon
*/
int raycast_check_poly(CollisionPolygon poly)
{
    bool hit_any = false;
    float closest_distance = global_raycast.blocked_distance;
    Vector3 closest_point = { 0 };
    Vector3 closest_normal = { 0 };

    for (int i = 0; i < poly.count; i++)
    {
        Triangle tri = poly.triangles[i];
        float hit_distance;
        Vector3 hit_point;

        if (raycast_check_triangle(global_raycast.ray, tri.a, tri.b, tri.c, &hit_distance, &hit_point))
        {
            if (hit_distance < closest_distance) {
                // Compute the normal here
                Vector3 edge1 = Vector3Subtract(tri.b, tri.a);
                Vector3 edge2 = Vector3Subtract(tri.c, tri.a);
                Vector3 normal = Vector3Normalize(Vector3CrossProduct(edge1, edge2));

                // Store closest hit
                closest_distance = hit_distance;
                closest_point = hit_point;
                closest_normal = normal;
                hit_any = true;
            }
        }
    }

    if (hit_any) {
        global_raycast.hit_point = closest_point;
        global_raycast.hit_normal = closest_normal;
        global_raycast.has_hit = true;
        global_raycast.blocked_distance = closest_distance;

        return true;
    }

    return false;
}

static int next_bullethole = 0;
void decal_create_bullethole(Vector3 position, Vector3 normal, float size)
{
    float offset = 0.04f; // TODO : Figure out A GOOD WAY how to make decals never overlap or clip eachother
    Vector3 adjusted_position = Vector3Add(position, Vector3Scale(normal, offset));

    Texture2D texture = texture_get_cached("decbul3");
    Decal *bh = &bullet_holes[next_bullethole];
    bh->position = adjusted_position;
    bh->normal = Vector3Negate(normal);
    bh->texture = texture;
    bh->size = size;
    bh->active = true;

    next_bullethole = (next_bullethole + 1) % MAX_BULLET_HOLES;
}