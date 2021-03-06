#pragma once

#include "GL/glew.h"
#include <cstdint>

namespace gl
{
	struct Cubemap
	{
		uint32_t textureID;
		Cubemap(int width, int height);
	};

}