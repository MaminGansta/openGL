#pragma once

#pragma once

#include "GL/glew.h"
#include <string>

namespace gl
{
	struct Skybox
	{
		uint32_t textureID;
		Skybox(const std::string& name);
	};

}