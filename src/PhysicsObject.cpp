//
// Created by David on 26/10/2025.
//

#include "PhysicsObject.h"

#include <iostream>
#include <ostream>

#include "raymath.h"
#define CYLINDER_SIDES 4
#define CYLINDER_RADIUS 0.1
#define CYLINDER_HEIGHT 0.2

ObjectType PhysicsObject::GetType() const { return type; }

Vector3 PhysicsObject::GetPosition() const { return position; }

Vector3 PhysicsObject::GetVelocity() const { return velocity; }

float PhysicsObject::GetSize() const { return size; }

float PhysicsObject::GetRadius() const { return this->radius; }

std::vector<Vector3> PhysicsObject::GetLocalVertices() { return this->localVertices; }

Matrix PhysicsObject::GetTransform() const { return this->transform; }

Matrix PhysicsObject::GetRotationMatrix() const { return this->rotationMatrix; }

void PhysicsObject::SetColor(const Color inputColor) { this->color = inputColor; }

void PhysicsObject::SetVelocity(const Vector3 inputVelocity) { this->velocity = inputVelocity; }

void PhysicsObject::SetPosition(const Vector3 inputPosition) { this->position = inputPosition; }

void PhysicsObject::SetIsColliding(const bool inputIsColliding) { this->isColliding = inputIsColliding; }


void PhysicsObject::SetLocalVertices(const std::vector<Vector3> inputLocalVertices)
{
    localVertices.clear();
    this->localVertices = inputLocalVertices;

    if (this->type == ObjectType::CYLINDER) { CreateCylinderMesh(); }

    float maxDistSq = 0.0f;
    for (const auto &v: localVertices)
    {
        float distSq = v.x * v.x + v.y * v.y + v.z * v.z;
        if (distSq > maxDistSq)
            maxDistSq = distSq;
    }
    this->radius = sqrtf(maxDistSq);
    if (this->radius == 0) this->radius = 0.1f;
}


void PhysicsObject::SetRandomRotationAxis()
{
    do
    {
        rotationAxis.x = static_cast<float>(GetRandomValue(-1.0f, 1.0f));
        rotationAxis.y = static_cast<float>(GetRandomValue(-1.0f, 1.0f));
        rotationAxis.z = static_cast<float>(GetRandomValue(-1.0f, 1.0f));
    }
    while (rotationAxis.x == 0.0f || rotationAxis.y == 0.0f || rotationAxis.z == 0.0f);
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
    if (type == ObjectType::CYLINDER) { CreateCylinderMesh(); }
}

PhysicsObject::PhysicsObject(ObjectType type, Vector3 position, Vector3 velocity, Color color) : type(type),
    position(position), velocity(velocity), color(color)
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

    if (type == ObjectType::CYLINDER) { CreateCylinderMesh(); }
}

void PhysicsObject::Update(const float deltaTime)
{
    position = Vector3Add(position, Vector3Scale(velocity, deltaTime));

    Rotate();

    Matrix matTranslation = MatrixTranslate(position.x, position.y, position.z);
    transform = MatrixMultiply(rotationMatrix, matTranslation);

    HandlePhysics(deltaTime);
}


void PhysicsObject::Rotate()
{
    const Matrix rotStep = MatrixRotate(Vector3Normalize(rotationAxis), rotationSpeed);
    rotationMatrix = MatrixMultiply(rotStep, rotationMatrix);
    totalRotationAngle += rotationSpeed;
}

void PhysicsObject::Draw()
{
    Color renderColor = isColliding ? RED : this->color;

    model.transform = this->transform;

    DrawModel(model, {0.0f, 0.0f, 0.0f}, 1.0f, renderColor);
}


void PhysicsObject::CreateCylinderMesh()
{
    const int SIDES = 6;
    const float RADIUS = CYLINDER_RADIUS;
    const float HEIGHT = CYLINDER_HEIGHT;

    this->localVertices.clear();
    std::vector<Vector3> normals;

    for (int i = 0; i < SIDES; i++)
    {
        const float angle = 2.0f * PI * static_cast<float>(i) / SIDES;
        const float x = cosf(angle) * RADIUS;
        const float z = sinf(angle) * RADIUS;

        // Bottom vertex
        this->localVertices.push_back({x, -HEIGHT / 2.0f, z});
        // Top vertex
        this->localVertices.push_back({x, HEIGHT / 2.0f, z});

        // Normals for the sides
        Vector3 sideNormal = Vector3Normalize({x, 0.0f, z});
        normals.push_back(sideNormal);
        normals.push_back(sideNormal);
    }

    const int bottomCenterIndex = this->localVertices.size();
    this->localVertices.push_back({0.0f, -HEIGHT / 2.0f, 0.0f});
    normals.push_back({0.0f, -1.0f, 0.0f});

    const int topCenterIndex = this->localVertices.size();
    this->localVertices.push_back({0.0f, HEIGHT / 2.0f, 0.0f});
    normals.push_back({0.0f, 1.0f, 0.0f});


    std::vector<unsigned short> indices;
    for (int i = 0; i < SIDES; i++)
    {
        int currentBottom = i * 2;
        int currentTop = i * 2 + 1;
        int nextBottom = ((i + 1) % SIDES) * 2;
        int nextTop = ((i + 1) % SIDES) * 2 + 1;

        indices.push_back(currentBottom);
        indices.push_back(nextBottom);
        indices.push_back(currentTop);

        indices.push_back(currentTop);
        indices.push_back(nextBottom);
        indices.push_back(nextTop);

        indices.push_back(bottomCenterIndex);
        indices.push_back(nextBottom);
        indices.push_back(currentBottom);

        indices.push_back(topCenterIndex);
        indices.push_back(currentTop);
        indices.push_back(nextTop);
    }

    Mesh mesh = {0};
    mesh.vertexCount = this->localVertices.size();
    mesh.triangleCount = indices.size() / 3;

    mesh.vertices = (float *) MemAlloc(mesh.vertexCount * 3 * sizeof(float));
    mesh.normals = (float *) MemAlloc(mesh.vertexCount * 3 * sizeof(float));
    mesh.indices = (unsigned short *) MemAlloc(mesh.triangleCount * 3 * sizeof(unsigned short));

    int vertexIndex = 0;
    for (const auto &v: this->localVertices)
    {
        mesh.vertices[vertexIndex++] = v.x;
        mesh.vertices[vertexIndex++] = v.y;
        mesh.vertices[vertexIndex++] = v.z;
    }

    // Copy normals
    int normalIndex = 0;
    for (const auto &n: normals)
    {
        mesh.normals[normalIndex++] = n.x;
        mesh.normals[normalIndex++] = n.y;
        mesh.normals[normalIndex++] = n.z;
    }

    // Copy indices
    memcpy(mesh.indices, indices.data(), indices.size() * sizeof(unsigned short));

    UploadMesh(&mesh, false);
    this->model = LoadModelFromMesh(mesh);
    this->color = PURPLE;
}

void PhysicsObject::HandlePhysics(const float deltaTime)
{
    /// gravity
    float limits = SPACE_LIMITS - 0.1;
    float limit_offet = 0.001f;
    if (Vector2Length(Vector2{velocity.x, velocity.z}) < 0.001f && position.y < -limits)
    {
        position.y = -limits + limit_offet;
        velocity.y = 0.0f;
        return;
    }
    velocity.y -= GRAVITY * deltaTime;

    /// friction
    velocity.x *= FRICTION_INV;
    velocity.y *= FRICTION_INV;
    velocity.z *= FRICTION_INV;


    if (position.x > limits)
    {
        isColliding = true;
        position.x = limits - limit_offet;
    } else if (position.y > limits)
    {
        position.y = limits - limit_offet;
        isColliding = true;
    } else if (position.z > limits)
    {
        position.z = limits - limit_offet;
        isColliding = true;
    } else if (position.x < -limits)
    {
        position.x = -limits + limit_offet;
        isColliding = true;
    } else if (position.y < -limits)
    {
        position.y = -limits + limit_offet;
        isColliding = true;
    } else if (position.z < -limits)
    {
        position.z = -limits + limit_offet;
        isColliding = true;
    }

    if (isColliding)
    {
        isColliding = false;
        velocity.x = -velocity.x * ENERGY_LOSS;
        velocity.y = -velocity.y * ENERGY_LOSS;
        velocity.z = -velocity.z * ENERGY_LOSS;
    }
}
