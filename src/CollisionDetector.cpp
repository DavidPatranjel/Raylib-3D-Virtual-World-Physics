//
// Created by mihae on 11/3/2025.
//

#include "CollisionDetector.h"

#include <cfloat>
#include <iostream>

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

///TODO: CHECK THIS
bool CollisionDetector::CheckSphereConvex(PhysicsObject &sphere, PhysicsObject &convex)
{
    Vector3 sphereCenter = sphere.GetPosition();
    float radius = sphere.GetRadius();

    // Transform all convex vertices to world space
    std::vector<Vector3> worldVertices;
    for (auto &v : convex.GetLocalVertices())
        worldVertices.push_back(Vector3Transform(v, convex.GetTransform()));

    // For each face of the convex shape, check if sphere is too far outside
    // We need to properly construct faces based on shape type

    if (convex.GetType() == ObjectType::CUBE)
    {
        return CheckSphereCube(sphere, worldVertices);
    }
    else if (convex.GetType() == ObjectType::CYLINDER)
    {
        return CheckSphereCylinder(sphere, worldVertices);
    }

    return false;
}


bool CollisionDetector::CheckSphereCube(PhysicsObject &sphere, const std::vector<Vector3> &cubeVertices)
{
    Vector3 sphereCenter = sphere.GetPosition();
    float radius = sphere.GetRadius();

    // Find the AABB (axis-aligned bounding box) of the cube in world space
    // But since the cube is rotated, we need to find the closest point on the oriented box

    // For a cube with 8 vertices, find the closest point by clamping to each face
    float minDistSq = FLT_MAX;

    // Define the 6 faces of the cube (each face has 4 vertices)
    int faces[6][4] = {
        {0, 1, 2, 3}, // back face
        {4, 5, 6, 7}, // front face
        {0, 1, 5, 4}, // bottom face
        {2, 3, 7, 6}, // top face
        {0, 3, 7, 4}, // left face
        {1, 2, 6, 5}  // right face
    };

    // Check each face
    for (auto & face : faces)
    {
        // Get 3 points to define the plane
        Vector3 p0 = cubeVertices[face[0]];
        Vector3 p1 = cubeVertices[face[1]];
        Vector3 p2 = cubeVertices[face[2]];

        // Calculate face normal
        Vector3 edge1 = Vector3Subtract(p1, p0);
        Vector3 edge2 = Vector3Subtract(p2, p0);
        Vector3 normal = Vector3Normalize(Vector3CrossProduct(edge1, edge2));

        // Distance from sphere center to plane
        float distToPlane = Vector3DotProduct(Vector3Subtract(sphereCenter, p0), normal);

        // Project sphere center onto the plane
        Vector3 pointOnPlane = Vector3Subtract(sphereCenter, Vector3Scale(normal, distToPlane));

        // Check if the projected point is inside the face rectangle
        // We'll use a simpler approach: check if point is on the correct side of all edges
        bool insideFace = true;

        for (int i = 0; i < 4; i++)
        {
            Vector3 v1 = cubeVertices[face[i]];
            Vector3 v2 = cubeVertices[face[(i + 1) % 4]];

            Vector3 edge = Vector3Subtract(v2, v1);
            Vector3 toPoint = Vector3Subtract(pointOnPlane, v1);

            // Cross product with face normal tells us which side we're on
            Vector3 cross = Vector3CrossProduct(edge, toPoint);
            if (Vector3DotProduct(cross, normal) < 0)
            {
                insideFace = false;
                break;
            }
        }

        if (insideFace)
        {
            // Closest point is on the face
            float distSq = distToPlane * distToPlane;
            if (distSq < minDistSq)
                minDistSq = distSq;
        }
        else
        {
            // Closest point might be on an edge or vertex of this face
            // Check all edges of this face
            for (int i = 0; i < 4; i++)
            {
                Vector3 edgeStart = cubeVertices[face[i]];
                Vector3 edgeEnd = cubeVertices[face[(i + 1) % 4]];

                float distSq = ClosestPointOnEdge(sphereCenter, edgeStart, edgeEnd);
                if (distSq < minDistSq)
                    minDistSq = distSq;
            }
        }
    }

    return minDistSq <= (radius * radius);
}

bool CollisionDetector::CheckSphereCylinder(PhysicsObject &sphere, const std::vector<Vector3> &cylinderVertices)
{
    Vector3 sphereCenter = sphere.GetPosition();
    float radius = sphere.GetRadius();
    float minDistSq = FLT_MAX;

    // Cylinder vertices are organized as pairs: bottom ring, then top ring
    // vertices[0], vertices[1] = first bottom, first top
    // vertices[2], vertices[3] = second bottom, second top, etc.

    int sides = cylinderVertices.size() / 2;

    // 1. Check distance to side faces (rectangles between bottom and top)
    for (int i = 0; i < sides; i++)
    {
        int next = (i + 1) % sides;

        // Four vertices of this rectangular face
        Vector3 bottomLeft = cylinderVertices[i * 2];
        Vector3 topLeft = cylinderVertices[i * 2 + 1];
        Vector3 bottomRight = cylinderVertices[next * 2];
        Vector3 topRight = cylinderVertices[next * 2 + 1];

        // Calculate face normal (pointing outward)
        Vector3 edge1 = Vector3Subtract(topLeft, bottomLeft);
        Vector3 edge2 = Vector3Subtract(bottomRight, bottomLeft);
        Vector3 normal = Vector3Normalize(Vector3CrossProduct(edge1, edge2));

        // Distance from sphere center to plane
        float distToPlane = Vector3DotProduct(Vector3Subtract(sphereCenter, bottomLeft), normal);

        // Project sphere center onto the plane
        Vector3 pointOnPlane = Vector3Subtract(sphereCenter, Vector3Scale(normal, distToPlane));

        // Check if projected point is inside the rectangular face
        // Simple approach: check if it's between the edges
        Vector3 toBL = Vector3Subtract(pointOnPlane, bottomLeft);
        Vector3 edgeBottom = Vector3Subtract(bottomRight, bottomLeft);
        Vector3 edgeLeft = Vector3Subtract(topLeft, bottomLeft);

        float projBottom = Vector3DotProduct(toBL, Vector3Normalize(edgeBottom));
        float projLeft = Vector3DotProduct(toBL, Vector3Normalize(edgeLeft));

        float edgeBottomLen = Vector3Length(edgeBottom);
        float edgeLeftLen = Vector3Length(edgeLeft);

        bool insideFace = (projBottom >= 0 && projBottom <= edgeBottomLen &&
                          projLeft >= 0 && projLeft <= edgeLeftLen);

        if (insideFace)
        {
            float distSq = distToPlane * distToPlane;
            if (distSq < minDistSq)
                minDistSq = distSq;
        }
        else
        {
            // Check edges of this face
            float d1 = ClosestPointOnEdge(sphereCenter, bottomLeft, bottomRight);
            float d2 = ClosestPointOnEdge(sphereCenter, topLeft, topRight);
            float d3 = ClosestPointOnEdge(sphereCenter, bottomLeft, topLeft);
            float d4 = ClosestPointOnEdge(sphereCenter, bottomRight, topRight);

            minDistSq = fminf(minDistSq, fminf(fminf(d1, d2), fminf(d3, d4)));
        }
    }

    // 2. Check distance to top and bottom caps (circular faces)
    // Bottom cap
    {
        Vector3 bottomCenter = {0, 0, 0};
        int count = 0;
        for (int i = 0; i < sides; i++)
        {
            bottomCenter = Vector3Add(bottomCenter, cylinderVertices[i * 2]);
            count++;
        }
        bottomCenter = Vector3Scale(bottomCenter, 1.0f / count);

        // Normal pointing down
        Vector3 normal = Vector3Normalize(Vector3Subtract(cylinderVertices[0], cylinderVertices[1]));
        float distToPlane = Vector3DotProduct(Vector3Subtract(sphereCenter, bottomCenter), normal);
        Vector3 pointOnPlane = Vector3Subtract(sphereCenter, Vector3Scale(normal, distToPlane));

        // Check if point is inside the circular cap
        float distFromCenter = Vector3Distance(pointOnPlane, bottomCenter);
        float capRadius = Vector3Distance(cylinderVertices[0], bottomCenter);

        if (distFromCenter <= capRadius)
        {
            float distSq = distToPlane * distToPlane;
            if (distSq < minDistSq)
                minDistSq = distSq;
        }
        else
        {
            // Check edges of bottom cap
            for (int i = 0; i < sides; i++)
            {
                int next = (i + 1) % sides;
                float distSq = ClosestPointOnEdge(sphereCenter, cylinderVertices[i * 2], cylinderVertices[next * 2]);
                if (distSq < minDistSq)
                    minDistSq = distSq;
            }
        }
    }

    // Top cap (similar to bottom)
    {
        Vector3 topCenter = {0, 0, 0};
        int count = 0;
        for (int i = 0; i < sides; i++)
        {
            topCenter = Vector3Add(topCenter, cylinderVertices[i * 2 + 1]);
            count++;
        }
        topCenter = Vector3Scale(topCenter, 1.0f / count);

        Vector3 normal = Vector3Normalize(Vector3Subtract(cylinderVertices[1], cylinderVertices[0]));
        float distToPlane = Vector3DotProduct(Vector3Subtract(sphereCenter, topCenter), normal);
        Vector3 pointOnPlane = Vector3Subtract(sphereCenter, Vector3Scale(normal, distToPlane));

        float distFromCenter = Vector3Distance(pointOnPlane, topCenter);
        float capRadius = Vector3Distance(cylinderVertices[1], topCenter);

        if (distFromCenter <= capRadius)
        {
            float distSq = distToPlane * distToPlane;
            if (distSq < minDistSq)
                minDistSq = distSq;
        }
        else
        {
            for (int i = 0; i < sides; i++)
            {
                int next = (i + 1) % sides;
                float distSq = ClosestPointOnEdge(sphereCenter, cylinderVertices[i * 2 + 1], cylinderVertices[next * 2 + 1]);
                if (distSq < minDistSq)
                    minDistSq = distSq;
            }
        }
    }

    return minDistSq <= (radius * radius);
}

// Helper function to find closest point on an edge
float CollisionDetector::ClosestPointOnEdge(const Vector3 &point, const Vector3 &edgeStart, const Vector3 &edgeEnd)
{
    Vector3 edge = Vector3Subtract(edgeEnd, edgeStart);
    Vector3 toPoint = Vector3Subtract(point, edgeStart);

    float edgeLengthSq = Vector3DotProduct(edge, edge);
    if (edgeLengthSq < 1e-6f)
    {
        // Degenerate edge
        Vector3 diff = Vector3Subtract(edgeStart, point);
        return Vector3DotProduct(diff, diff);
    }

    // Project point onto edge, clamped to [0, 1]
    float t = Vector3DotProduct(toPoint, edge) / edgeLengthSq;
    t = fmaxf(0.0f, fminf(1.0f, t));

    Vector3 closestPoint = Vector3Add(edgeStart, Vector3Scale(edge, t));
    Vector3 diff = Vector3Subtract(closestPoint, point);
    return Vector3DotProduct(diff, diff);
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
