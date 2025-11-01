//
// Created by David on 26/10/2025.
//

#ifndef INC_3DVIRTUALWORLDPHYSICS_OBJECTMANAGER_H
#define INC_3DVIRTUALWORLDPHYSICS_OBJECTMANAGER_H



#include <vector>
#include "PhysicsObject.h"

class ObjectManager {
private:
    Vector3 boxCenter;
    float boxSize;
    std::vector<PhysicsObject> objects;

    Vector3 GetRandomPositionInBox() const;
    Vector3 GetRandomVelocity() const;
    Color GetRandomColor() const;
    ObjectType GetRandomObjectType() const;

public:
    ObjectManager(Vector3 boxCenter, float boxSize);

    void SpawnRandomObject();
    void SpawnObject(ObjectType objectType);
    void Update(float deltaTime);
    void Draw() const;
    void Clear();

    int GetObjectCount() const { return objects.size(); }

};


#endif //INC_3DVIRTUALWORLDPHYSICS_OBJECTMANAGER_H