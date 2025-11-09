//
// Created by David on 24/10/2025.
//

#include "SceneManager.h"
#include <iostream>

SceneManager::SceneManager() : objectManager(boxPosition, boxSize) {
}


void SceneManager::DrawSceneCube() const {
    DrawCubeWires(boxPosition, boxSize, boxSize, boxSize, RED);
}

int SceneManager::GetObjectCount() {
    return objectManager.GetObjectCount();
}

void SceneManager::Update() {
    if (IsKeyReleased(KEY_O))
        objectManager.SpawnRandomObject();

    functioningMode.Update();
    VerifyModeModif();

    const bool debug = functioningMode.GetMode() == FMode::DEBUG_MODE;
    objectManager.Update(GetFrameTime(), debug);
}

void SceneManager::Draw() {
    DrawSceneCube();
    objectManager.Draw();
}

UIData SceneManager::GetUIData() const {
    return UIData{
        objectManager.GetObjectCount(),
        functioningMode.GetMode(),
        functioningMode.GetGenMode()
    };
}

void SceneManager::VerifyModeModif() {
    AllModes modes = functioningMode.GetAllModes();

    if (modes.mode != oldMode.mode || modes.genMode != oldMode.genMode) {
        if (modes.mode == FMode::GEN_MODE) {
            GenerateObjects(modes.genMode);
        }
        else if (modes.mode == FMode::USER_MODE){
            objectManager.Clear();
            std::cout << "Switched to user mode" << std::endl;
        }
        else if (modes.mode == FMode::DEBUG_MODE){
            GenerateDebugObjects();
            std::cout << "Switched to debug mode" << std::endl;
        }
        oldMode = modes;
    }
}

void SceneManager::GenerateDebugObjects() {
    objectManager.Clear();
    objectManager.SpawnPrimitives();
}

void SceneManager::GenerateObjects(GenMode genMode) {
    objectManager.Clear();

    int idMode = -1;
    switch (genMode) {
        case GenMode::GEN1:
            idMode = 0;
            break;
        case GenMode::GEN2:
            idMode = 1;
            break;
        case GenMode::GEN3:
            idMode = 2;
    }

    for (int i = 0; i < genObjects[idMode][0]; i++) {
        objectManager.SpawnObject(ObjectType::SPHERE);
    }

    for (int i = 0; i < genObjects[idMode][1]; i++) {
        objectManager.SpawnObject(ObjectType::CUBE);
    }

    for (int i = 0; i < genObjects[idMode][2]; i++) {
        objectManager.SpawnObject(ObjectType::CYLINDER);
    }

}