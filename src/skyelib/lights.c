
#define RLIGHTS_IMPLEMENTATION

#include "skyelib.h"

/*
light_create
Creates a light object with the designated properties
Illuminates the world and gameobjects
*/
LightObject light_create(
    Color color, 
    float brightness, 
    Vector3 position, 
    float radius
){
    LightObject l;
    l.gameobject.position = position;
    l.brightness = brightness;
    l.radius = radius;

    // Apply brightness to color (scale and clamp to 255)
    Color scaled = {
        (unsigned char)fminf((float)color.r * brightness, 255.0f),
        (unsigned char)fminf((float)color.g * brightness, 255.0f),
        (unsigned char)fminf((float)color.b * brightness, 255.0f),
        (unsigned char)fminf((float)color.a, 255.0f),
    };

    l.color = scaled;

    l.light = CreateLight(
        LIGHT_POINT, 
        position,
        Vector3Zero(), 
        scaled, 
        sh_light);

    return l;
}


/*
lights_init
Called on room entry once
Will initialize all the lights in a room
*/
void lights_init()
{
    sh_light = LoadShader(TextFormat("shaders/glsl%i/lighting.vs", GLSL_VERSION),
                          TextFormat("shaders/glsl%i/lighting.fs", GLSL_VERSION));
    sh_light.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(sh_light, "viewPos");
    int ambientLoc = GetShaderLocation(sh_light, "ambient");
    
    float str = MAX_DARK; // how dark it can get (0 = BLACK)
    SetShaderValue(sh_light, ambientLoc, (float[4]){ str, str, str, 10.0f }, SHADER_UNIFORM_VEC4);

    radiusLoc = GetShaderLocation(sh_light, "radius");
}


/*
lights_update
Updates all the lights properties
Including brightness, color, radius.
NOTE: Must be called every TICK or else lights will not work
*/
void lights_update()
{
    float cam_pos[3] = { camera.position.x,camera.position.y,camera.position.z };
    SetShaderValue(sh_light, sh_light.locs[SHADER_LOC_VECTOR_VIEW], cam_pos, SHADER_UNIFORM_VEC3);

    int light_count = map.light_count;
    float radii[255] = { 0 };

    for (int i = 0; i < map.light_count; i++)
    {
        if (distance_to_player(&map.lights[i].gameobject, RENDER_DISTANCE) == false) continue;
        
        UpdateLightValues(sh_light, map.lights[i].light);
        radii[i] = map.lights[i].radius;
    }

    SetShaderValueV(
        sh_light, 
        radiusLoc, 
        radii, 
        SHADER_UNIFORM_FLOAT,
        map.light_count);
}