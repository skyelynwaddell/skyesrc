#include "gameloop.h"
#include "skyelib.h"
#include "global.h"
#include "player.h"
#include "enemy.h"

/*
update
-- this method is called every game tick
-- put your game logic in here
*/
void update()
{
    if (global_paused) return; // If the game is paused, skip the update logic
    raycast_start();

    float time = GetTime();
int loc = GetShaderLocation(sh_water, "time");
SetShaderValue(sh_water, loc, &time, SHADER_UNIFORM_FLOAT);


    #ifdef DEV_MODE
        map_hotreload();
    #endif

    // --- Update ---
    console_update();
    player_update();
    enemy_update_all();
    lights_update();

    global_frustum = frustum_get_from_camera(camera);
    raycast_update(); // call last to update raycast
}