#include "skyelib.h"

int texture_cache_count = 0;
Texture2D default_texture;
TextureCacheEntry texture_cache[MAX_TEXTURES];


/*
texture_get_default
-- gets the default texture for the game, usually returned when no texture is found to prevent a game crash
*/
Texture2D texture_get_default()
{
    return default_texture;
}


/*
texture_get_cached
*texture_name [char] - the texture filename to be loaded ie. "myamazingimage"
!!!!! expects the image to be in /textures folder and of filetype .png !!!!!!
-- this function is called to search the cache for a texture, or adds it if not found 
-- and then returns the texture2d of the texture name provided
*/
Texture2D texture_get_cached(char *texture_name)
{
    //search for already-cached texture
    for (int i = 0; i < texture_cache_count; i++)
    {
        if (strcmp(texture_cache[i].name, texture_name) == 0)
        {
            // found cached texture
            return texture_cache[i].texture;
        }
    }

    //prepare the texture path
    char path[128];
    snprintf(path, sizeof(path), TEXTURE_DIR "%s.png", texture_name);

    //attempt to load the texture
    Texture2D tex = LoadTexture(path);
    SetTextureFilter(tex, TEXTURE_FILTER);
    SetTextureWrap(tex, RL_TEXTURE_WRAP_REPEAT);

    
    //fallback to __TB_empty.png if failed
    if (tex.id == 0)
    {
        #ifdef DEBUG
            printf("Failed to load texture: %s\n", path);
        #endif

        //check if fallback is already cached
        for (int i = 0; i < texture_cache_count; i++)
        {
            if (string_equals(texture_cache[i].name, "__TB_empty"))
            {
                // returns cached ETB empty texture
                return texture_cache[i].texture;
            }
        }

        //fallback not cached yet, try to load it
        snprintf(path, sizeof(path), TEXTURE_DIR "__TB_empty.png");
        tex = LoadTexture(path);
        if (tex.id == 0)
        {
            // returns a blank texture in case of extra fail and no TB empty was found
            #ifdef DEBUG
                printf("FATAL: Failed to load fallback texture __TB_empty.png\n");
            #endif
            return default_texture;
        }

        //cache fallback texture
        strncpy(texture_cache[texture_cache_count].name, "__TB_empty", 63);
        texture_cache[texture_cache_count].texture = tex;
        texture_cache_count++;

        //return newly cached fallback texture
        return tex;
    }

    //cache the successfully loaded texture
    if (texture_cache_count < MAX_TEXTURES)
    {
        //adds the newly loaded texture to the cache
        strncpy(texture_cache[texture_cache_count].name, texture_name, 63);
        texture_cache[texture_cache_count].texture = tex;
        texture_cache_count++;
    }
    else
    {
        //we had no more space in the texture cache
        #ifdef DEBUG
            printf("Texture cache is full! Returning uncached texture.\n");
        #endif
    }

    // returns the newly loaded and cached texture
    return tex;
}


/*
texture_cache_cleanup()
Call this at game end to free up memory of
all the textures used and stored in memory
*/
void texture_cache_cleanup()
{
    for (int i = 0; i < texture_cache_count; i++) {
        UnloadTexture(texture_cache[i].texture);
    }
    texture_cache_count = 0;
}
