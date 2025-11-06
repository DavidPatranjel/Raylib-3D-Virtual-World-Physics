//
// Created by David on 24/10/2025.
//

#include "SceneManager.h"
#include <iostream>

SceneManager::SceneManager() : objectManager(boxPosition, boxSize) {
}


void SceneManager::DrawSceneCube() const {

    // DrawCubeWires(boxPosition, boxSize, boxSize, boxSize, RED);
    // DrawSphereWires(spherePosition1, sphereRadius, 10, 10, GREEN);
    // DrawSphereWires(spherePosition2, sphereRadius, 10, 10, GREEN);
}

int SceneManager::GetObjectCount() {
    return objectManager.GetObjectCount();
}

void SceneManager::Update() {

    if (IsKeyReleased(KEY_P))
        objectManager.SpawnPrimitives();

    if (IsKeyReleased(KEY_O))
        objectManager.SpawnRandomObject();

    if (IsKeyPressed(KEY_A)) {
        Vector3 newVelocity = {1.0f, 0.0f, 0.0f};
        std::cout << "A" << std::endl;
        for (int i = 0; i < objectManager.GetPairsCount(); i++)
        {
            if (i % 2 != 0)
                objectManager.primitives.at(i).SetVelocity(newVelocity);
        }
    }
    if (IsKeyReleased(KEY_A)) {
        for (int i = 0; i < objectManager.GetPairsCount(); i++)
        {
            if (i % 2 != 0)
                objectManager.primitives.at(i).SetVelocity({0, 0, 0});
        }
    }

    if (IsKeyPressed(KEY_D)) {
        Vector3 newVelocity = {-1.0f, 0.0f, 0.0f};
        for (int i = 0; i < objectManager.GetPairsCount(); i++)
        {
            if (i % 2 != 0)
                objectManager.primitives.at(i).SetVelocity(newVelocity);
        }
    }
    if (IsKeyReleased(KEY_D)) {
        for (int i = 0; i < objectManager.GetPairsCount(); i++)
        {
            if (i % 2 != 0)
                objectManager.primitives.at(i).SetVelocity({0, 0, 0});
        }

    }
    objectManager.Update(GetFrameTime());

}

void SceneManager::Draw() {
    DrawSceneCube();
    objectManager.Draw();
}