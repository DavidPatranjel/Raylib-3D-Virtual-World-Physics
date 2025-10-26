//
// Created by David on 26/10/2025.
//

#ifndef INC_3DVIRTUALWORLDPHYSICS_PHYSICSOBJECT_H
#define INC_3DVIRTUALWORLDPHYSICS_PHYSICSOBJECT_H


#include "raylib.h"

enum class ObjectType {
    SPHERE,
    CUBE,
    CYLINDER
};

class PhysicsObject {
private:
    ObjectType type;
    Vector3 position;
    Vector3 velocity;
    Color color;
    const float size = 0.1;

public:
    PhysicsObject(ObjectType type, Vector3 position, Vector3 velocity, Color color);

    void Update(float deltaTime);
    void Draw() const;

    ObjectType GetType() const { return type; }
    Vector3 GetPosition() const { return position; }
    Vector3 GetVelocity() const { return velocity; }
    float GetSize() const { return size; }

};

#endif //INC_3DVIRTUALWORLDPHYSICS_PHYSICSOBJECT_H