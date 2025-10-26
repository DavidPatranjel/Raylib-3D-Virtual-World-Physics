//
// Created by David on 24/10/2025.
//

#include "SceneManager.h"

SceneManager::SceneManager() : objectManager(boxPosition, boxSize) {
}


void SceneManager::DrawSceneCube() const {
    DrawCubeWires(boxPosition, boxSize, boxSize, boxSize, RED);
}

int SceneManager::GetObjectCount() {
    return objectManager.GetObjectCount();
}

void SceneManager::Update() {
    if (IsKeyPressed(KEY_O)) {
        objectManager.SpawnRandomObject();
    }
    objectManager.Update(GetFrameTime());
}

void SceneManager::Draw() {
    DrawSceneCube();
    objectManager.Draw();
}