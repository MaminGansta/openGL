
#include "input.h"

namespace gl
{
    // input
    char input[512];
    char mouse_input[512];

    float lastX = 400, lastY = 300;
    float MoffsetX = 0, MoffsetY = 0;
    float Mfov_offset = 0;
    float sensetivity = 0.004f;


    bool pressed(int code)
    {
        return input[code];
    }

    bool clicked(int code)
    {
        return input[code] == 1;
    }


    bool mouse_pressed(int code)
    {
        return mouse_input[code];
    }


    float mouse_offsetX()
    {
        float temp = MoffsetX;
        MoffsetX = 0;
        return temp;
    }

    float mouse_offsetY()
    {
        float temp = MoffsetY;
        MoffsetY = 0;
        return temp;
    }

    float fov_offset()
    {
        float temp = Mfov_offset;
        Mfov_offset = 0;
        return temp;
    }

}