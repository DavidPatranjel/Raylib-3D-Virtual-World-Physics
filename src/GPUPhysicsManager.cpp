#include "GPUPhysicsManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "external/glad.h"

GPUPhysicsManager::GPUPhysicsManager()
    : computePhysicsProgram(0), computeCollisionProgram(0),
      physicsSSBO(0), objectCount(0), initialized(false) {
}

GPUPhysicsManager::~GPUPhysicsManager() {
    Cleanup();
}

unsigned int GPUPhysicsManager::LoadComputeShader(const char* shaderPath) {
    std::cout << "Loading compute shader: " << shaderPath << std::endl;

    // Read shader source from file
    std::ifstream shaderFile(shaderPath);
    if (!shaderFile.is_open()) {
        std::cerr << "Failed to open shader file: " << shaderPath << std::endl;
        return 0;
    }

    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    std::string shaderCode = shaderStream.str();
    const char* shaderSource = shaderCode.c_str();

    // Create and compile compute shader
    unsigned int computeShader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(computeShader, 1, &shaderSource, NULL);
    glCompileShader(computeShader);

    // Check for compilation errors
    int success;
    char infoLog[512];
    glGetShaderiv(computeShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(computeShader, 512, NULL, infoLog);
        std::cerr << "Compute shader compilation failed: " << infoLog << std::endl;
        glDeleteShader(computeShader);
        return 0;
    }
    std::cout << "✓ Shader compiled successfully" << std::endl;

    // Create program and attach shader
    unsigned int program = glCreateProgram();
    glAttachShader(program, computeShader);
    glLinkProgram(program);

    // Check for linking errors
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "Compute shader program linking failed: " << infoLog << std::endl;
        glDeleteShader(computeShader);
        glDeleteProgram(program);
        return 0;
    }
    std::cout << "✓ Program linked successfully (ID: " << program << ")" << std::endl;

    // Clean up shader (no longer needed after linking)
    glDeleteShader(computeShader);

    return program;
}

void GPUPhysicsManager::Initialize() {
    if (initialized) return;

    std::cout << "=== Initializing GPU Physics Manager ===" << std::endl;

    // Check if compute shaders are supported
    int workGroupCount[3];
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &workGroupCount[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &workGroupCount[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &workGroupCount[2]);

    std::cout << "Max compute work group count: "
              << workGroupCount[0] << ", "
              << workGroupCount[1] << ", "
              << workGroupCount[2] << std::endl;

    if (workGroupCount[0] == 0) {
        std::cerr << "ERROR: Compute shaders not supported on this GPU!" << std::endl;
        return;
    }

    // Load compute shaders using our custom loader
    computePhysicsProgram = LoadComputeShader("shaders/physics_update.comp");
    if (computePhysicsProgram == 0) {
        std::cerr << "Failed to load physics compute shader!" << std::endl;
        return;
    }

    computeCollisionProgram = LoadComputeShader("shaders/collision_detect.comp");
    if (computeCollisionProgram == 0) {
        std::cerr << "Failed to load collision compute shader!" << std::endl;
        return;
    }

    // Create Shader Storage Buffer Object
    glGenBuffers(1, &physicsSSBO);
    std::cout << "✓ SSBO created (ID: " << physicsSSBO << ")" << std::endl;

    std::cout << "✓ GPU Physics Manager initialized successfully!" << std::endl;
    initialized = true;
}

void GPUPhysicsManager::UploadObjects(const std::vector<PhysicsObject>& objects) {
    if (!initialized) {
        std::cerr << "GPU Physics Manager not initialized!" << std::endl;
        return;
    }

    objectCount = objects.size();
    if (objectCount == 0) return;

    // Prepare GPU data
    gpuData.clear();
    gpuData.reserve(objectCount);

    for (const auto& obj : objects){
        GPUPhysicsData data;
        data.position = obj.GetPosition();
        data.rotationAngle = obj.GetRotationAngle();
        data.velocity = obj.GetVelocity();
        data.rotationSpeed = 0.03f;  // Match the rotation speed from PhysicsObject
        data.rotationAxis = obj.GetRotationAxis();
        data.radius = obj.GetRadius();
        data.objectType = static_cast<int>(obj.GetType());
        data.collisionFlag = 0;
        data.padding3[0] = 0.0f;
        data.padding3[1] = 0.0f;

        gpuData.push_back(data);
    }

    // Upload to GPU
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, physicsSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
                 gpuData.size() * sizeof(GPUPhysicsData),
                 gpuData.data(),
                 GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, physicsSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void GPUPhysicsManager::UpdatePhysics(float deltaTime) {
    if (!initialized || objectCount == 0) return;

    // Use the compute program
    glUseProgram(computePhysicsProgram);

    // Bind the buffer
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, physicsSSBO);

    // Set uniforms using glUniform instead of Raylib's SetShaderValue
    float gravity = GRAVITY;
    float frictionInv = FRICTION_INV;
    float spaceLimits = SPACE_LIMITS;
    float energyLoss = ENERGY_LOSS;

    int deltaTimeLoc = glGetUniformLocation(computePhysicsProgram, "deltaTime");
    int gravityLoc = glGetUniformLocation(computePhysicsProgram, "gravity");
    int frictionLoc = glGetUniformLocation(computePhysicsProgram, "frictionInv");
    int limitsLoc = glGetUniformLocation(computePhysicsProgram, "spaceLimits");
    int energyLoc = glGetUniformLocation(computePhysicsProgram, "energyLoss");

    if (deltaTimeLoc != -1) glUniform1f(deltaTimeLoc, deltaTime);
    if (gravityLoc != -1) glUniform1f(gravityLoc, gravity);
    if (frictionLoc != -1) glUniform1f(frictionLoc, frictionInv);
    if (limitsLoc != -1) glUniform1f(limitsLoc, spaceLimits);
    if (energyLoc != -1) glUniform1f(energyLoc, energyLoss);

    // Dispatch compute shader
    int workGroups = (objectCount + 255) / 256;
    glDispatchCompute(workGroups, 1, 1);

    // Wait for completion
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    // Unbind program
    glUseProgram(0);
}

void GPUPhysicsManager::DetectCollisions() {
    if (!initialized || objectCount == 0) return;

    // Use the compute program
    glUseProgram(computeCollisionProgram);

    // Bind the buffer
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, physicsSSBO);

    // Set uniforms
    int countLoc = glGetUniformLocation(computeCollisionProgram, "objectCount");
    if (countLoc != -1) glUniform1i(countLoc, objectCount);

    // Dispatch compute shader
    int workGroups = (objectCount + 15) / 16;
    glDispatchCompute(workGroups, workGroups, 1);

    // Wait for completion
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    // Unbind program
    glUseProgram(0);
}

void GPUPhysicsManager::DownloadResults(std::vector<PhysicsObject>& objects) {
    if (!initialized || objectCount == 0) return;

    // Download from GPU
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, physicsSSBO);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0,
                       gpuData.size() * sizeof(GPUPhysicsData),
                       gpuData.data());
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // Update CPU objects
    for (int i = 0; i < objectCount && i < objects.size(); i++) {
        objects[i].SetPosition(gpuData[i].position);
        objects[i].SetVelocity(gpuData[i].velocity);
        objects[i].SetRotationAngle(gpuData[i].rotationAngle);
        objects[i].SetIsColliding(gpuData[i].collisionFlag == 1);
    }
}

void GPUPhysicsManager::Cleanup() {
    if (physicsSSBO != 0) {
        glDeleteBuffers(1, &physicsSSBO);
        physicsSSBO = 0;
    }

    if (computePhysicsProgram != 0) {
        glDeleteProgram(computePhysicsProgram);
        computePhysicsProgram = 0;
    }

    if (computeCollisionProgram != 0) {
        glDeleteProgram(computeCollisionProgram);
        computeCollisionProgram = 0;
    }

    initialized = false;
}