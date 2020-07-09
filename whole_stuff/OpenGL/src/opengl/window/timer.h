#pragma once

#include "GLFW/glfw3.h"

namespace gl
{
	struct Timer
	{
		double m_LastTime = 0;
		double m_DeltaTime = 0;

		inline void Update()
		{
			double now = glfwGetTime();
			m_DeltaTime = now - m_LastTime;
			m_LastTime = now;
		}

		inline double GetDeltaTime()
		{
			return m_DeltaTime;
		}
	};
}