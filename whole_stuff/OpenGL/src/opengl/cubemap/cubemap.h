#pragma once

#include "GL/glew.h"
#include <string>

namespace gl
{
	struct Cubemap
	{
		uint32_t textureID;

		Cubemap(const std::string& name);
	};

}