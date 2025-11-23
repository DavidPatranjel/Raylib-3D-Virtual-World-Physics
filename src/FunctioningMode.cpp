//
// Created by David on 01/11/2025.
//

#include "FunctioningMode.h"

void FunctioningMode::SwitchMode(bool debug){
    if (debug) {
        mode = FMode::DEBUG_MODE;
        return;
    }

    if (mode == FMode::USER_MODE) {
        mode = FMode::GEN_MODE;
    } else {
        mode = FMode::USER_MODE;
    }
}

void FunctioningMode::Update()
{
    if (IsKeyPressed(KEY_T)) {
        SwitchMode(true);
    }
    if (IsKeyPressed(KEY_M)) {
        SwitchMode();
    }

    if (mode == FMode::GEN_MODE) {
        if (IsKeyPressed(KEY_ONE)) {
            genMode = GenMode::GEN1;
        }
        else if (IsKeyPressed(KEY_TWO)) {
            genMode = GenMode::GEN2;
        }
        else if (IsKeyPressed(KEY_THREE)) {
            genMode = GenMode::GEN3;
        }
    }
}

AllModes FunctioningMode::GetAllModes() const {
    return AllModes{
        mode, genMode
    };
}