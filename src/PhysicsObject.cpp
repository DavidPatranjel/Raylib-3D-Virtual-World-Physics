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
    // localVertices.clear();
    // this->localVertices = inputLocalVertices;

    localVertices.clear();
    this->localVertices = inputLocalVertices;

    // FIX: Calculate the actual radius based on the furthest vertex
    // This ensures the Fast Check in CollisionDetector is accurate.
    float maxDistSq = 0.0f;
    for (const auto& v : localVertices)
    {
        float distSq = v.x*v.x + v.y*v.y + v.z*v.z;
        if (distSq > maxDistSq)
            maxDistSq = distSq;
    }
    this->radius = sqrtf(maxDistSq);
    // Optional: slight padding to prevent floating point errors
    if (this->radius == 0) this->radius = 0.1f;
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
    {
        model = LoadModelFromMesh(GenMeshSphere(radius, 8, 8));
        color = BLUE;
    }
    if (type == ObjectType::CUBE)
    {
        model = LoadModelFromMesh(GenMeshCube(0.2, 0.2, 0.2));
        color = GREEN;
    }
    else if (type == ObjectType::CYLINDER) {
        // --- APPLY THE SAME FIX HERE ---
        const float cylRadius = 0.1f;
        const float cylHeight = 0.2f;
        Mesh mesh = GenMeshCylinder(cylRadius, cylHeight, 8);

        for (int i = 0; i < mesh.vertexCount; i++) {
            mesh.vertices[i * 3 + 1] -= cylHeight / 2.0f;
        }
        model = LoadModelFromMesh(mesh);
        color = PURPLE;
    }
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
        model = LoadModelFromMesh(GenMeshSphere(radius, 8, 8));
    if (type == ObjectType::CUBE)
        model = LoadModelFromMesh(GenMeshCube(0.2, 0.2, 0.2));
    if (type == ObjectType::CYLINDER)
    {
        const float cylRadius = 0.1f;
        const float cylHeight = 0.2f; // Note: In your constructor you used 0.1, but in ObjectManager it's 0.2. Let's use 0.2 for consistency.

        // 2. Generate the standard (offset) raylib mesh
        Mesh mesh = GenMeshCylinder(cylRadius, cylHeight, 8);

        // 3. Manually translate all its vertices down by height/2
        for (int i = 0; i < mesh.vertexCount; i++) {
            // raylib stores vertex data as a flat float array: [x, y, z, x, y, z, ...]
            // The y-coordinate is at index (i * 3 + 1)
            mesh.vertices[i * 3 + 1] -= cylHeight / 2.0f;
        }
        model = LoadModelFromMesh(mesh);
    }
}

void PhysicsObject::Update(const float deltaTime)
{
    // position.x += velocity.x * deltaTime;
    // position.y += velocity.y * deltaTime;
    // position.z += velocity.z * deltaTime;

    // Update rotation
    position = Vector3Add(position, Vector3Scale(velocity, deltaTime));

    Rotate();

    // FIX: Update the Transform Matrix!
    // Without this, GetTransform() returns Identity (or old data)
    // and SAT collision detection will be incorrect.
    Matrix matTranslation = MatrixTranslate(position.x, position.y, position.z);
    transform = MatrixMultiply(rotationMatrix, matTranslation);

    // HandlePhysics(deltaTime);
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
    // const Matrix rotStep = MatrixRotate(Vector3Normalize(rotationAxis), rotationSpeed);
    // rotationMatrix = MatrixMultiply(rotStep, rotationMatrix);

    const Matrix rotStep = MatrixRotate(Vector3Normalize(rotationAxis), rotationSpeed);
    rotationMatrix = MatrixMultiply(rotStep, rotationMatrix);
    totalRotationAngle += rotationSpeed;
}


// void PhysicsObject::Draw()
// {
//     if (this -> GetType() == ObjectType::SPHERE)
//     {
//         DrawSphere();
//         return;
//     }
//     Rotate();
//     transform = MatrixMultiply(this->rotationMatrix,
//                                MatrixTranslate(this->position.x, this->position.y, this->position.z));
//
//     std::vector<Vector3> worldVertices(this->localVertices.size());
//     for (size_t i = 0; i < this->localVertices.size(); i++)
//         worldVertices[i] = Vector3Transform(this->localVertices[i], transform);
//
//     const size_t verticesCount = this->localVertices.size();
//
//     if (verticesCount < 4)
//         return;
//
//     const size_t half = verticesCount / 2;
//
//     if (isColliding)
//         color = RED;
//     else
//         color = GREEN;
//
//     for (int i = 0; i < half; i++)
//             DrawLine3D(worldVertices[i], worldVertices[i + half], this->color);
//
//     for (int i = 0; i < half; i++)
//     {
//         const size_t next = (i + 1) % half;
//         DrawLine3D(worldVertices[i], worldVertices[next], this->color);
//         DrawLine3D(worldVertices[i + half], worldVertices[next + half], this->color);
//     }
// }


void PhysicsObject::Draw()
{

    Color renderColor = isColliding ? RED : this->color;

    if (this->GetType() == ObjectType::CYLINDER)
    {
        DrawCylinder(renderColor);
        return;
    }

    DrawModelEx(
        model,
        position,
        Vector3Normalize(rotationAxis),
        totalRotationAngle * RAD2DEG,  // Convert total rotation from radians to degrees
        {1.0f, 1.0f, 1.0f},            // We use a uniform scale of 1, as size is handled by the mesh generation.
        renderColor
    );
}


void PhysicsObject::DrawCylinder(Color renderColor)
{
    // Transform all the local vertices into their world positions
    std::vector<Vector3> worldVertices(this->localVertices.size());
    for (size_t i = 0; i < this->localVertices.size(); i++)
    {
        worldVertices[i] = Vector3Transform(this->localVertices[i], this->transform);
    }

    const size_t verticesCount = this->localVertices.size();
    if (verticesCount < 4) return;

    // The number of vertices on a single cap (e.g., the base)
    const size_t half = verticesCount / 2;

    // Draw the vertical lines connecting the top and bottom caps
    for (size_t i = 0; i < half; i++)
    {
        DrawLine3D(worldVertices[i], worldVertices[i + half], renderColor);
    }

    // Draw the top and bottom cap outlines
    for (size_t i = 0; i < half; i++)
    {
        // Connect vertex 'i' to the next vertex on the same cap
        const size_t next_i = (i + 1) % half;

        // Draw bottom cap edge
        DrawLine3D(worldVertices[i], worldVertices[next_i], renderColor);

        // Draw top cap edge
        DrawLine3D(worldVertices[i + half], worldVertices[next_i + half], renderColor);
    }
}

