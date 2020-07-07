#pragma once

#include "glm.hpp"
#include "GLFW/glfw3.h"
#include "input.h"
#include <unordered_map>

namespace gl
{
	struct Window
	{
        GLFWwindow* m_Window;
		
        Window(const char* name, int width = 800, int height = 600);
        ~Window();

        void Resize(int width, int height);
        void ResizeFrameBuffer();

        void CatchCursor(bool flag);
        
        bool isOpen();

        glm::ivec2 GetSize();

        void SwapBuffer();

	};

    static std::unordered_map<GLFWwindow*, Window*> windows;
}