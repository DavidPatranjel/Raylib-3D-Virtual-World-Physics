//
// Created by mihae on 11/3/2025.
//

#ifndef INC_3DVIRTUALWORLDPHYSICS_COLLISIONDETECTOR_H
#define INC_3DVIRTUALWORLDPHYSICS_COLLISIONDETECTOR_H
#include <functional>
#include <map>
#include <ostream>
#include "PhysicsObject.h"
#include "raymath.h"


class CollisionDetector
{
    void ProjectOntoAxis(const std::vector <Vector3> &vertices, int count, Vector3 axis, float *min, float *max);

    bool TryCross(Vector3 a, Vector3 b, Vector3 *out);

    static std::vector<Vector3> GetFaceNormals(const std::vector<Vector3> &vertices);

    bool CheckSphereSphere(const PhysicsObject &a, const PhysicsObject &b);

    bool CheckSphereConvex(PhysicsObject& sphere, PhysicsObject& convex);
private:
    bool CheckSphereCube(PhysicsObject &sphere, const std::vector<Vector3> &cubeVertices);
    bool CheckSphereCylinder(PhysicsObject &sphere, const std::vector<Vector3> &cylinderVertices);
    float ClosestPointOnEdge(const Vector3 &point, const Vector3 &edgeStart, const Vector3 &edgeEnd);

public:
    bool CheckConvexCollision(PhysicsObject a, PhysicsObject b);
};


#endif //INC_3DVIRTUALWORLDPHYSICS_COLLISIONDETECTOR_H
