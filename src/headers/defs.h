#ifndef DEFS_H
#define DEFS_H

// player settings
#define PLAYER_HEIGHT 8.0f  // units tall
#define PLAYER_SPAWN_GAP PLAYER_HEIGHT + 20.0f // how high to increase the player spawn gap so isnt stuck in floor

#define PLAYER_WIDTH 1.0f   // units thick/wide
#define PLAYER_HEALTH_MAX 100
#define PLAYER_HEALTH 100
#define PLAYER_JUMPHEIGHT 35.0
#define PLAYER_SPEED 40.0f
#define CROSSHAIR_SIZE 3

#define GRAVITY_DEFAULT -60.0f
#define INTERACT_DISTANCE_ENEMY 50.0f
#define RENDER_DISTANCE 1000.0f // Max distance before we stop rendering something 
#define COLLISION_DISTANCE 60.0f // Max distance before we dont check for collisions

typedef enum ACTOR_STATE {
    STATE_IDLE,
    STATE_HURT,
    STATE_DEAD
} ENEMY_STATE;

#endif