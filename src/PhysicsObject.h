//
// Created by David on 26/10/2025.
//

#ifndef INC_3DVIRTUALWORLDPHYSICS_PHYSICSOBJECT_H
#define INC_3DVIRTUALWORLDPHYSICS_PHYSICSOBJECT_H
#define GRAVITY 3.0f
#define FRICTION_INV 0.97f
#define SPACE_LIMITS 3
#define ENERGY_LOSS 0.85f

#include "raylib.h"
#include "raymath.h"
#include <vector>

enum class ObjectType
{
    SPHERE,
    CUBE,
    CYLINDER
};

class PhysicsObject
{
private:
    ObjectType type;
    Model model;
    std::vector<Vector3> localVertices;
    Vector3 position = {0, 0, 0};
    Vector3 velocity;
    Vector3 rotationAxis;
    Vector3 center;

    bool isColliding;
    float rotationSpeed = 0.01;
    float size;
    float radius;
    Color color;

    Matrix rotationMatrix = MatrixIdentity();
    Matrix transform;

public:
    explicit PhysicsObject(ObjectType type);

    PhysicsObject(ObjectType type, Vector3 position, Vector3 velocity, Color color);

    [[nodiscard]] ObjectType GetType() const;

    [[nodiscard]] Vector3 GetPosition() const;

    [[nodiscard]] Vector3 GetVelocity() const;

    [[nodiscard]] float GetSize() const;

    [[nodiscard]] float GetRadius() const;


    [[nodiscard]] std::vector<Vector3> GetLocalVertices();

    [[nodiscard]] Matrix GetTransform() const;

    [[nodiscard]] Matrix GetRotationMatrix() const;


    void SetColor(Color inputColor);

    void SetVelocity(Vector3 inputVelocity);

    void SetPosition(Vector3 inputPosition);

    void SetIsColliding(bool inputIsColliding);

    void SetLocalVertices(const std::vector<Vector3> &inputLocalVertices);

    void SetRandomRotationAxis();

    void Update(float deltaTime);

    void HandlePhysics(const float deltaTime);

    void Draw();

    void DrawSphere();

    void Rotate();
};

#endif //INC_3DVIRTUALWORLDPHYSICS_PHYSICSOBJECT_H


