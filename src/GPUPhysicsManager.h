//
// Created by David on 02/12/2025.
//

#ifndef INC_3DVIRTUALWORLDPHYSICS_GPUPHYSICSMANAGER_H
#define INC_3DVIRTUALWORLDPHYSICS_GPUPHYSICSMANAGER_H



#include "raylib.h"
#include "rlgl.h"
#include "PhysicsObject.h"
#include <vector>

struct GPUPhysicsData {
    Vector3 position;
    float padding1;
    Vector3 velocity;
    float padding2;
    Vector3 rotationAxis;
    float radius;
    int objectType;
    int collisionFlag;
    float padding3[2];
};

class GPUPhysicsManager {
private:
    unsigned int computePhysicsProgram;      // Changed from Shader
    unsigned int computeCollisionProgram;    // Changed from Shader
    unsigned int physicsSSBO;
    std::vector<GPUPhysicsData> gpuData;
    int objectCount;
    bool initialized;

    // Helper function to load compute shader
    unsigned int LoadComputeShader(const char* shaderPath);

public:
    GPUPhysicsManager();
    ~GPUPhysicsManager();

    void Initialize();
    void UploadObjects(const std::vector<PhysicsObject>& objects);
    void UpdatePhysics(float deltaTime);
    void DetectCollisions();
    void DownloadResults(std::vector<PhysicsObject>& objects);
    void Cleanup();

    bool IsInitialized() const { return initialized; }
    int GetObjectCount() const { return objectCount; }
};


#endif //INC_3DVIRTUALWORLDPHYSICS_GPUPHYSICSMANAGER_H