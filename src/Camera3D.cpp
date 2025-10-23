//
// Created by David on 24/10/2025.
//

#include "Camera3D.h"


Camera GetDefaultCamera() {
    Camera camera = {0};
    camera.position = cameraPosition;
    camera.target = cameraTarget;
    camera.up = cameraUp;
    camera.fovy = cameraFovy;
    camera.projection = cameraPerspective;
    return camera;
}