//
// Created by David on 26/10/2025.
//

#ifndef INC_3DVIRTUALWORLDPHYSICS_OBJECTMANAGER_H
#define INC_3DVIRTUALWORLDPHYSICS_OBJECTMANAGER_H

#include <vector>
#include "PhysicsObject.h"
#include "CollisionDetector.h"
#define MAX_SPEED 10

enum class LocalVerticesType
{
    SPHERE,
    CUBE,
    CYLINDER
};

class ObjectManager {
private:
    float boxSize;
    Vector3 boxCenter;
    CollisionDetector collisionDetector;


    Vector3 GetRandomPositionInBox() const;
    Color GetRandomColor() const;
    ObjectType GetRandomObjectType() const;
    std::vector<Vector3> cubeVertices;
    std::vector<Vector3> sphereVertices;
    std::vector<Vector3> cylinderVertices;


    void CreateCylinderVertices();
    void CreateSphereVertices();
    void CreateCubeVertices();

public:
    int physics_time;
    bool firstMeasurementDone = false;
    std::vector<PhysicsObject> objects;
    ObjectManager(Vector3 boxCenter, float boxSize);
    Vector3 GetRandomVelocity() const;
    void SpawnRandomObject();
    void SpawnObject(ObjectType objectType);
    void Update(float deltaTime, bool debug);
    void HandleMoveDebugObjects();
    void Draw();
    void Clear();
    void SpawnPrimitives();
    size_t GetObjectCount() const { return objects.size(); }
    void CheckCollisions();
    void ResetAllCollisions();
    void setLocalVertices(PhysicsObject &object) const;

    // std::vector <Vector3> getLocalVertices(ObjectType objectType);

};


#endif //INC_3DVIRTUALWORLDPHYSICS_OBJECTMANAGER_H