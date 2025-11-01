//
// Created by David on 01/11/2025.
//

#ifndef INC_3DVIRTUALWORLDPHYSICS_FUNCTIONINGMODE_H
#define INC_3DVIRTUALWORLDPHYSICS_FUNCTIONINGMODE_H
#include "raylib.h"

enum class FMode{
    USER_MODE,
    GEN_MODE
};

enum class GenMode{
    GEN1,
    GEN2,
    GEN3
};

struct AllModes {
    FMode mode;
    GenMode genMode;
};


class FunctioningMode {
private:
    FMode mode = FMode::USER_MODE;
    GenMode genMode = GenMode::GEN1;

public:
    void SwitchMode();
    FMode GetMode() const { return mode;}
    GenMode GetGenMode() const { return genMode;}
    AllModes GetAllModes() const;
    void Update();
};


#endif //INC_3DVIRTUALWORLDPHYSICS_FUNCTIONINGMODE_H