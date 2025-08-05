#include "weapons.h"
#include "skyelib.h"
#include "anims.h"

WEAPON_TYPE current_weapon = WEAPON_TYPE_PISTOL;
WEAPON_TYPE player_weapons[255];
int player_weapon_count = 0;

Weapon weapons[64];
int weapon_count = 0;

// ammo pools
int ammo_pool_9mm[255];
Texture viewmodel_pistol_textures[6];

// typedef struct Weapon {
//     AMMO_POOL_TYPE ammo_pool_type;
//     int clip_size;
//     int mag_size;
//     float shoot_cooldown;

//     const char *model_filepath;
//     float model_scale;
//     int anim_count;
//     Texture model_textures[24];
//     int anim_idle;      // index of the idle animation
//     int anim_reload;
//     int anim_shoot;

// } Weapon;

void weapons_init()
{
    // --- Arms ---
    weapons[WEAPON_TYPE_NONE] = (Weapon){
        AMMO_POOL_TYPE_NONE,                  // ammo_pool_type
        0,                                    // clip_size
        0,                                    // mag_size
        0,                                    // animation duration
        0,                                    // shoot_cooldown
        "",                                   // model_filepath
        0,                                    // model_scale
        0,                                    // anim_count
        {0},                                  // model_textures (set below)
        0,                                    // anim_idle
        0,                                    // anim_reload
        0,                                    // anim_shoot
        {0},                                  // position
        0,                                    // rotation
        false                                 // automatic fire
    };

    // --- 9mm Pistol ---
    weapons[WEAPON_TYPE_PISTOL] = (Weapon){
        AMMO_POOL_TYPE_9MM,                   // ammo_pool_type
        6,                                    // clip_size
        6,                                    // mag_size
        0.25,                                 // animation duration
        0.15,                                 // shoot_cooldown
        "arms_newpistol/arms_newpistol2.glb",  // model_filepath
        1.5,                                  // model_scale
        6,                                    // anim_count
        {0},                                  // model_textures (set below)
        0,                                    // anim_idle
        1,                                    // anim_reload
        2,                                    // anim_shoot
        (Vector3){0, -3.0, -2.5},             // position
        -45,                                  // rotation
        false                                 // automatic fire
    };
    // --- 9mm Pistol Textures ---
    // TODO : Validate the existence of the array positions 
    weapons[WEAPON_TYPE_PISTOL].model_textures[0] = texture_get_cached("skin");
    weapons[WEAPON_TYPE_PISTOL].model_textures[1] = texture_get_cached("skin");
    weapons[WEAPON_TYPE_PISTOL].model_textures[2] = texture_get_cached("gloves");
    weapons[WEAPON_TYPE_PISTOL].model_textures[3] = texture_get_cached("gloves");
    weapons[WEAPON_TYPE_PISTOL].model_textures[4] = texture_get_cached("newpistol");
    weapons[WEAPON_TYPE_PISTOL].model_textures[5] = texture_get_cached("newpistol");
}

