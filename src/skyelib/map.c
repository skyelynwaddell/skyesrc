#include "skyelib.h"
#include "global.h"
#include "enemy.h"
#include "bsp.h"

Map map = {0}; // stores the currently loaded map
BspFile bsp_file = {0};

/*
parse_map
filename[const char*] -- the filename of the map to be loaded ie. "myamazingmap.map"
-- Stores the data about the currently loaded map.
*/
int map_parse(const char* filename)
{

    //bsp_load(&bsp_file, "gamedata/maps/test.bsp");

    global_paused = true;
    char fullpath[256];

    // preserve filename we need to load after, 
    // since we are about to wipe the map data from memory
    char preserved_filename[256];
    snprintf(preserved_filename, sizeof(preserved_filename), "%s", filename);

    // clear map data
    map_clear_models();

    #ifdef DEBUG
        printf("\n");
        printf("### LOADING MAP FILE ### \n");
    #endif
    console_log("Loading map file...");

    // add maps/ filepath to the filename
    snprintf(map.filename, sizeof(map.filename), "%s", preserved_filename);
    snprintf(fullpath, sizeof(fullpath), "%s%s", MAP_DIR, preserved_filename);


    // attempt to load the .map file
    FILE* file = fopen(fullpath, "r");
    if (!file)
    {
        console_log("Failed to open .map file! Tried searching in:");
        console_log(fullpath);
        perror("Failed to open .map file! \n");
        printf("Tried searching in: %s \n", fullpath);
        return false;
    }

    #ifdef DEBUG
        printf("    Successfully opened map: %s \n \n", fullpath);

        printf("-----------------------------\n");
        printf("### MAP PROPERTIES ### \n");
    #endif

    char line[MAX_LINE];
    int in_entity = false;
    int in_brush = false;
    int current_entity_has_brush = false;

    Brush current_brush = {0};
    int current_brushface_index = 0;

    Entity current_entity = {0};
    int current_entity_index = 0;

    console_log("Creating brushes & entities...");

    // Loop through all text in the .map file
    while (fgets(line, sizeof(line), file))
    {
        char* trimmed = line;

        // trim leading whitespaces
        while(*trimmed == ' ' || *trimmed == '\t') trimmed++;

        //skip comments and empty lines
        if (trimmed[0] == '/' && trimmed[1] == '/') continue;
        if (trimmed[0] == '\n' || trimmed[0] == '\0') continue;

        /*
        Object Start {
        ----------------------------------
        */
        if (string_equals(trimmed, "{\n"))
        {
            if (in_entity && !in_brush)
            {
                // Brush Start
                in_brush = true;
                current_brushface_index = 0; // restart brush 
                memset(&current_brush, 0, sizeof(Brush)); //clear brush struct
            }
            else if (!in_entity)
            {
                // Entity Start
                in_entity = true;
                current_entity_index = 0; // restart entity
                memset(&current_entity, 0, sizeof(Entity));
            }
            continue;
        }
        /*
        ----------------------------------
        */


        /*
        Object End }
        ----------------------------------
        */
        if (string_equals(trimmed, "}\n"))
        {
            if (in_brush)
            {
                current_brush.func = current_entity;
                if (map.brush_count < MAX_BRUSHES) 
                    map.brushes[map.brush_count++] = current_brush;
                in_brush = false;
            }
            
            else if (in_entity)
            {
                /*
                Parse Entity Classname's
                ----------------------------------
                */
                    // info_player_start
                    if (string_equals(current_entity.classname, "info_player_start"))
                    {
                        global_player_spawn = trench_to_raylib_origin(
                            (Vector3){
                                current_entity.origin.x,
                                current_entity.origin.y + PLAYER_SPAWN_GAP,
                                current_entity.origin.z
                            }
                        );
                    }

                    // light
                    // if (string_equals(current_entity.classname, "light"))
                    // {
                    //     LightObject new_light = light_create(
                    //         (Color){
                    //             current_entity.color.r,
                    //             current_entity.color.g,
                    //             current_entity.color.b,
                    //             current_entity.color.a
                    //         },
                    //         (float)current_entity.brightness,
                    //         trench_to_raylib_origin(current_entity.origin),
                    //         (float)current_entity.radius
                    //     );

                    //     // create & store light object
                    //     map.lights[map.light_count++] = new_light; 
                    // }

                    // monster_shotgunner
                    if (string_equals(current_entity.classname, "monster_shotgunner"))
                    {
                        enemy_create(
                            ENEMY_SHOTGUNNER, 
                            trench_to_raylib_origin((Vector3){
                                current_entity.origin.x,
                                current_entity.origin.y + PLAYER_SPAWN_GAP,
                                current_entity.origin.z
                        }));
                    }
                /*
                ----------------------------------
                */

                // Add Entity to array
                if (map.entity_count < MAX_ENTITIES) 
                    map.entities[map.entity_count++] = current_entity;

                in_entity = false;
            }
            continue;
        }
        /*
        ----------------------------------
        */


        /*
        Parse Entity
        ----------------------------------
        */
        if (in_entity && !in_brush)
        {
            char key[128], value[128];

            if (sscanf(trimmed, "\"%127[^\"]\" \"%127[^\"]\"", key, value) == 2) 
            {
                #ifdef DEBUG
                    if (strstr(key, "classname") == 0)
                        printf("  Property: %s = %s\n", key, value);
                    else
                    {
                        printf("-----------------------------\n\n");
                        printf("-----------------------------\n");
                        printf("### Entity: %s = %s ###\n", key, value);
                    }
                #endif
                    

                /*
                Map Version
                ----------------------------------
                */
                if (string_equals(key, "mapversion"))
                {
                    map.mapversion = atoi(value);
                    if (map.mapversion != 220)
                    {
                        printf("Only support for valve 220 .map files currently...");
                        CloseWindow();
                    }
                }
                /*
                ----------------------------------
                */


                /*
                Set Current Entity Properties
                ----------------------------------
                */
                    // ### mandatory properties ###

                    // classname
                    if (string_equals(key, "classname"))
                        strcpy(current_entity.classname, value);

                    // origin
                    if (string_equals(key, "origin"))
                        sscanf(value, "%f %f %f", &current_entity.origin.x, &current_entity.origin.z, &current_entity.origin.y);

                    // ### entity specific properties ###

                    // color
                    if (string_equals(key, "color")) 
                    {
                        float r, g, b;
                        if (sscanf(value, "%f %f %f", &r, &g, &b) == 3) {
                            current_entity.color.r = (unsigned char)r;
                            current_entity.color.g = (unsigned char)g;
                            current_entity.color.b = (unsigned char)b;
                        } else {
                            printf("Warning: Failed to parse color value: %s\n", value);
                        }
                    }

                    // alpha
                    if (string_equals(key, "alpha"))
                    {
                        float a;
                        if (sscanf(value, "%f", &a) == 1) 
                        {
                            current_entity.alpha = a;
                        }
                    }


                    // brightness
                    if (string_equals(key, "brightness"))
                        sscanf(value, "%f", &current_entity.brightness);

                    // radius
                    if (string_equals(key, "radius"))
                        sscanf(value, "%f", &current_entity.radius);

                    // is_dangerous
                    if (string_equals(key, "is_dangerous"))
                        sscanf(value, "%d", &current_entity.is_dangerous);


                    

                /*
                ----------------------------------
                */
            }
        }
        /*
        ----------------------------------
        */


        /*
        Parse Brush Face
        ----------------------------------
        */
        if (in_brush)
        {
            BrushFace brushface = {0};
            char texture_name[64];

            int matched = sscanf(trimmed,
                "( %f %f %f ) ( %f %f %f ) ( %f %f %f ) %63s [ %f %f %f %f ] [ %f %f %f %f ] %i %f %f",
                &brushface.pos_1.x, &brushface.pos_1.y, &brushface.pos_1.z,
                &brushface.pos_2.x, &brushface.pos_2.y, &brushface.pos_2.z,
                &brushface.pos_3.x, &brushface.pos_3.y, &brushface.pos_3.z,
                texture_name,
                &brushface.uv_s.x, &brushface.uv_s.y, &brushface.uv_s.z, &brushface.uv_s.w,
                &brushface.uv_t.x, &brushface.uv_t.y, &brushface.uv_t.z, &brushface.uv_t.w,
                &brushface.uv_rotation, &brushface.u_scale, &brushface.v_scale
            );

            int SUCCESS = 21;
            if (matched == SUCCESS)
            {
                // success
                // get texture name
                strncpy(brushface.texture, texture_name, sizeof(brushface.texture));
                brushface.texture[sizeof(brushface.texture) - 1] = '\0';

                if (current_brushface_index < BRUSH_FACE_COUNT)
                {
                    current_brush.brush_faces[current_brushface_index++] = brushface;
                    current_brush.brush_face_count = current_brushface_index;
                }
            } 
            else 
            {
                // failed
                printf("!!! Failed to parse brush face line: %s (matched %d)\n", trimmed, matched);
            }
        }
        /*
        ----------------------------------
        */
    }
    #ifdef DEBUG
    printf("-----------------------------\n\n");
    printf("### MAP OBJECTS CREATED ### \n");
    printf("%i brushes. \n", map.brush_count);
    printf("%i lights. \n", map.light_count);
    printf("\n");

    //generate polygons
    printf("### GENERATING MAP ###\n");
    printf("Generating map polygons from brushes....\n");
    printf("Generating planes from brushfaces and resorting polygon vertices...\n");
    #endif

    console_log("Generating map geometry from brushes...");
    
    for (int i=0; i < map.brush_count; i++)
    {
        //printf("Generating Polygon: %i \n", i);
        polygon_generate_from_brush(&map.brushes[i]);
        
        // loop over all faces in brush
        for (int j= 0; j < map.brushes[i].brush_face_count; j++)
        {
            Plane plane = brushface_to_plane(map.brushes[i].brush_faces[j]);
            polygon_sort_vertices(&map.brushes[i].polys[j], (Vector3){ plane.normal.x, plane.normal.y, plane.normal.z });
        }
    }

    fclose(file);
    map_create_models();
    console_log("Map was successfully generated.");
    return true;
}



/*
map_create_models
-- creates a model from a polygonal brush
*/
void map_create_models()
{
    #ifdef DEBUG
        printf("Converting polygons into models... \n");
        printf("\n### LOADING UV TEXTURES ### \n");
    #endif

    for (int i = 0; i < map.brush_count; i++)
    {
        Brush *brush = &map.brushes[i];

        for (int j = 0; j < brush->brush_face_count; j++)
        {
            BrushFace *face = &brush->brush_faces[j];
            Polygon *poly = &brush->polys[j];

            if (poly->vertex_count < 3) continue;

            // Centroid calculation (raw)
            Vector3 centroid = {0};
            for (int i = 0; i < poly->vertex_count; i++) 
                centroid = Vector3Add(centroid, poly->vertices[i]);
            
            centroid = Vector3Scale(centroid, 1.0f / poly->vertex_count);

            int triangle_count = poly->vertex_count;
            Mesh mesh = {0};
            mesh.vertexCount = triangle_count * 3;
            mesh.triangleCount = triangle_count;

            mesh.vertices = (float *)MemAlloc(mesh.vertexCount * 3 * sizeof(float));
            mesh.texcoords = (float *)MemAlloc(mesh.vertexCount * 2 * sizeof(float));
            mesh.normals  = (float *)MemAlloc(mesh.vertexCount * 3 * sizeof(float));  // <-- ADD THIS

            int index = 0;
            for (int i = 0; i < triangle_count; i++) {
                Vector3 verts[3] = {
                    poly->vertices[(i + 1) % poly->vertex_count],
                    poly->vertices[i],
                    centroid
                };

                // Calculate triangle normal
                Vector3 edge1 = Vector3Subtract(verts[1], verts[0]);
                Vector3 edge2 = Vector3Subtract(verts[2], verts[0]);
                Vector3 normal = Vector3Normalize(Vector3CrossProduct(edge1, edge2));  // <-- FACE NORMAL

                Vector2 uvs[3];

                switch(map.mapversion)
                {
                    default: // Standard MAP Format
                        uvs[0] = polygon_project_to_uv_standard(verts[0], face);
                        uvs[1] = polygon_project_to_uv_standard(verts[1], face);
                        uvs[2] = polygon_project_to_uv_standard(verts[2], face);
                    break;
                    
                    case 220: // Valve 220 Map Format
                        uvs[0] = polygon_project_to_uv_valve220(verts[0], face);
                        uvs[1] = polygon_project_to_uv_valve220(verts[1], face);
                        uvs[2] = polygon_project_to_uv_valve220(verts[2], face);
                    break;
                }

                for (int v = 0; v < 3; v++) {
                    // Coordinate conversion: (x, y, z) → (x, z, -y)
                    Vector3 p = verts[v];
                    Vector3 pos = trench_to_raylib_origin((Vector3){p.x,p.z,p.y});

                    // Store vertex
                    mesh.vertices[index * 3 + 0] = pos.x;
                    mesh.vertices[index * 3 + 1] = pos.y;
                    mesh.vertices[index * 3 + 2] = pos.z;

                    mesh.texcoords[index * 2 + 0] = uvs[v].x;
                    mesh.texcoords[index * 2 + 1] = uvs[v].y;

                    // Assign the face normal to each vertex
                    mesh.normals[index * 3 + 0] = normal.x;
                    mesh.normals[index * 3 + 1] = normal.y;
                    mesh.normals[index * 3 + 2] = normal.z;

                    index++;
                }
            }

            // AABB Bounds - Create bounding box / collision shape for simple checks
            Vector3 min = { FLT_MAX, FLT_MAX, FLT_MAX };
            Vector3 max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

            for (int v = 0; v < mesh.vertexCount; v++) {
                Vector3 vert = {
                    mesh.vertices[v * 3 + 0],
                    mesh.vertices[v * 3 + 1],
                    mesh.vertices[v * 3 + 2],
                };

                if (vert.x < min.x) min.x = vert.x;
                if (vert.y < min.y) min.y = vert.y;
                if (vert.z < min.z) min.z = vert.z;

                if (vert.x > max.x) max.x = vert.x;
                if (vert.y > max.y) max.y = vert.y;
                if (vert.z > max.z) max.z = vert.z;
            }
            BoundingBox bounds = (BoundingBox){ min, max };

            int tri_count = mesh.vertexCount / 3;

            CollisionPolygon shape = {
                .triangles = MemAlloc(sizeof(Triangle) * tri_count),
                .count = tri_count
            };

            // Polygonal Collision Shape -- More advanced detection
            for (int i=0; i<tri_count; i++)
            {
                shape.triangles[i].a = (Vector3){
                mesh.vertices[(i * 3 + 0) * 3 + 0],
                mesh.vertices[(i * 3 + 0) * 3 + 1],
                mesh.vertices[(i * 3 + 0) * 3 + 2]
            };
            shape.triangles[i].b = (Vector3){
                mesh.vertices[(i * 3 + 1) * 3 + 0],
                mesh.vertices[(i * 3 + 1) * 3 + 1],
                mesh.vertices[(i * 3 + 1) * 3 + 2]
            };
            shape.triangles[i].c = (Vector3){
                mesh.vertices[(i * 3 + 2) * 3 + 0],
                mesh.vertices[(i * 3 + 2) * 3 + 1],
                mesh.vertices[(i * 3 + 2) * 3 + 2]
            };
            }

            UploadMesh(&mesh, false);
            Model model = LoadModelFromMesh(mesh);

            Geometry geometry;
            geometry.func = &brush->func;

            material_to_geometry(&geometry, face->texture);

            if (string_equals(geometry.func->classname, "func_water")) 
                geometry.material->shader = &sh_water;

            model.materials[0].shader = *geometry.material->shader;
            model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = geometry.material->texture;
            geometry.model = model;
            geometry.bounds = bounds;
            geometry.collision = shape;
            geometry.position = trench_to_raylib_origin(centroid);

            Vector3 center = Vector3Lerp(geometry.bounds.min, geometry.bounds.max, 0.5f); // center of the box
            Vector3 extent = Vector3Subtract(geometry.bounds.max, center);     // half-dimensions
            Vector3 diagonal = Vector3Subtract(geometry.bounds.max, geometry.bounds.min);

            float frustum_padding = 3.0f; // padding for frustum checks so we dont clip the model when in view
            geometry.bounding_radius = Vector3Length(diagonal) * frustum_padding;

            map.models[map.model_count++] = geometry;            
        }
    }
    #ifdef DEBUG
        printf("\nMap was successfully generated! \n \n");
    #endif
}


/*
map_clear_models
-- clears all the map models from memory
-- clear memory because raylib does not
-- call this when the game ends
*/
void map_clear_models()
{
    for (int i=0; i < map.model_count; i++)
    {
        UnloadModel(map.models[i].model);
    }
    memset(&map, 0, sizeof(map)); // clear map safely after
    map.model_count = 0;
    map.light_count = 0;
}


static void map_draw_decals()
{
    rlDisableBackfaceCulling();
    BeginBlendMode(BLEND_ALPHA);

        for (int i=0; i<MAX_BULLET_HOLES; i++)
        {
            Decal *bh = &bullet_holes[i];
            if (bh->active == false) continue;
            Vector3 normal = bh->normal;
            
            Vector3 world_up = (Vector3){0, 1, 0};
                if (fabsf(Vector3DotProduct(world_up, normal)) > 0.99f)
                    world_up = (Vector3){1, 0, 0};

                Vector3 right = Vector3Normalize(Vector3CrossProduct(world_up, normal));
                Vector3 up = Vector3CrossProduct(normal, right);

                float half_size = bullet_holes[i].size * 0.5f;
                Vector3 center = bullet_holes[i].position;

                Vector3 corner1 = Vector3Add(center, Vector3Scale(Vector3Add(Vector3Negate(right), up), half_size));
                Vector3 corner2 = Vector3Add(center, Vector3Scale(Vector3Add(right, up), half_size));
                Vector3 corner3 = Vector3Add(center, Vector3Scale(Vector3Add(right, Vector3Negate(up)), half_size));
                Vector3 corner4 = Vector3Add(center, Vector3Scale(Vector3Add(Vector3Negate(right), Vector3Negate(up)), half_size));

                rlSetTexture(bullet_holes[i].texture.id);

                rlBegin(RL_QUADS);
                    rlNormal3f(normal.x, normal.y, normal.z);

                    rlTexCoord2f(0, 0); rlVertex3f(corner1.x, corner1.y, corner1.z);
                    rlTexCoord2f(1, 0); rlVertex3f(corner2.x, corner2.y, corner2.z);
                    rlTexCoord2f(1, 1); rlVertex3f(corner3.x, corner3.y, corner3.z);
                    rlTexCoord2f(0, 1); rlVertex3f(corner4.x, corner4.y, corner4.z);
                rlEnd();

                rlSetTexture(0);
            }

    rlEnableBackfaceCulling();
    EndBlendMode();
    
}


/*
map_draw
-- draw all polygons generated in the map
*/
void map_draw()
{
    map_draw_models();
}


static void BeginWaterMode(Geometry *geo)
{
    // func_water effects
    if (string_equals(geo->func->classname, "func_water"))
    {
        rlDisableBackfaceCulling();
        BeginBlendMode(BLEND_ALPHA);
        int loc_alpha = GetShaderLocation(sh_water, "water_alpha");
        SetShaderValue(sh_water, loc_alpha, &geo->func->alpha, SHADER_UNIFORM_FLOAT);
    }
}
static void EndWaterMode(Geometry *geo)
{
    // Reset Water Effects
    if (string_equals(geo->func->classname, "func_water"))
    {
        rlEnableBackfaceCulling();
        EndBlendMode();
    }
}


#include "player.h"
static int frame_count = 0;
/*
map_draw_models
-- draws each model in the model array
*/
void map_draw_models()
{
    frame_count++;
    if (frame_count > 100) frame_count = 0;

    for (int i = 0; i < map.model_count; i++)
    {
        Geometry *geo = &map.models[i];
        geo->visible = false;

        // check distance to player
        if (distance_to_pos(geo->position, 
        player.gameobject.position, RENDER_DISTANCE) == false) 
        {
            geo->was_visible_lastframe = false;
            continue; // too far to care
        }

        // only run the check every 5 frames
        if (frame_count % 2 == 0)
        {
            // cheap frustum check - this one sucks
            //if (!frustum_check_sphere(geo->position, geo->bounding_radius, global_frustum)) 
            //  continue; 

            // expensive frustum check
            geo->was_visible_lastframe = frustum_check_boundingbox(geo->bounds, global_frustum);
        }

        if (geo->was_visible_lastframe == false) continue;

        if (strcmp(geo->func->classname, "func_water") != 0)
            raycast_check_poly(geo->collision);
    
        geo->visible = true;
        geo->material->shader = &sh_light;

        // Draw the map and decals
        BeginWaterMode(geo);
            map_draw_model(geo->model);
            map_draw_decals();
        EndWaterMode(geo);
    }
}


/*
map_draw_model
Wil draw the models in the room with correct positioning
If DEBUG flag enabled will draw wire frames as well
*/
#ifdef DEBUG
void map_draw_model(Model model){
    //DrawModel(model, (Vector3){0}, 1.0f, WHITE);
    //DrawModelWires(model, (Vector3){0}, 1.0f, RED);
}
#else
void map_draw_model(Model model){
    DrawModel(model, (Vector3){0}, 1.0f, WHITE);
}
#endif


/*
map_hotreload
DEBUG : Allows for you to instantly reload the map for testing changes when you press ENTER
WARNING : Dont be moving or you may fall through the floor
*/
void map_hotreload() {
    #ifdef DEV_MODE
    if (IsKeyPressed(KEY_BACKSLASH))
    {
        global_game_loading = true;
        char filename[256];
        snprintf(filename, sizeof(filename), "%s", map.filename); // safe copy
        map_parse(filename);
    }
    #endif
}
