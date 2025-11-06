//
// Created by David on 26/10/2025.
//

#include "ObjectManager.h"

#define CYLINDER_SIDES 16
#define CYLINDER_RADIUS 1.0
#define CYLINDER_HEIGHT 2.0
#define CIRCLE_RINGS 16.0

ObjectManager::ObjectManager(const Vector3 boxCenter, const float boxSize)
    : boxSize(boxSize), boxCenter(boxCenter)
{
    cubeVertices = {
        {-0.5f, -0.5f, -0.5f},
        {0.5f, -0.5f, -0.5f},
        {0.5f, 0.5f, -0.5f},
        {-0.5f, 0.5f, -0.5f},
        {-0.5f, -0.5f, 0.5f},
        {0.5f, -0.5f, 0.5f},
        {0.5f, 0.5f, 0.5f},
        {-0.5f, 0.5f, 0.5f}
    };

    for (int i = 0; i < CYLINDER_SIDES; i++)
    {
        const float a = (2.0f * PI / CYLINDER_SIDES) * static_cast<float> (i);
        const auto x = static_cast<float>(cosf(a) * CYLINDER_RADIUS);

        const auto z = static_cast<float>(sinf(a) * CYLINDER_RADIUS);

        cylinderVertices.push_back({x, -CYLINDER_HEIGHT / 2, z});
        cylinderVertices.push_back({x, CYLINDER_HEIGHT / 2, z});
    }

    for (int i = 0; i <= CIRCLE_RINGS; i++)
    {
        const auto phi = static_cast<float>((PI *  static_cast<float>(i)) / CIRCLE_RINGS); // latitude angle
        const float y = cosf(phi) * 1;
        const float r = sinf(phi) * 1; // ring radius

        for (int j = 0; j <= CIRCLE_RINGS; j++)
        {
            const double theta = 2.0f * PI * static_cast<float>(j) / CIRCLE_RINGS; // longitude angle
            const float x = r * cosf(static_cast<float>(theta));
            const float z = r * sinf(static_cast<float>(theta));

            sphereVertices.push_back({ x, y, z });
        }
    }
}

void ObjectManager::SpawnRandomObject()
{
    ObjectType type = GetRandomObjectType();
    SpawnObject(type);
}

void ObjectManager::SpawnObject(ObjectType objectType) {
    ObjectType type = objectType;
    Vector3 position = GetRandomPositionInBox();
    Vector3 velocity = GetRandomVelocity();
    Color color = GetRandomColor();


    objects.emplace_back(type, position, velocity, color);
    setLocalVertices(objects.back());
}


void ObjectManager::SpawnPrimitives()
{
    primitives.emplace_back(ObjectType::CUBE);
    primitives.emplace_back(ObjectType::CUBE);

    primitives.emplace_back(ObjectType::CYLINDER);
    primitives.emplace_back(ObjectType::CYLINDER);

    primitives.emplace_back(ObjectType::CUBE);
    primitives.emplace_back(ObjectType::CYLINDER);

    primitives.emplace_back(ObjectType::SPHERE);
    primitives.emplace_back(ObjectType::SPHERE);

    primitives.emplace_back(ObjectType::SPHERE);
    primitives.emplace_back(ObjectType::CUBE);

    primitives.emplace_back(ObjectType::SPHERE);
    primitives.emplace_back(ObjectType::CYLINDER);

    int i = 0;
    int j = 0;

    for (auto & primitive : primitives)
    {
        setLocalVertices(primitive);

        primitive.SetColor(GREEN);
        primitive.SetPosition({static_cast<float>(j % 2) * 2.0f, static_cast<float>(-i) * 4.0f, 0.0f});
        j++;
        if (j % 2 == 0)
            i++;
    }
}


void ObjectManager::setLocalVertices(PhysicsObject &object) const
{
    if (object.GetType() == ObjectType::CUBE)
        object.SetLocalVertices(cubeVertices);
    if (object.GetType() == ObjectType::CYLINDER)
        object.SetLocalVertices(cylinderVertices);
    if (object.GetType() == ObjectType::SPHERE)
        object.SetLocalVertices(sphereVertices);
    if (object.GetType() == ObjectType::SPHERE)
        object.SetLocalVertices(sphereVertices);
}

void ObjectManager::Update(float deltaTime)
{
    ResetAllCollisions();

    for (auto& obj : objects)
        obj.Update(deltaTime);

    for (auto &obj: primitives)
         obj.Update(deltaTime);

    if (IsKeyPressed(KEY_R))
        Clear();

    CheckCollisions();
}

void ObjectManager::Draw()
{
    for (auto& obj : objects)
        obj.Draw();

    for (auto &obj: primitives)
        obj.Draw();
}

void ObjectManager::Clear()
{
    objects.clear();
}

Vector3 ObjectManager::GetRandomPositionInBox() const
{
    const float halfSize = boxSize / 2.0f;
    const float margin = 0.2f; //distance from the cube margin

    return
    {
        boxCenter.x + static_cast<float>(GetRandomValue(-100, 100)) / 100.0f * (halfSize - margin),
        boxCenter.y + static_cast<float>(GetRandomValue(-100, 100)) / 100.0f * (halfSize - margin),
        boxCenter.z + static_cast<float>(GetRandomValue(-100, 100)) / 100.0f * (halfSize - margin)
    };
}

Vector3 ObjectManager::GetRandomVelocity() const
{
    // Viteze aleatorii pt fiecare axă//
    return
    {
        static_cast<float>(GetRandomValue(-200, 200)) / 100.0f,
        static_cast<float>(GetRandomValue(-200, 200)) / 100.0f,
        static_cast<float>(GetRandomValue(-200, 200)) / 100.0f
    };
}

Color ObjectManager::GetRandomColor() const
{
    constexpr Color colors[] = {RED, GREEN, BLUE, YELLOW, PURPLE, ORANGE, PINK, LIME, GOLD, SKYBLUE};
    return colors[GetRandomValue(0, 9)];
}

ObjectType ObjectManager::GetRandomObjectType() const
{
    int type = GetRandomValue(0, 2);
    return static_cast<ObjectType>(type);
}

void ObjectManager::CheckCollisions()
{
    if (primitives.empty())
        return;

    for (int i = 0; i < primitives.size() - 1; i++)
    {
        for (int j = i + 1; j < primitives.size(); j++)
        {
            if (collisionDetector.CheckConvexCollision(primitives.at(i), primitives.at(j)))
            {
                primitives.at(i).SetIsColliding(true);
                primitives.at(j).SetIsColliding(true);
            }
        }
    }
}

void ObjectManager::ResetAllCollisions()
{
    for (auto &object: primitives)
        object.SetIsColliding(false);
}
