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

    if (IsKeyReleased(KEY_P))
        objectManager.SpawnPrimitives();

    if (IsKeyReleased(KEY_O))
        objectManager.SpawnRandomObject();

    if (IsKeyPressed(KEY_A)) {
        Vector3 newVelocity = {1.0f, 0.0f, 0.0f};
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

    functioningMode.Update();
    VerifyModeModif();
    objectManager.Update(GetFrameTime());
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
        else {
            objectManager.Clear();
            std::cout << "Switched to user mode" << std::endl;
        }
        oldMode = modes;
    }
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