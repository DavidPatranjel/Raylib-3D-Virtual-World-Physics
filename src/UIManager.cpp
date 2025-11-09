//
// Created by David on 26/10/2025.
//

#include "UIManager.h"


void UIManager::DrawGameUI(UIData uiData, bool isFreeMode) {
    int y = padding;

    DrawRectangle(0, 0, 250, (padding + lineHeight) * nrUIElements, backgroundColor);

    int fps = GetFPS();
    DrawText(TextFormat("FPS: %d", fps), padding, y, fontSize, textColor);
    y += lineHeight;

    DrawText(TextFormat("Objects: %d", uiData.objectCount), padding, y, fontSize, textColor);
    y += lineHeight;


    if (uiData.mode == FMode::USER_MODE) {
        DrawText("M/T* - Toggle mode", padding, y, fontSize, textColor);
        y += lineHeight;
        DrawText(TextFormat("Mode: USER"), padding, y, fontSize, MAGENTA);
        y += lineHeight;
    }
    else if (uiData.mode == FMode::DEBUG_MODE) {
        DrawText("M - To user mode", padding, y, fontSize, textColor);
        y += lineHeight;
        DrawText(TextFormat("Mode: DEBUG"), padding, y, fontSize, BLUE);
        y += lineHeight;
    }
    else
    {
        const char* functionMode;
        switch (uiData.genMode) {
            case GenMode::GEN1:
                functionMode = "GENERATING 1";
                break;
            case GenMode::GEN2:
                functionMode = "GENERATING 2";
                break;
            case GenMode::GEN3:
                functionMode = "GENERATING 3";
                break;
            default:
                functionMode = "GENERATING ERR_MODE";
                break;

        }

        DrawText("M/1/2/3- Toggle mode", padding, y, fontSize, textColor);
        y += lineHeight;
        DrawText(TextFormat("Mode: %s", functionMode), padding, y, fontSize, BLUE);
        y += lineHeight;
    }

    DrawText("O - Spawn object", padding, y, fontSize, textColor);
    y += lineHeight;

    DrawText("R - Reset scene", padding, y, fontSize, textColor);
    y += lineHeight;

    DrawText("C - Toggle camera", padding, y, fontSize, textColor);
    y += lineHeight;

    const char* cameraMode = isFreeMode ? "FREE" : "STATIC";
    Color cameraModeColor = isFreeMode ? GREEN : ORANGE;
    DrawText(TextFormat("Camera: %s", cameraMode), padding, y, fontSize, cameraModeColor);
}