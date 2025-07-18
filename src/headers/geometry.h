#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "raylib.h"
#include "collisionpolygon.h"

typedef struct {
    Model model;
    BoundingBox bounds;
    CollisionPolygon collision;
    Vector3 position;
} Geometry;



#endif // GEOMETRY_H