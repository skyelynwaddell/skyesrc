#ifndef BSP_H
#define BSP_H

#include "skyelib.h"

typedef float scalar_t;	// Scalar value,

// Structs declaration
#pragma pack(push,1)
typedef struct dentry_t			 // A Directory entry
{
    int32_t  offset;		     // Offset to entry, in bytes, from start of file
    int32_t  size;               // Size of entry in file, in bytes
} dentry_t;

typedef struct dheader_t		 // The BSP file header
{
    int32_t  version;            // Model version, must be 0x17 (23).
    dentry_t entities;           // List of Entities.
    dentry_t planes;             // Map Planes. // numplanes = size/sizeof(plane_t)
    dentry_t miptex;             // Wall Textures.
    dentry_t vertices;           // Map Vertices. // numvertices = size/sizeof(vertex_t)
    dentry_t visilist;           // Leaves Visibility lists.
    dentry_t nodes;              // BSP Nodes. // numnodes = size/sizeof(node_t)
    dentry_t texinfo;            // Texture Info for faces. // numtexinfo = size/sizeof(texinfo_t)
    dentry_t faces;              // Faces of each surface. // numfaces = size/sizeof(face_t)
    dentry_t lightmaps;          // Wall Light Maps.
    dentry_t clipnodes;          // clip nodes, for Models. // numclips = size/sizeof(clipnode_t)
    dentry_t leaves;             // BSP Leaves. // numlaves = size/sizeof(leaf_t)
    dentry_t lface;              // List of Faces.
    dentry_t edges;              // Edges of faces. // numedges = Size/sizeof(edge_t)
    dentry_t ledges;             // List of Edges.
    dentry_t models;             // List of Models. // nummodels = Size/sizeof(model_t)
} dheader_t;
#pragma pack(pop)


/* vec3_t */
typedef struct vec3_t			 // Vector or Position
{
    scalar_t x;                  // horizontal
    scalar_t y;                  // horizontal
    scalar_t z;                  // vertical
} vec3_t;


/* boundbox_t */
typedef struct boundbox_t		 // Bounding Box, Float values
{
    vec3_t   min;                // minimum values of X,Y,Z
    vec3_t   max;                // maximum values of X,Y,Z
} boundbox_t;


/* bboxshort_t */
typedef struct bboxshort_t			// Bounding Box, Short values
{
    uint16_t   min;                 // minimum values of X,Y,Z
    uint16_t   max;                 // maximum values of X,Y,Z
} bboxshort_t;


/* vertex_t */
typedef struct vertex_t
{
    float X;                    // X,Y,Z coordinates of the vertex
    float Y;                    // usually some integer value
    float Z;                    // but coded in floating point
} vertex_t;


/* edge_t */
typedef struct edge_t
{
    uint16_t vertex0;     // index of the start vertex
    uint16_t vertex1;     // index of the end vertex  // must be in [0,numvertices]
} edge_t;


/* surface_t */
typedef struct surface_t
{
    vec3_t   vectorS;           // S vector, horizontal in texture space)
    scalar_t distS;             // horizontal offset in texture space
    vec3_t   vectorT;           // T vector, vertical in texture space
    scalar_t distT;             // vertical offset in texture space
    uint32_t texture_id;        // Index of Mip Texture //  must be in [0,numtex]
    uint32_t animated;          // 0 for ordinary textures, 1 for water 
} surface_t;


/* face_t */
typedef struct face_t
{
    uint16_t plane_id;    // The plane in which the face lies - must be in [0,numplanes]
    uint16_t side;        // 0 if in front of the plane, 1 if behind the plane
    uint32_t ledge_id;    // first edge in the List of edges - must be in [0,numledges]
    uint16_t ledge_num;   // number of edges in the List of edges
    uint16_t texinfo_id;  // index of the Texture info the face is part of - must be in [0,numtexinfos]

    unsigned char typelight;    // type of lighting, for the face
    unsigned char baselight;    // from 0xFF (dark) to 0 (bright)
    unsigned char light[2];     // two additional light models  
    uint32_t lightmap;          // Pointer inside the general light map, or -1 // this define the start of the face light map
} face_t;


/* plane_t */
typedef struct plane_t
{
    vec3_t normal;              // Vector orthogonal to plane (Nx,Ny,Nz) // with Nx2+Ny2+Nz2 = 1
    scalar_t dist;              // Offset to plane, along the normal vector. // Distance from (0,0,0) to the plane
    uint32_t type;              // Type of plane, depending on normal vector.
} plane_t;


/* mipheader_t */
typedef struct mipheader_t		 // Mip texture list header
{
    uint32_t numtex;             // Number of textures in Mip Texture list
    uint32_t* offset;            // Offset to each of the individual texture
} mipheader_t;				     // from the beginning of mipheader_t


/* miptex_t */
typedef struct miptex_t
{
    char name[16];
    uint32_t width;
    uint32_t height;
    uint32_t offset1;
    uint32_t offset2;
    uint32_t offset4;
    uint32_t offset8;
} miptex_t;


/* model_t */
typedef struct model_t
{
    boundbox_t bound;			    // The bounding box of the Model
    vec3_t origin;				    // origin of model, usually (0,0,0)
    uint32_t node_id0;				// index of first BSP node
    uint32_t node_id1;				// index of the first Clip node
    uint32_t node_id2;				// index of the second Clip node
    uint32_t node_id3;				// usually zero
    uint32_t numleafs;				// number of BSP leaves
    uint32_t face_id;				// index of Faces
    uint32_t face_num;				// number of Faces
} model_t;


/* MipTextureData */
typedef struct MipTextureData
{
    char name[64];
    Vector2 size;
    uint32_t* data;      // Pixel data
    int dataCount;       // Number of pixels
} MipTextureData;


/* BspFile */
typedef struct BspFile
{
    plane_t* planes;
    int num_planes;

    vertex_t* vertices;
    int num_vertices;

    miptex_t* mip_textures;
    int num_mip_textures;

    MipTextureData* mip_textures_data;
    int num_mip_textures_data;

    face_t* faces;
    int num_faces;

    edge_t* edges;
    int num_edges;

    int32_t* ledges;
    int num_ledges;

    model_t* models;
    int num_models;

    surface_t* texture_infos;
    int num_texture_infos;

    uint8_t *lightmap_data;
    size_t lightmap_data_size;

    // Private data
    FILE* file;
    int is_open;
    int header_parsed; // 0 or 1
    dheader_t header;

} BspFile;


extern BspFile bsp_file; // Global BSP File loaded


/*
ReadType
Read a single object of type T
*/
#define ReadType(file, ptr) \
    fread((ptr), sizeof(*(ptr)), 1, (file)->file)


/*
ReadArray
Read an array of objects of type T
*/
#define ReadArray(file, ptr, count) \
    fread((ptr), sizeof(*(ptr)), (count), (file)->file)


/*
bsp_file_load
Open the file for reading in binary mode
*/
static inline int bsp_file_load(BspFile* bspFile, const char* path) 
{
    if (!bspFile) return -1;

    bspFile->file = fopen(path, "rb");
    bspFile->is_open = (bspFile->file != NULL);

    return bspFile->is_open ? 1 : -1;
}


/*
bsp_file_unload
Closes the BSP file
*/
static inline void bsp_file_unload(BspFile* bspFile) 
{
    if (bspFile && bspFile->file) {
        fclose(bspFile->file);
        bspFile->file = NULL;
        bspFile->is_open = 0;
    }
}


/*
bsp_seek
Seek to an offset from the beginning of the file
*/
static inline void bsp_seek(BspFile* bspFile, long offset) 
{
    if (bspFile && bspFile->file) {
        fseek(bspFile->file, offset, SEEK_SET);
    }
}


/*
bsp_load_header
Loads the header file and the contents contained
within the bsp file
*/
static int bsp_load_header(BspFile* bsp) 
{
    if (!bsp || !bsp->file) return -1;

    // Seek to file start
    fseek(bsp->file, 0, SEEK_SET);

    // Read header (fixed size)
    if (fread(&bsp->header, sizeof(dheader_t), 1, bsp->file) != 1) 
    {
        printf("Failed to read BSP header\n");
        return -1;
    }

    dentry_t lightmaps_lump = bsp->header.lightmaps;

    bsp->header_parsed = true;

    printf("BSP Header loaded:\n");
    printf("  Version: %d\n", bsp->header.version);
    printf("  Entities lump offset: %d, size: %d\n",
           bsp->header.entities.offset, bsp->header.entities.size);
    printf("  Miptex lump offset: %d, size: %d\n",
           bsp->header.miptex.offset, bsp->header.miptex.size);

    return 0;
}


/*
bsp_load_miptextures
Loads the texture data for each of the faces
*/
static int bsp_load_miptextures(BspFile* bsp) 
{
    if (!bsp || !bsp->file || !bsp->header_parsed) return -1;

    dentry_t* miptex_lump = &bsp->header.miptex;

    if (miptex_lump->size == 0) {
        printf("No miptextures found (size=0)\n");
        bsp->mip_textures = NULL;
        bsp->num_mip_textures = 0;
        return 0;
    }

    // Allocate memory to read the whole lump
    uint8_t* lump_data = (uint8_t*)malloc(miptex_lump->size);
    if (!lump_data) {
        printf("Failed to allocate lump buffer\n");
        return -1;
    }

    fseek(bsp->file, miptex_lump->offset, SEEK_SET);
    if (fread(lump_data, 1, miptex_lump->size, bsp->file) != miptex_lump->size) {
        printf("Failed to read miptex lump\n");
        free(lump_data);
        return -1;
    }

    int32_t* mip_data = (int32_t*)lump_data;
    int32_t num_textures = mip_data[0];

    printf("Number of miptextures: %d\n", num_textures);
    bsp->num_mip_textures = num_textures;
    bsp->mip_textures = (miptex_t*)malloc(sizeof(miptex_t) * num_textures);

    if (!bsp->mip_textures) {
        printf("Failed to allocate miptexture array\n");
        free(lump_data);
        return -1;
    }

    int32_t* offsets = &mip_data[1];

    for (int32_t i = 0; i < num_textures; i++) 
    {
        int32_t offset = offsets[i];
        if (offset == -1) {
            printf("MipTexture %d is unused (-1 offset)\n", i);
            memset(&bsp->mip_textures[i], 0, sizeof(miptex_t));
            continue;
        }

        if (offset < 0 || offset + sizeof(miptex_t) > miptex_lump->size) 
        {
            printf("Invalid offset for miptexture %d: %d\n", i, offset);
            continue;
        }

        miptex_t* src = (miptex_t*)(lump_data + offset);
        memcpy(&bsp->mip_textures[i], src, sizeof(miptex_t));

        printf("MipTexture %d: name='%s', size=%ux%u\n", i,
            bsp->mip_textures[i].name,
            bsp->mip_textures[i].width,
            bsp->mip_textures[i].height);
    }

    free(lump_data);
    return 0;
}


/*
bsp_load_lightmaps
Loads and parses the lightmap data from the bsp
*/
static int bsp_load_lightmaps(BspFile *bsp)
{
    if (!bsp || !bsp->file || !bsp->header_parsed) return -1;

    dentry_t *lightmaps_lump = &bsp->header.lightmaps;

    if (lightmaps_lump->size == 0) {
        printf("No lightmaps found (size=0)\n");
        return 0;
    }

    uint8_t *lightmap_data = (uint8_t*)malloc(lightmaps_lump->size);
    if (!lightmap_data) {
        printf("Failed to allocate memory for lightmaps\n");
        return -1;
    }

    bsp_seek(bsp, lightmaps_lump->offset);

    if (fread(lightmap_data, 1, lightmaps_lump->size, bsp->file) != (size_t)lightmaps_lump->size) {
        printf("Failed to read lightmap data\n");
        free(lightmap_data);
        return -1;
    }

    bsp->lightmap_data = lightmap_data;
    bsp->lightmap_data_size = lightmaps_lump->size;

    printf("Loaded lightmaps: %d bytes\n", lightmaps_lump->size);
    return 0;
}


/*
bsp_load
Call this from external files to load and parse
the bsp file
*/
int bsp_load(BspFile *bspFile, const char* path) 
{
    if (bsp_file_load(bspFile, path) < 0) 
    {
        printf("Failed to open BSP file at: %s\n", path);
        return -1;
    }

    unsigned char header_bytes[16];
    bsp_seek(bspFile, 0);
    fread(header_bytes, 1, 16, bspFile->file);

    printf("Header bytes: ");
    for(int i = 0; i < 16; i++)
        printf("%02X ", header_bytes[i]);
    printf("\n");

    if (bsp_load_header(bspFile) < 0) 
    {
        printf("Failed to load bsp headers\n");
        bsp_file_unload(bspFile);
        return -1;
    }

    if (bsp_load_miptextures(bspFile) < 0) 
    {
        printf("Failed to load MipTextures from bsp");
        bsp_file_unload(bspFile);
        return -1;
    }

    bsp_load_lightmaps(bspFile);
    // TODO: Load other lumps similarly...


    bsp_file_unload(bspFile);
    return 0;
}


/* palette.lmp */
unsigned char palette[768] =
{
// marked: colormap colors: cb = (colormap & 0xF0);cb += (cb >= 128 && cb < 224) ? 4 : 12;
// 0x0*
0,0,0,        15,15,15,     31,31,31,     47,47,47,     63,63,63,     75,75,75,     91,91,91,     107,107,107,
123,123,123,  139,139,139,  155,155,155,  171,171,171,  187,187,187,  203,203,203,  219,219,219,  235,235,235,
// 0x1*                                                   0 ^
15,11,7,      23,15,11,     31,23,11,     39,27,15,     47,35,19,     55,43,23,     63,47,23,     75,55,27,
83,59,27,     91,67,31,     99,75,31,     107,83,31,    115,87,31,    123,95,35,    131,103,35,   143,111,35,
// 0x2*                                                   1 ^
11,11,15,     19,19,27,     27,27,39,     39,39,51,     47,47,63,     55,55,75,     63,63,87,     71,71,103,
79,79,115,    91,91,127,    99,99,139,    107,107,151,  115,115,163,  123,123,175,  131,131,187,  139,139,203,
// 0x3*                                                   2 ^
0,0,0,        7,7,0,        11,11,0,      19,19,0,      27,27,0,      35,35,0,      43,43,7,      47,47,7,
55,55,7,      63,63,7,      71,71,7,      75,75,11,     83,83,11,     91,91,11,     99,99,11,     107,107,15,
// 0x4*                                                   3 ^
7,0,0,        15,0,0,       23,0,0,       31,0,0,       39,0,0,       47,0,0,       55,0,0,       63,0,0,
71,0,0,       79,0,0,       87,0,0,       95,0,0,       103,0,0,      111,0,0,      119,0,0,      127,0,0,
// 0x5*                                                   4 ^
19,19,0,      27,27,0,      35,35,0,      47,43,0,      55,47,0,      67,55,0,      75,59,7,      87,67,7,
95,71,7,      107,75,11,    119,83,15,    131,87,19,    139,91,19,    151,95,27,    163,99,31,    175,103,35,
// 0x6*                                                   5 ^
35,19,7,      47,23,11,     59,31,15,     75,35,19,     87,43,23,     99,47,31,     115,55,35,    127,59,43,
143,67,51,    159,79,51,    175,99,47,    191,119,47,   207,143,43,   223,171,39,   239,203,31,   255,243,27,
// 0x7*                                                   6 ^
11,7,0,       27,19,0,      43,35,15,     55,43,19,     71,51,27,     83,55,35,     99,63,43,     111,71,51,
127,83,63,    139,95,71,    155,107,83,   167,123,95,   183,135,107,  195,147,123,  211,163,139,  227,179,151,
// 0x8*                                                   7 ^        v 8
171,139,163,  159,127,151,  147,115,135,  139,103,123,  127,91,111,   119,83,99,    107,75,87,    95,63,75,
87,55,67,     75,47,55,     67,39,47,     55,31,35,     43,23,27,     35,19,19,     23,11,11,     15,7,7,
// 0x9*                                                   9 v
187,115,159,  175,107,143,  163,95,131,   151,87,119,   139,79,107,   127,75,95,    115,67,83,    107,59,75,
95,51,63,     83,43,55,     71,35,43,     59,31,35,     47,23,27,     35,19,19,     23,11,11,     15,7,7,
// 0xA*                                                  10 v
219,195,187,  203,179,167,  191,163,155,  175,151,139,  163,135,123,  151,123,111,  135,111,95,   123,99,83,
107,87,71,    95,75,59,     83,63,51,     67,51,39,     55,43,31,     39,31,23,     27,19,15,     15,11,7,
// 0xB*                                                  11 v
111,131,123,  103,123,111,  95,115,103,   87,107,95,    79,99,87,     71,91,79,     63,83,71,     55,75,63,
47,67,55,     43,59,47,     35,51,39,     31,43,31,     23,35,23,     15,27,19,     11,19,11,     7,11,7,
// 0xC*                                                  12 v
255,243,27,   239,223,23,   219,203,19,   203,183,15,   187,167,15,   171,151,11,   155,131,7,    139,115,7,
123,99,7,     107,83,0,     91,71,0,      75,55,0,      59,43,0,      43,31,0,      27,15,0,      11,7,0,
// 0xD*                                                  13 v
0,0,255,      11,11,239,    19,19,223,    27,27,207,    35,35,191,    43,43,175,    47,47,159,    47,47,143,
47,47,127,    47,47,111,    47,47,95,     43,43,79,     35,35,63,     27,27,47,     19,19,31,     11,11,15,
// 0xE*
43,0,0,       59,0,0,       75,7,0,       95,7,0,       111,15,0,     127,23,7,     147,31,7,     163,39,11,
183,51,15,    195,75,27,    207,99,43,    219,127,59,   227,151,79,   231,171,95,   239,191,119,  247,211,139,
// 0xF*                                                  14 ^
167,123,59,   183,155,55,   199,195,55,   231,227,87,   127,191,255,  171,231,255,  215,255,255,  103,0,0,
139,0,0,      179,0,0,      215,0,0,      255,0,0,      255,243,147,  255,247,199,  255,255,255,  159,91,83
}; //     


#endif // BSP_H