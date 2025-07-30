#include "player.h"
#include "skyelib.h"
#include "weapons.h"

Player player;

/*
player_init
Called when the player object first enters the world
*/
void player_init()
{
    #ifdef DEBUG
        printf("### Player Object Created ###\n\n");
    #endif

    // spawn position
    player.gameobject.position = global_player_spawn;

    // create player collision box
    float h = PLAYER_HEIGHT; // units tall
    float w = PLAYER_WIDTH;  // units thick/wide
    collisionbox_set(
        &player.gameobject.collision_box,
        player.gameobject.position,
        (Vector3){ w, h, w },
        1
    );

    // set player stats
    player.health = PLAYER_HEALTH;
    player.jump_height = PLAYER_JUMPHEIGHT;
    player.gameobject.speed = PLAYER_SPEED;
    player.gameobject.gravity = GRAVITY_DEFAULT;

    global_camera_height_current = global_camera_height;
};

static float anim_cooldown = 0;
static float shoot_cooldown = 0;

/*
player_shoot
Call this to invoke the player shooting
*/
static void player_shoot()
{
    global_player_shooting = true;
    smodel_animation_change(&viewmodel.model,weapons[current_weapon].anim_shoot);
    shoot_cooldown = weapons[current_weapon].shoot_cooldown;
    anim_cooldown = weapons[current_weapon].anim_duration;
    decal_create_bullethole(global_raycast.hit_point, global_raycast.hit_normal, 0.6f);
}

static int reloading = false;

/*
player_handle_shoot
Handles if the player can shoot, and the button presses
*/
static void player_handle_shoot(float delta)
{
    if (reloading) return;

    if (anim_cooldown > 0){
        anim_cooldown -= delta;
        anim_cooldown = Clamp(anim_cooldown,0,weapons[current_weapon].anim_duration);

        if (anim_cooldown <= 0.1)
            viewmodel.model.current_anim = weapons[current_weapon].anim_idle;
    }

    
    if (shoot_cooldown > 0) 
    {
        shoot_cooldown -= delta;
        return;
    }

    if (weapons[current_weapon].automatic)
    {
    if (IsMouseButtonDown(BUTTON_SHOOT_KEY) || 
    IsGamepadButtonDown(GAMEPAD_P1, BUTTON_SHOOT_PAD))
        player_shoot();
    }
    else
    {
    if (IsMouseButtonPressed(BUTTON_SHOOT_KEY) || 
    IsGamepadButtonPressed(GAMEPAD_P1, BUTTON_SHOOT_PAD))
        player_shoot();
    }
}



/*
player_handle_reload
*/
void player_handle_reload()
{
    if (IsKeyPressed(BUTTON_RELOAD_KEY) || IsGamepadButtonPressed(GAMEPAD_P1, BUTTON_RELOAD_PAD))
    {
        if (reloading) return;
        reloading = true;
        smodel_animation_change(&viewmodel.model, weapons[current_weapon].anim_reload);
        printf("REEEELOADDDINNNGGG");
    }

    // Check if reload animation is finished
    if (reloading && viewmodel.model.current_anim_finished)
    {
        reloading = false;
        smodel_animation_change(&viewmodel.model, weapons[current_weapon].anim_idle);
    }
}


/*
player_update
Called every Tick
*/
void player_update()
{

    check_collisions(&player, true, COLLISION_MASK_ALL);
    player_handle_crouch();


    if (global_paused || global_game_loading || global_console_open) 
    {
        player.gameobject.velocity = Vector3Zero();
        return;
    }

    float delta = GetFrameTime();

    player_handle_reload();
    player_handle_shoot(delta);
    player_movement();
    apply_gravity(&player);
    player_handle_jump();
    camera_follow_player(&camera, &player);
}


/*
player_draw
Any calls to draw 3d objects on the player can be
ie. collision box shape etc
For drawing weapons, hands, arms etc. see Viewmodel to draw instead
This is for actual player model etc not the arms or view model weapon.
*/
void player_draw()
{
    #ifdef DEBUG
    DrawBoundingBox(player.gameobject.collision_box.bounding_box, RED);
    #endif
}


/*
player_handle_crouch
Handles when the player presses the Crouch key,
and toggles the player crouching moving up/down
*/
void player_handle_crouch()
{
    if (IsKeyPressed(BUTTON_CROUCH_KEY) || IsGamepadButtonPressed(GAMEPAD_P1, BUTTON_CROUCH_PAD))
    {
        float old_height = player.gameobject.collision_box.size.y;
        float center_y = player.gameobject.position.y;
        float feet_y = center_y - (old_height * 0.5f);  // get current bottom of box

        if (!global_player_crouching)
        {
            global_player_crouching = true;
            global_camera_height = CAMERA_HEIGHT_CROUCH;

            player.gameobject.collision_box.size.y = PLAYER_HEIGHT * 0.34f;
            float new_height = player.gameobject.collision_box.size.y;

            // Adjust position to keep feet locked
            player.gameobject.position.y = feet_y + (new_height * 0.5f);

            if (global_player_crouchboost && !global_player_onground)
            {
                player.gameobject.velocity.y = 20;
                global_player_crouchboost = false;
            }
        }
        else
        {
            // Optional: check if headroom is clear

            // Check if there's room to stand up
            CollisionBox original_box = player.gameobject.collision_box;
            float test_height = PLAYER_HEIGHT;
            Vector3 test_pos = { player.gameobject.position.x, feet_y + (test_height * 0.5f), player.gameobject.position.z };

            // Create a test box for standing height
            player.gameobject.collision_box.size.y = test_height;
            collisionbox_set_position(&player.gameobject.collision_box, test_pos);

            if (!place_meeting_solid(&player.gameobject, COLLISION_MASK_SOLID, 1)) // 1 = is_player
            {
                // Room to stand up
                global_player_crouching = false;
                global_camera_height = CAMERA_HEIGHT_DEFAULT;

                player.gameobject.collision_box.size.y = test_height;
                player.gameobject.position.y = test_pos.y;
            }
            else
            {
                // Blocked: restore original box
                player.gameobject.collision_box = original_box;
                collisionbox_set_position(&player.gameobject.collision_box, player.gameobject.position);
            }

        }
    }
}






/*
player_movement
Handles player directional input, movement, acceleration, and friction
*/
void player_movement()
{   
    float input_x, input_z;

    // keyboard movement
    input_x = (IsKeyDown(BUTTON_MOVE_LEFT_KEY) - IsKeyDown(BUTTON_MOVE_RIGHT_KEY));
    input_z = (IsKeyDown(BUTTON_MOVE_FORWARD_KEY) - IsKeyDown(BUTTON_MOVE_BACKWARD_KEY));

    // gamepad movement
    if (IsGamepadAvailable(GAMEPAD_P1) && (input_x == 0 && input_z == 0))
    {
        float lx = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
        float ly = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);

        float deadzone = 0.2f;

        if (fabsf(lx) < deadzone) lx = 0.0f;
        if (fabsf(ly) < deadzone) ly = 0.0f;

        input_x = -lx;
        input_z = -ly;
    }

    float input_len = sqrtf(input_x * input_x + input_z * input_z);
    if (input_len > 0.0f) {
        input_x /= input_len;
        input_z /= input_len;
    }

    Vector3 forward = { sinf(global_cam_yaw), 0.0f, cosf(global_cam_yaw)  };
    Vector3 right   = { cosf(global_cam_yaw), 0.0f, -sinf(global_cam_yaw) };

    Vector3 move_dir = {
        forward.x * input_z + right.x * input_x,
        0.0f,
        forward.z * input_z + right.z * input_x
    };

    float accel = 5.0f;
    float fric = 10.0f;
    float air_fric = 1.0f;
    float dt = GetFrameTime();

    if (input_len > 0.0f)
    {
        global_player_moving = true;
        Vector3 desired = Vector3Scale(move_dir, player.gameobject.speed * input_len);
        Vector3 delta = Vector3Subtract(desired, (Vector3){ player.gameobject.velocity.x, 0.0f, player.gameobject.velocity.z });
        Vector3 accel_step = Vector3Scale(delta, accel * dt);
        player.gameobject.velocity.x += accel_step.x;
        player.gameobject.velocity.z += accel_step.z;
    }
    else
    {
        // apply friction only when no input
        global_player_moving = false;
        float current_fric = global_player_onground ? fric : air_fric;
        player.gameobject.velocity.x = Lerp(player.gameobject.velocity.x, 0.0f, current_fric * dt);
        player.gameobject.velocity.z = Lerp(player.gameobject.velocity.z, 0.0f, current_fric * dt);
    }
}


/*
player_jump
Call this to invoke the player jumping into the air
*/
void player_jump()
{
    player.gameobject.velocity.y = player.jump_height;

    if (global_player_inwater) 
        return;

    global_player_onground = false;
}


/*
player_handle_jump
Called to handle when / if the player CAN jump
*/
void player_handle_jump()
{

    // jump
    if (((GetMouseWheelMove() < 0) || IsKeyPressed(BUTTON_JUMP_KEY) || (IsGamepadButtonPressed(GAMEPAD_P1, BUTTON_JUMP_PAD))) && global_player_onground)
        player_jump();
}