//
// Created by David on 26/10/2025.
//

#include "UIManager.h"

void UIManager::DrawGameUI(int objectCount, bool isFreeMode) {
    int y = padding;

    DrawRectangle(0, 0, 250, (padding + lineHeight) * nrUIElements, backgroundColor);

    int fps = GetFPS();
    DrawText(TextFormat("FPS: %d", fps), padding, y, fontSize, textColor);
    y += lineHeight;

    DrawText(TextFormat("Objects: %d", objectCount), padding, y, fontSize, textColor);
    y += lineHeight;

    DrawText("O - Spawn object", padding, y, fontSize, textColor);
    y += lineHeight;

    DrawText("R - Reset scene", padding, y, fontSize, textColor);
    y += lineHeight;

    DrawText("C - Toggle camera", padding, y, fontSize, textColor);
    y += lineHeight;

    const char* cameraMode = isFreeMode ? "FREE" : "STATIC";
    Color modeColor = isFreeMode ? GREEN : ORANGE;
    DrawText(TextFormat("Camera: %s", cameraMode), padding, y, fontSize, modeColor);
}