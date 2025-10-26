//
// Created by David on 26/10/2025.
//

#ifndef INC_3DVIRTUALWORLDPHYSICS_UIMANAGER_H
#define INC_3DVIRTUALWORLDPHYSICS_UIMANAGER_H
#include "raylib.h"


class UIManager {
private:
    static constexpr int fontSize = 18;
    static constexpr int padding = 10;
    static constexpr int lineHeight = 30;
    static constexpr Color textColor = WHITE;
    static constexpr Color backgroundColor = {0, 0, 0, 150};
    static constexpr int nrUIElements = 5;

public:
    static void DrawGameUI(int objectCount, bool isFreeMode);
};

#endif //INC_3DVIRTUALWORLDPHYSICS_UIMANAGER_H