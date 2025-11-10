//
// Created by David on 24/10/2025.
//

#ifndef INC_3DVIRTUALWORLDPHYSICS_SCENE_H
#define INC_3DVIRTUALWORLDPHYSICS_SCENE_H

#include "FunctioningMode.h"
#include "ObjectManager.h"
#include "raylib.h"

struct UIData {
    size_t objectCount;
    FMode mode;
    GenMode genMode;
};

class SceneManager {
private:
    std::vector<std::vector<int>> genObjects = {
        {10, 25, 50},   // gen 1
        {100, 250, 500},   // gen 1
        {250, 500, 1000},  // gen 2
        {500, 1000, 2500}  // gen 3
    };

    static constexpr Vector3 boxPosition = { 0.0f, 0.0f, 0.0f };
    static constexpr float boxSize = 6.0f;
    ObjectManager objectManager;
    FunctioningMode functioningMode;
    AllModes oldMode = {FMode::USER_MODE, GenMode::GEN1};

    void DrawSceneCube() const;

public:
    SceneManager();
    int GetObjectCount();
    UIData GetUIData() const;
    void Update();
    void Draw();
    void VerifyModeModif();
    void GenerateDebugObjects();
    void GenerateObjects(GenMode genMode);
};


#endif //INC_3DVIRTUALWORLDPHYSICS_SCENE_H
