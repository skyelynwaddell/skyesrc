#include "skyelib.h"
#include "gameloop.h"
#include "player.h"
#include "enemy.h"
#include "global.h"

// Program Entry Point
// -----------------------------
int gameloop()
{
    // Initialization
    // -----------------------------
    init();

    // --- ENET Client Initialization ---
    // TODO : Move this to a multiplayer connection screen
    // after main menu is made
    server_online = true;
    enetclient_init();

    // Main Game Loop
    // -----------------------------
    while(game_is_running())
    {
        enetclient_update();
        window_check_for_resize();

        if (global_game_loading == false)
        {
            input();
            update();

            // Draw
            // -----------------------------
            BeginTextureMode(target);
                ClearBackground(BLACK);

                // 3D World
                // -----------------------------
                BeginMode3D(camera);

                    draw();
                    player_draw();
                    enemy_draw_all();

                EndMode3D();

                // View Model
                // -----------------------------
                BeginModeViewModel();
                BeginShaderMode(sh_viewmodel);

                    viewmodel_draw();

                EndShaderMode();
                EndModeViewModel();

                draw_gui();

            EndTextureMode();

            BeginDrawing();

                ClearBackground(BLACK);
                DrawTexturePro(
                    target.texture, 
                    (Rectangle){ 
                        0.0f, 0.0f, 
                        (float)target.texture.width, 
                        (float)-target.texture.height 
                    },
                    (Rectangle){ 
                        (GetScreenWidth() - ((float)GAME_SCREEN_WIDTH*window_scale))*0.5f, 
                        (GetScreenHeight() - ((float)GAME_SCREEN_HEIGHT*window_scale))*0.5f,
                        (float)GAME_SCREEN_WIDTH*window_scale, 
                        (float)GAME_SCREEN_HEIGHT*window_scale 
                    }, 
                    (Vector2){ 0, 0 }, 
                    0.0f, 
                    WHITE
                );
            
            EndDrawing();
        }
    }

    // De-Initialization
    // -----------------------------
    clean_up();
    CloseWindow();
    return false;
    // de-init ----------------------
}