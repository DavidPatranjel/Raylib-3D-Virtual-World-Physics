//
// Created by David on 24/10/2025.
//

#ifndef INC_3DVIRTUALWORLDPHYSICS_SCENE_H
#define INC_3DVIRTUALWORLDPHYSICS_SCENE_H

#include "ObjectManager.h"
#include "raylib.h"

class SceneManager {
private:
    static constexpr Vector3 boxPosition = { 0.0f, 0.0f, 0.0f };
    static constexpr float boxSize = 5.0f;
    ObjectManager objectManager;

    void DrawSceneCube() const;

public:
    SceneManager();
    int GetObjectCount();
    void Update();
    void Draw();
};


#endif //INC_3DVIRTUALWORLDPHYSICS_SCENE_H