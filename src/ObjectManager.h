//
// Created by David on 26/10/2025.
//

#ifndef INC_3DVIRTUALWORLDPHYSICS_OBJECTMANAGER_H
#define INC_3DVIRTUALWORLDPHYSICS_OBJECTMANAGER_H

#include <vector>
#include "PhysicsObject.h"
#include "CollisionDetector.h"

class ObjectManager
{
private:
    float boxSize;
    Vector3 boxCenter;
    std::vector<PhysicsObject> objects;
    CollisionDetector collisionDetector;

    [[nodiscard]] Vector3 GetRandomPositionInBox() const;

    [[nodiscard]] static Color GetRandomColor();

    [[nodiscard]] static ObjectType GetRandomObjectType();

    static Vector3 GetRandomVelocity() ;

    std::vector<Vector3> cubeVertices;
    std::vector<Vector3> sphereVertices;
    std::vector<Vector3> cylinderVertices;

public:
    std::vector<PhysicsObject> primitives;

    ObjectManager(Vector3 boxCenter, float boxSize);

    [[nodiscard]] size_t GetObjectCount() const { return objects.size(); }

    [[nodiscard]] size_t GetPairsCount() const { return primitives.size(); }

    void Update(float deltaTime);

    void Draw();

    void Clear();

    void SpawnRandomObject();

    void SpawnPrimitives();

    void CheckCollisions();

    void ResetAllCollisions();

    void setLocalVertices(PhysicsObject &object) const;
};


#endif //INC_3DVIRTUALWORLDPHYSICS_OBJECTMANAGER_H
