//
// Created by David on 26/10/2025.
//

#include "PhysicsObject.h"

#include <iostream>
#include <ostream>

#include "raymath.h"

ObjectType PhysicsObject::GetType() const { return type; }

Vector3 PhysicsObject::GetPosition() const { return position; }

Vector3 PhysicsObject::GetVelocity() const { return velocity; }

float PhysicsObject::GetSize() const { return size; }

float PhysicsObject::GetRadius() const { return this -> radius; }

std::vector<Vector3> PhysicsObject::GetLocalVertices() { return this->localVertices; }

Matrix PhysicsObject::GetTransform() const { return this->transform; }

Matrix PhysicsObject::GetRotationMatrix() const { return this->rotationMatrix; }

void PhysicsObject::SetColor(const Color inputColor) { this->color = inputColor; }

void PhysicsObject::SetVelocity(const Vector3 inputVelocity) { this->velocity = inputVelocity; }

void PhysicsObject::SetPosition(const Vector3 inputPosition) { this->position = inputPosition; }

void PhysicsObject::SetIsColliding(const bool inputIsColliding) { this->isColliding = inputIsColliding; }

void PhysicsObject::SetLocalVertices(const std::vector<Vector3> &inputLocalVertices)
{
    localVertices.clear();
    this->localVertices = inputLocalVertices;
}

void PhysicsObject::SetRandomRotationAxis()
{
    rotationAxis.x = static_cast<float>(GetRandomValue(-1.0f, 1.0f));
    rotationAxis.y = static_cast<float>(GetRandomValue(-1.0f, 1.0f));
    rotationAxis.z = static_cast<float>(GetRandomValue(-1.0f, 1.0f));
}

PhysicsObject::PhysicsObject(ObjectType type)
    : type(type), position({0.0f, 0.0f, 0.0f}), velocity({0.0f, 0.0f, 0.0f})
{
    SetRandomRotationAxis();
    transform = MatrixIdentity();
    isColliding = false;
    size = 0.1;
    radius = 0.1;

    if (type == ObjectType::SPHERE)
        model = LoadModelFromMesh(GenMeshSphere(radius, 16, 16));
}

PhysicsObject::PhysicsObject(ObjectType type, Vector3 position, Vector3 velocity, Color color):
             type(type), position(position), velocity(velocity), color(color)
{
    SetRandomRotationAxis();
    transform = MatrixIdentity();
    isColliding = false;
    size = 0.1;
    radius = 0.1;

    if (type == ObjectType::SPHERE)
        model = LoadModelFromMesh(GenMeshSphere(radius, 16, 16));
}

void PhysicsObject::Update(const float deltaTime)
{
    position = Vector3Add(position, Vector3Scale(velocity, deltaTime));
    HandlePhysics(deltaTime);
}

void PhysicsObject::HandlePhysics(const float deltaTime) {
    /// gravity
     float limits = SPACE_LIMITS - 0.1;
    float limit_offet = 0.001f;
    if (Vector2Length(Vector2{ velocity.x, velocity.z }) < 0.001f && position.y < -limits) {
        position.y = -limits + limit_offet;
        velocity.y = 0.0f;
        return;
    }
    velocity.y -= GRAVITY * deltaTime;

    /// friction
    velocity.x *= FRICTION_INV;
    velocity.y *= FRICTION_INV;
    velocity.z *= FRICTION_INV;


    if (position.x > limits) {
        isColliding = true;
        position.x = limits - limit_offet;
    }
    else if (position.y > limits) {
        position.y =  limits - limit_offet;
        isColliding = true;
    }
    else if (position.z > limits) {
        position.z = limits - limit_offet;
        isColliding = true;
    }
    else if (position.x < -limits) {
        position.x = -limits + limit_offet;
        isColliding = true;
    }
    else if (position.y < -limits) {
        position.y = -limits + limit_offet;
        isColliding = true;
    }
    else if (position.z < -limits) {
        position.z = -limits + limit_offet;
        isColliding = true;
    }

    if (isColliding) {
        isColliding = false;
        velocity.x = -velocity.x * ENERGY_LOSS;
        velocity.y = -velocity.y * ENERGY_LOSS;
        velocity.z = -velocity.z * ENERGY_LOSS;
    }
}

void PhysicsObject::Rotate()
{
    const Matrix rotStep = MatrixRotate(Vector3Normalize(rotationAxis), rotationSpeed);
    rotationMatrix = MatrixMultiply(rotStep, rotationMatrix);
}


void PhysicsObject::Draw()
{
    if (this -> GetType() == ObjectType::SPHERE)
    {
        DrawSphere();
        return;
    }
    Rotate();
    transform = MatrixMultiply(this->rotationMatrix,
                               MatrixTranslate(this->position.x, this->position.y, this->position.z));

    std::vector<Vector3> worldVertices(this->localVertices.size());
    for (size_t i = 0; i < this->localVertices.size(); i++)
        worldVertices[i] = Vector3Transform(this->localVertices[i], transform);

    const size_t verticesCount = this->localVertices.size();

    if (verticesCount < 4)
        return;

    const size_t half = verticesCount / 2;

    if (isColliding)
        color = RED;
    else
        color = GREEN;

    for (int i = 0; i < half; i++)
            DrawLine3D(worldVertices[i], worldVertices[i + half], this->color);

    for (int i = 0; i < half; i++)
    {
        const size_t next = (i + 1) % half;
        DrawLine3D(worldVertices[i], worldVertices[next], this->color);
        DrawLine3D(worldVertices[i + half], worldVertices[next + half], this->color);
    }
}

void PhysicsObject::DrawSphere()
{
    Rotate();

    transform = MatrixMultiply(this->rotationMatrix,
                               MatrixTranslate(this->position.x, this->position.y, this->position.z));

    const int rings = static_cast <int> (sqrt(this->localVertices.size()));
    const int slices = rings;
    if (isColliding)
        color = RED;
    else
        color = GREEN;

    // Draw rings (latitude lines)
    for (int i = 0; i < rings; i++)
    {
        for (int j = 0; j < slices; j++)
        {
            const int index1 = i * (slices + 1) + j;
            int index2 = index1 + 1;
            if (index2 < this->localVertices.size())
            {
                DrawLine3D(Vector3Transform(this->localVertices[index1], transform),
                           Vector3Transform(this->localVertices[index2], transform),
                           this->color);
            }
        }
    }

}
