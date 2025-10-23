#include "raylib.h"
#include "Camera3D.h"
#include "Scene.h"

int main() {
    InitWindow(800, 600, "Raylib + C++ Setup Test");
    SetTargetFPS(60);

    Camera camera = GetDefaultCamera();

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(GRAY);

        BeginMode3D(camera);
        DrawSceneCube();
        EndMode3D();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
