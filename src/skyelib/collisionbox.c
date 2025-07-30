#include "skyelib.h"
#include "global.h"

/*
collisionbox_set
Sets all of a collision box's properties
*/
void collisionbox_set(CollisionBox *box, Vector3 _position, Vector3 _size, int _scale)
{
    box->position = _position;
    box->size = _size;
    box->scale = _scale;
    collisionbox_update_aabb(box);
}


/*
collisionbox_update_aabb
! Call after you update a CollisionBox position, scale and/or size
or else no affect will be made to the collisionbox !
*/
void collisionbox_update_aabb(CollisionBox *box)
{
    Vector3 half = Vector3Scale(box->size, 0.5f * box->scale);

    box->bounding_box.min = Vector3Subtract(box->position, half);
    box->bounding_box.max = Vector3Add(box->position, half);
}


/*
collisionbox_set_position
Sets the xyz of the collisionbox, use this to make collisionbox
follow a player etc.
*/
void collisionbox_set_position(CollisionBox *box, Vector3 _position)
{
    box->position = _position;
    collisionbox_update_aabb(box);
}