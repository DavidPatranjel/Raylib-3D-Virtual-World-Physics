#include "raylib.h"
#include "Camera3DController.h"
#include "SceneManager.h"
#include "UIManager.h"

void DrawSceneCube();

int main() {
    InitWindow(1200, 800, "3D Virtual World Physics");
    SetTargetFPS(60);

    Camera3DController cameraController;
    SceneManager sceneManager;
    while (!WindowShouldClose()) {
        Camera camera = cameraController.GetCamera();
        cameraController.Update();
        sceneManager.Update();

        BeginDrawing();
        ClearBackground(GRAY);

        BeginMode3D(camera);
        sceneManager.Draw();
        EndMode3D();

        UIManager::DrawGameUI(sceneManager.GetUIData(), cameraController.IsFreeMode());

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
