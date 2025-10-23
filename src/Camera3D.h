//
// Created by David on 24/10/2025.
//

#ifndef INC_3DVIRTUALWORLDPHYSICS_CAMERA3D_H
#define INC_3DVIRTUALWORLDPHYSICS_CAMERA3D_H


#include "raylib.h"

constexpr Vector3 cameraPosition = { -6.5f, -1.5f, -8.5f };
constexpr Vector3 cameraTarget = { 2.5f, 1.5f, 2.5f };
constexpr Vector3 cameraUp = { 0.0f, 1.0f, 0.0f };
constexpr float cameraFovy = 45.0f;
constexpr int cameraPerspective = CAMERA_PERSPECTIVE;


Camera GetDefaultCamera();

#endif //INC_3DVIRTUALWORLDPHYSICS_CAMERA3D_H