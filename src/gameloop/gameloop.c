#include "gameloop.h"
#include <stdio.h>
#include "raylib.h"
#include "raygui.h"
#include "raymath.h"

#include "defs.h"
#include "global.h"

#include "camera.h"
#include "map.h"
#include "player.h"

#include "init.h"
#include "input.h"
#include "update.h"
#include "draw.h"
#include "draw_viewmodel.h"
#include "draw_gui.h"
#include "cleanup.h"

// Program Entry Point
// -----------------------------
int gameloop()
{
    // Initialization
    // -----------------------------
    SetConfigFlags(FLAG_MSAA_4X_HINT); // Multi Sampling Anti Aliasing 4X

    init();
    player_init();
    GuiLoadStyle(STYLE_AMBER);

    // Main Game Loop
    // -----------------------------
    while(!WindowShouldClose())
    {
        SetExitKey(false); // Disable exit key (ESC)
        
        input();
        update();
        player_update();
        lights_update();

        // Draw
        // -----------------------------
        BeginDrawing();

            ClearBackground(BLACK);
            BeginMode3D(camera);
            BeginShaderMode(sh_light);
                draw();
                player_draw();
                draw_viewmodel(); 
            EndShaderMode();
            EndMode3D();

            draw_gui();

        EndDrawing();
        // draw -------------------------
    }

    // De-Initialization
    // -----------------------------
    clean_up();
    UnloadShader(sh_light);
    CloseWindow();
    return false;
    // de-init ----------------------
}