#pragma once

#include "glm.hpp"

namespace gl
{
	struct FramebufferSpecification
	{
		glm::ivec2 Size;
		int Samples = 1;
		bool SwapChainTarget = false;
	};
}