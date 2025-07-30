#include "skyelib.h"
#include "global.h"
#include "enemy.h"

/*
place_meeting
returns TRUE if GameObject is colliding with a solid object
*/
int place_meeting_solid(GameObject *object, COLLISION_MASK mask, int is_player)
{
    // --- Map Collisions ---
    if (mask == COLLISION_MASK_ALL || mask == COLLISION_MASK_SOLID)
    {
        for (int i=0; i < map.model_count; i++)
        {
            Geometry *geo = &map.models[i];
            if (is_player)
                global_player_inwater = false;

            Vector3 center = Vector3Lerp(geo->bounds.min, geo->bounds.max, 0.5f);
            float dist = Vector3Distance(center, object->position);
            if (dist > COLLISION_DISTANCE) continue; // too far to care

            // potential collision -- do cheap math first
            if ((CheckCollisionBoxes(object->collision_box.bounding_box, map.models[i].bounds)) == false) 
                continue;

            //actual collision -- do expensive math
            if (CheckCollisionBoxesExt(object->collision_box.bounding_box, map.models[i].collision))
            {
                if (string_equals(geo->func->classname, "func_water") && is_player)
                {
                    global_player_inwater = true;
                    return false;
                }

                return true;
            }
        }
    }

    // --- Enemy Collisions ---
    if (mask == COLLISION_MASK_ALL || mask == COLLISION_MASK_ENEMY)
    {
        for (int i=0; i < MAX_ENEMIES; i++)
        {
            Enemy *e = enemies[i];
            if (!e || e->health <= 0) continue;
            if (e == object) continue;

            Vector3 center = Vector3Lerp(e->gameobject.collision_box.bounding_box.min, e->gameobject.collision_box.bounding_box.max, 0.5f);
            float dist = Vector3Distance(center, object->position);
            if (dist > COLLISION_DISTANCE) continue; // too far to care

            // collision
            if ((CheckCollisionBoxes(object->collision_box.bounding_box, e->gameobject.collision_box.bounding_box))) 
                return true;
        }
    }

    return false;
}


/*
CheckCollisionBoxesExt
Checks for a collision between a BoundingBox and a CollisionPolygon
For expensive calculations against players simple BoundingBox and a Polygonal Brush/Geometry in the world
*/
int CheckCollisionBoxesExt(BoundingBox box, CollisionPolygon shape)
{
    for (int i=0; i<shape.count; i++)
    {
        if (check_AABB_triangle_SAT(box, shape.triangles[i])) 
            return true;
    }
    return false;
}


/*
check_AABB_triangle_SAT
Used to check collision between BoundingBox and a Triangle
*/
int check_AABB_triangle_SAT(BoundingBox box, Triangle tri)
{
    // get triangle vertices relative to box center
    Vector3 boxCenter = Vector3Scale(Vector3Add(box.min, box.max), 0.5f);
    Vector3 boxHalfSize = Vector3Subtract(box.max, boxCenter);

    Vector3 v0 = Vector3Subtract(tri.a, boxCenter);
    Vector3 v1 = Vector3Subtract(tri.b, boxCenter);
    Vector3 v2 = Vector3Subtract(tri.c, boxCenter);

    Vector3 f0 = Vector3Subtract(v1, v0);
    Vector3 f1 = Vector3Subtract(v2, v1);
    Vector3 f2 = Vector3Subtract(v0, v2);

    // 13 possible separating axes
    Vector3 axes[13] = {
        // box normals (x, y, z)
        {1,0,0}, {0,1,0}, {0,0,1},
        // cross products of edges
        Vector3CrossProduct((Vector3){1,0,0}, f0),
        Vector3CrossProduct((Vector3){1,0,0}, f1),
        Vector3CrossProduct((Vector3){1,0,0}, f2),
        Vector3CrossProduct((Vector3){0,1,0}, f0),
        Vector3CrossProduct((Vector3){0,1,0}, f1),
        Vector3CrossProduct((Vector3){0,1,0}, f2),
        Vector3CrossProduct((Vector3){0,0,1}, f0),
        Vector3CrossProduct((Vector3){0,0,1}, f1),
        Vector3CrossProduct((Vector3){0,0,1}, f2),
        // triangle normal
        Vector3CrossProduct(f0, f1)
    };

    for (int i = 0; i < 13; i++)
    {
        Vector3 axis = axes[i];
        if (Vector3Length(axis) < 0.0001f) continue; // skip near-zero axes

        // project triangle onto axis
        float minT = INFINITY, maxT = -INFINITY;
        float dots[3] = {
            Vector3DotProduct(v0, axis),
            Vector3DotProduct(v1, axis),
            Vector3DotProduct(v2, axis)
        };

        for (int j = 0; j < 3; j++) {
            if (dots[j] < minT) minT = dots[j];
            if (dots[j] > maxT) maxT = dots[j];
        }

        // project box onto axis
        float r =
            boxHalfSize.x * fabsf(Vector3DotProduct(axis, (Vector3){1,0,0})) +
            boxHalfSize.y * fabsf(Vector3DotProduct(axis, (Vector3){0,1,0})) +
            boxHalfSize.z * fabsf(Vector3DotProduct(axis, (Vector3){0,0,1}));

        // SAT test
        if (maxT < -r || minT > r) {
            return false; // separation found - NO collision
        }
    }

    return true; // no separation axis found — collision!
}


/*
player_apply_gravity
Applies vertical gravity to the player object
*/
void apply_gravity(GameObject *obj)
{
    if (global_paused || global_game_loading) 
    {
        obj->velocity = Vector3Zero();
        return;
    }
    int _grvty = global_player_inwater ? obj->gravity/2 : obj->gravity;
    obj->velocity.y += _grvty * GetFrameTime();
}


/*
check_collisions
Handles all the X Y Z collisions between obj and walls/floors/ceilings
*/
void check_collisions(GameObject *obj, int is_player, COLLISION_MASK mask)
{
    Vector3 original_pos = obj->position;
    float dt = GetFrameTime();


    // ---- Y axis (gravity + jump) ----
    Vector3 moveY = { 0.0f, obj->velocity.y * dt, 0.0f };
    Vector3 testY = Vector3Add(obj->position, moveY);
    collisionbox_set_position(&obj->collision_box, testY);

    if (!place_meeting_solid(obj, mask, is_player)) {
        obj->position.y += moveY.y;
        
        if (is_player) 
            global_player_onground = false;
    } else {
        if (obj->velocity.y < 0 && is_player) 
        {
            global_player_onground = true;
            global_player_crouchboost = true;
        }

        obj->velocity.y = 0.0f;
    }

    if (is_player) {
        // Check slightly below current position to detect ground
        const float ground_check_offset = 2.0f;
        Vector3 ground_check_pos = Vector3Add(obj->position, (Vector3){0.0f, -ground_check_offset, 0.0f});
        collisionbox_set_position(&obj->collision_box, ground_check_pos);

        if (place_meeting_solid(obj, mask, is_player)) {
            global_player_onground = true;
        } else {
            global_player_onground = false;
        }

        // Restore original collision box position so it's not offset for other logic
        collisionbox_set_position(&obj->collision_box, obj->position);
    }

    // ---- X axis ----
    Vector3 moveX = { obj->velocity.x * dt, 0.0f, 0.0f };
    Vector3 testX = Vector3Add(obj->position, moveX);
    collisionbox_set_position(&obj->collision_box, testX);

    if (!place_meeting_solid(obj, mask, is_player)) {
        obj->position.x += moveX.x;
        if (is_player) global_player_onXwall = false;
    } else {
        // Try to step up
        Vector3 tryStep = obj->position;
        tryStep.y += PLAYER_STEP_HEIGHT;
        tryStep.x += moveX.x;

        collisionbox_set_position(&obj->collision_box, tryStep);
        if (!place_meeting_solid(obj, mask, is_player)) {
            obj->position = tryStep;
            obj->velocity.y = 0.0f;
            if (is_player) global_player_onXwall = false;
        } else {
            obj->velocity.x = 0.0f;
            if (is_player) global_player_onXwall = true;
        }
    }

    // ---- Z axis ----
    Vector3 moveZ = { 0.0f, 0.0f, obj->velocity.z * dt };
    Vector3 testZ = Vector3Add(obj->position, moveZ);
    collisionbox_set_position(&obj->collision_box, testZ);

    if (!place_meeting_solid(obj, mask, is_player)) {
        obj->position.z += moveZ.z;

        if (is_player)
            global_player_onZwall = false;
    } else {
        // Try to step up
        Vector3 tryStep = obj->position;
        tryStep.y += PLAYER_STEP_HEIGHT;
        tryStep.z += moveZ.z;

        collisionbox_set_position(&obj->collision_box, tryStep);
        if (!place_meeting_solid(obj, mask, is_player)) {
            obj->position = tryStep;
            obj->velocity.y = 0.0f;
            if (is_player) global_player_onZwall = false;
        } else {
            obj->velocity.z = 0.0f;
            if (is_player) global_player_onZwall = true;
        }
    }

    // Final update of the collision box
    collisionbox_set_position(&obj->collision_box, obj->position);
}


/*
raycast_start()
Called at the start of the game loop to initialize the raycast
*/
void raycast_start()
{
    global_raycast_has_target = false;
    global_player_shooting = false;
} 


/*
raycast_check_bb()
Called to check if the raycast hits a BoundingBox.
Must be called on a BoundingBox every TICK or 
the boundingbox will ignore the raycast collision.
(ie. you cant shoot what you dont check)
*/
int raycast_check_bb(BoundingBox box)
{
    RayCollision hit = GetRayCollisionBox(global_raycast.ray, box);
    if (hit.hit && hit.distance < global_raycast.blocked_distance) {
        global_raycast.blocked_distance = hit.distance;
        global_raycast.has_hit = true;
        return true;
    }
    return false;
}


/*
raycast_check()
Checks for a collision between the global raycast
sets the gameobject is_hit variable to true

NOTE: Must remember to call raycast_reset after doing logic
or else the raycast will never lose target
*/
int raycast_check(GameObject *obj)
{
    BoundingBox world_bb = obj->collision_box.bounding_box;
    RayCollision ray = GetRayCollisionBox(global_raycast.ray, world_bb);

    if (ray.hit &&
        (!global_raycast.has_hit || ray.distance < global_raycast.blocked_distance))
    {
        // still blocked by something else?
        if (ray.distance < global_raycast.blocked_distance)
        {
            global_raycast_has_target = true;

            if (global_player_shooting)
            {
                obj->is_hit = true;
                return true;
            }
        }
    }

    obj->is_hit = false;
    return false;
}


/*
raycast_update()
This function is called every TICK before drawing
*/
void raycast_update()
{
    global_raycast.ray = CENTER_RAY(camera);
    global_raycast.has_hit = false;
    global_raycast.blocked_distance = FLT_MAX;
}


/*
raycast_reset
Called at the end of all drawing and update logic
*/
void raycast_reset(GameObject *obj)
{
    obj->is_hit = false;
}


/*
distance_to
Returns true if the gameobjects are equal or less to the distance
*/
int distance_to(GameObject *obj1, GameObject *obj2, float distance)
{
    float dist = Vector3Distance(obj1->position, obj2->position);
    return dist <= distance;
}


/*
distance_to
Returns true if the gameobjects are equal or less to the distance
*/
int distance_to_pos(Vector3 pos1, Vector3 pos2, float distance)
{
    float dist = Vector3Distance(pos1, pos2);
    return dist <= distance;
}


/*
distance_to_player
Returns true if the gameobjects are equal or less to the distance
*/
int distance_to_player(GameObject *obj1, float distance)
{
    float dist = Vector3Distance(obj1->position, player.gameobject.position);
    return dist <= distance;
}