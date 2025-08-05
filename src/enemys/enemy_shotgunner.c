#include "enemy.h"
#include "player.h"

static Vector3 lastRootPosition = {0, 0, 0};  // Store last frame root motion

/*
shotgunner_handle_hurt()
Checks if the enemy is hit, and handles the hurt logic
*/
static void shotgunner_handle_hurt(Enemy *mon)
{
    if (mon->gameobject.is_hit == true)
    {
        mon->health -= 25;

        if (mon->health <= 0)
        {
            int death_anim = ANIM_SHOTGUNNER_DEATH_1; //+ (rand() % 2);
            enemy_change_state(mon, STATE_DEAD, death_anim);

            return;
        }
        int hurt_anim = ANIM_SHOTGUNNER_HURT_1 + (rand() % 2);
        enemy_change_state(mon, STATE_HURT, hurt_anim);
    }
}


/*
shotgunner_update()
Called every game TICK -- Main Update Event
*/
static void shotgunner_update(void *self) 
{
    Enemy *mon = (Enemy *)self;
    GameObject *obj = &mon->gameobject;

    check_collisions(obj, false, COLLISION_MASK_SOLID);

    if (global_paused) return;
    if (enemy_should_update(mon) == false) return;

    sModel *mdl = &mon->model;
    apply_gravity(obj);
    smodel_update_position(obj, mdl, (Vector3){ 0, -4.7, 0 });
    raycast_check(obj);

    // --- State Machine ---
    switch(mon->state)
    {
        case STATE_IDLE:
            smodel_animate(mdl, true);
            shotgunner_handle_hurt(mon);
        break;

        case STATE_HURT:
            smodel_animate(mdl, true);
            shotgunner_handle_hurt(mon);
            
            if (mdl->current_anim_finished == true)
                enemy_change_state(mon, STATE_IDLE, ANIM_SHOTGUNNER_IDLE);
        break;

        case STATE_DEAD:
            smodel_animate(mdl, false);
        break;
        default: break;
    }

    raycast_reset(obj);
}


/*
shotgunner_draw()
This draws a single enemy (used as function pointer)
*/
static void shotgunner_draw(void *self)
{
    Enemy *mon = (Enemy *)self;

    if (mon->gameobject.visible == false || 
        distance_to_player(&mon->gameobject, RENDER_DISTANCE) == false) 
            return;
   
    smodel_draw(&mon->model, NULL, 0);
}


/*
shotgunner_create()
Creates a Shotgunner enemy, and returns a pointer to it
The init event
*/
Enemy *shotgunner_create(Vector3 position) {
    Enemy *e = calloc(1, sizeof(Enemy));

    textures_shotgunner[0] = texture_get_cached("soldier_texture");
    textures_shotgunner[1] = texture_get_cached("soldier_texture");
    textures_shotgunner[2] = texture_get_cached("pumpnew");

    e->type = ENEMY_SHOTGUNNER;
    e->health = 100;
    e->gameobject.is_hit = false;
    e->state = STATE_IDLE;
    e->gameobject.position = position;
    e->update = shotgunner_update;
    e->draw = shotgunner_draw;
    e->model = smodel_create(
            MODEL_SHOTGUNNER,
            textures_shotgunner,
            ANIM_COUNT_SHOTGUNNER, 
            ANIM_SHOTGUNNER_IDLE,
            position,
            (Vector3){0},
            SHOTGUNNER_MODEL_SCALE
        );


    // create collision box
    float h = 9.5;  // units tall
    float w = 4;    // units thick/wide

    collisionbox_set(
        &e->gameobject.collision_box,
        e->gameobject.position,
        (Vector3){ w, h, w },
        1
    );

    return e;
}

