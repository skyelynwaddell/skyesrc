#ifndef MATERIAL_H
#define MATERIAL_H

#include "skyelib.h"


typedef enum sMATERIAL_TYPE {
    MATERIAL_TYPE_DEFAULT,
    MATERIAL_TYPE_METAL,
} sMATERIAL_TYPE;

typedef struct sMaterial {
    char name[64];
    sMATERIAL_TYPE type;
    Texture2D texture;
    Shader *shader;
} sMaterial;

#define MAX_MATERIALS 4098
extern sMaterial materials[4098];
extern int material_count;



#endif // MATERIAL_H