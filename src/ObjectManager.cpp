//
// Created by David on 26/10/2025.
//

#include "ObjectManager.h"


ObjectManager::ObjectManager(Vector3 boxCenter, float boxSize)
    : boxCenter(boxCenter), boxSize(boxSize) {
}

void ObjectManager::SpawnRandomObject() {
    ObjectType type = GetRandomObjectType();
    SpawnObject(type);
}

void ObjectManager::SpawnObject(ObjectType objectType) {
    ObjectType type = objectType;
    Vector3 position = GetRandomPositionInBox();
    Vector3 velocity = GetRandomVelocity();
    Color color = GetRandomColor();

    objects.emplace_back(type, position, velocity, color);
}

void ObjectManager::Update(float deltaTime) {
    for (auto& obj : objects) {
        obj.Update(deltaTime);
    }

    if (IsKeyPressed(KEY_R)) {
        Clear();
    }
}

void ObjectManager::Draw() const {
    for (const auto& obj : objects) {
        obj.Draw();
    }
}

void ObjectManager::Clear() {
    objects.clear();
}

Vector3 ObjectManager::GetRandomPositionInBox() const {
    float halfSize = boxSize / 2.0f;
    float margin = 0.2f; //distance from the cube margin

    return {
        boxCenter.x + static_cast<float>(GetRandomValue(-100, 100)) / 100.0f * (halfSize - margin),
        boxCenter.y + static_cast<float>(GetRandomValue(-100, 100)) / 100.0f * (halfSize - margin),
        boxCenter.z + static_cast<float>(GetRandomValue(-100, 100)) / 100.0f * (halfSize - margin)
    };
}

Vector3 ObjectManager::GetRandomVelocity() const {
    // Viteze aleatorii pt fiecare axă
    return {
        static_cast<float>(GetRandomValue(-200, 200)) / 100.0f,
        static_cast<float>(GetRandomValue(-200, 200)) / 100.0f,
        static_cast<float>(GetRandomValue(-200, 200)) / 100.0f
    };
}

Color ObjectManager::GetRandomColor() const {
    Color colors[] = {RED, GREEN, BLUE, YELLOW, PURPLE, ORANGE, PINK, LIME, GOLD, SKYBLUE};
    return colors[GetRandomValue(0, 9)];
}

ObjectType ObjectManager::GetRandomObjectType() const {
    int type = GetRandomValue(0, 2);
    return static_cast<ObjectType>(type);
}