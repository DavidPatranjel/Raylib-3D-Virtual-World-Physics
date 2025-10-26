//
// Created by David on 26/10/2025.
//

#include "PhysicsObject.h"


PhysicsObject::PhysicsObject(ObjectType type, Vector3 position, Vector3 velocity, Color color)
    : type(type), position(position), velocity(velocity), color(color) {

}

void PhysicsObject::Update(float deltaTime) {
    position.x += velocity.x * deltaTime;
    position.y += velocity.y * deltaTime;
    position.z += velocity.z * deltaTime;
}

void PhysicsObject::Draw() const {
    switch (type) {
        case ObjectType::SPHERE:
            DrawSphere(position, size, color);
            break;
        case ObjectType::CUBE:
            DrawCube(position, size, size, size, color);
            break;
        case ObjectType::CYLINDER:
            DrawCylinder(position, size, size, size * 2.0f, 16, color);
            break;
    }
}