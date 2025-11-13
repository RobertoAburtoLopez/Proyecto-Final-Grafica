#pragma once
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm.hpp>
#include <glfw3.h>

#define MAX_FRAMES 100

struct FRAME {
    float gx;
    float gy;
    float gz;
    float angle;
    float gx_Inc;
    float gy_Inc;
    float gz_Inc;
    float angle_Inc;
};

class KeyframeAnimation {
public:
    KeyframeAnimation();

    void SaveFrame();
    void LoadFrames();
    void ResetElements();
    void Interpolation();
    void Animate();
    void InputKeyframes(bool* keys);
    void Instrucciones();

    FRAME KeyFrame[MAX_FRAMES];
    int FrameIndex;
    float gx, gy, gz, angle;

private:
    int playIndex;
    int i_max_steps, i_curr_steps;

    float reproduciranimacion, habilitaranimacion, guardoFrame, reinicioFrame;
    float ciclo, ciclo2, ciclo3, ciclo4, ciclo5, ciclo6;
    bool play;
    bool keyLPressed;
    bool keyPPressed;
    bool keySpacePressed;
};
