#include "skyelib.h"
#include "global.h"

sMaterial materials[MAX_MATERIALS];
int material_count = 0;

static sMaterial material_create(
    const char *name, 
    sMATERIAL_TYPE type,
    char *texture_name, 
    Shader *shader
){
    sMaterial mat = {0};
    strncpy(mat.name, name, sizeof(mat.name) - 1);
    mat.name[sizeof(mat.name) - 1] = '\0';

    mat.type = type;
    mat.texture = texture_get_cached((char *)texture_name);  // cast away const if needed

    mat.shader = shader;

    materials[material_count++] = mat;

    return mat;
}

void materials_init()
{
    material_create("water_blue", MATERIAL_TYPE_DEFAULT, "water_blue", &sh_water);
}

static int find_material_by_name(const char *texture_name)
{
    int found = false;
    for (int i=0; i<material_count; i++)
    {
        if (strncmp(materials[i].name, texture_name, sizeof(materials[i].name)) == 0)
        {
            found = i;
            break;
        }
    }

    return found;
}

void material_to_geometry(Geometry *geometry, const char *material_name)
{
    // Try to load custom shader by name
    int found = find_material_by_name(material_name);
    if (found > 0)
    {
        geometry->material = &materials[found];
        return;
    }

    // Create a material for this texture
    if (material_count < MAX_MATERIALS)
    {
        sMaterial new_mat = (sMaterial){0};
        strncpy(new_mat.name, material_name, sizeof(new_mat.name) - 1);
        new_mat.name[sizeof(new_mat.name) - 1] = '\0';

        printf("%s", material_name);

        new_mat.type = MATERIAL_TYPE_DEFAULT;
        new_mat.texture = texture_get_cached(material_name);
        new_mat.shader = &sh_light;

        material_count++;
        materials[material_count] = new_mat;
        geometry->material = &materials[material_count];

        return;
    }

    fprintf(stderr, "Error: Max Materials reached.");
}