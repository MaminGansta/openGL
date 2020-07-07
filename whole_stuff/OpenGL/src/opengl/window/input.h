#pragma once

namespace gl
{

    // input
    extern char input[512];
    extern char mouse_input[512];


    extern float lastX, lastY;
    extern float MoffsetX, MoffsetY;
    extern float Mfov_offset;
    extern float sensetivity;

    bool pressed(int code);
    bool mouse_pressed(int code);
    bool clicked(int code);
    float mouse_offsetX();
    float mouse_offsetY();
    float fov_offset();

}