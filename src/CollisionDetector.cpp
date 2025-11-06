//
// Created by mihae on 11/3/2025.
//

#include "CollisionDetector.h"

#include <cfloat>

void CollisionDetector::ProjectOntoAxis(const std::vector<Vector3> &vertices, const int count, const Vector3 axis,
                                        float *min, float *max)
{
    *min = *max = Vector3DotProduct(vertices.at(0), axis);
    for (int i = 1; i < count; i++)
    {
        const float d = Vector3DotProduct(vertices.at(i), axis);
        if (d < *min)
            *min = d;
        if (d > *max)
            *max = d;
    }
}

bool CollisionDetector::TryCross(const Vector3 a, const Vector3 b, Vector3 *out)
{
    const Vector3 c = Vector3Subtract(a, b);
    const float len = Vector3Length(c);
    if (len < 1e-6f)
        return false;
    *out = Vector3Scale(c, 1.0f / len);
    return true;
}

std::vector<Vector3> CollisionDetector::GetFaceNormals(const std::vector<Vector3> &vertices)
{
    std::vector<Vector3> normals;

    // we're going to use triplets of adjacent vertices to form faces (ppbg indices vector style)
    for (int i = 0; i < vertices.size(); i += 2)
    {
        const size_t next = (i + 2) % vertices.size();

        const Vector3 e1 = Vector3Subtract(vertices.at(next), vertices.at(i));
        const Vector3 e2 = Vector3Subtract(vertices.at((i + 1) % vertices.size()), vertices.at(i));

        if (const Vector3 n = Vector3CrossProduct(e1, e2); Vector3Length(n) > 1e-6f)
            normals.push_back(Vector3Normalize(n));
    }
    return normals;
}

bool CollisionDetector::CheckSphereSphere(const PhysicsObject &a, const PhysicsObject &b)
{
    float distance = Vector3Distance(a.GetPosition(), b.GetPosition());
    float radiusSum = a.GetSize() + b.GetSize();

    return distance < radiusSum;
}

bool CollisionDetector::CheckSphereConvex(PhysicsObject &sphere, PhysicsObject &convex)
{
    // Find the closest point on convex shape
    float minDistSq = FLT_MAX;
    for (auto &v: convex.GetLocalVertices())
    {
        Vector3 worldV = Vector3Transform(v, convex.GetTransform());
        Vector3 diff = Vector3Subtract(worldV, sphere.GetPosition());
        float distSq = Vector3DotProduct(diff, diff);
        if (distSq < minDistSq)
            minDistSq = distSq;
    }

    return minDistSq <= (sphere.GetRadius() * sphere.GetRadius());
}


bool CollisionDetector::CheckConvexCollision(PhysicsObject a, PhysicsObject b)
{
    if (a.GetType() == ObjectType::SPHERE && b.GetType() == ObjectType::SPHERE)
        return CheckSphereSphere(a, b);
    if (a.GetType() == ObjectType::SPHERE && b.GetType() != ObjectType::SPHERE)
        return CheckSphereConvex(a, b);
    if (a.GetType() != ObjectType::SPHERE && b.GetType() == ObjectType::SPHERE)
        return CheckSphereConvex(b, a);


    const std::vector<Vector3> verticesA = a.GetLocalVertices();
    const std::vector<Vector3> verticesB = b.GetLocalVertices();

    size_t verticesCountA = verticesA.size();
    size_t verticesCountB = verticesB.size();

    // vertices that get transformed to world space
    std::vector<Vector3> worldVerticesA;
    std::vector<Vector3> worldVerticesB;

    Matrix transformA = a.GetTransform();
    Matrix transformB = b.GetTransform();


    for (auto vertex: verticesA)
        worldVerticesA.push_back(Vector3Transform(vertex, transformA));

    for (auto vertex: verticesB)
        worldVerticesB.push_back(Vector3Transform(vertex, transformB));


    std::vector<Vector3> normals;
    normals = GetFaceNormals(worldVerticesA);

    std::vector<Vector3> normalsB = GetFaceNormals(worldVerticesB);

    normals.reserve(normalsB.size());
    normals.insert(normals.end(), normalsB.begin(), normalsB.end());

        for (int i = 0; i < verticesCountA; i++)
        {
            const size_t nextA = (i + 1) % verticesCountA;
            const Vector3 edgeA = Vector3Subtract(worldVerticesA.at(nextA), worldVerticesA.at(i));
            for (int j = 0; j < verticesCountB; j++)
            {
                const size_t nextB = (j + 1) % verticesCountB;
                const Vector3 edgeB = Vector3Subtract(worldVerticesB.at(nextB), worldVerticesB.at(j));
                Vector3 cross;
                if (TryCross(edgeA, edgeB, &cross))
                    normals.push_back(cross);
            }
        }

    constexpr float EPS = 1e-4f;

    for (auto normal: normals)
    {
        if (Vector3Length(normal) < EPS)
            continue;
        normal = Vector3Normalize(normal);

        float minA, maxA, minB, maxB;
        ProjectOntoAxis(worldVerticesA, verticesCountA, normal, &minA, &maxA);
        ProjectOntoAxis(worldVerticesB, verticesCountB, normal, &minB, &maxB);

        if (maxA < minB || maxB < minA)
            return false;
    }
    return true;
}
