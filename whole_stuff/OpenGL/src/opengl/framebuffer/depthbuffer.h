#pragma once

#include "framebuffer_specification.h"
#include "glm.hpp"
#include "GL/glew.h"
#include <cstdint>
#include <cassert>


namespace gl
{

	struct Depthbuffer
	{
		uint32_t m_RendererID = 0;
		uint32_t m_DepthAttachment = 0;
		FramebufferSpecification m_Specification;

		Depthbuffer() = default;
		Depthbuffer(const Depthbuffer&) = delete;
		Depthbuffer(Depthbuffer&& other);
		Depthbuffer& operator = (Depthbuffer&& other);

		Depthbuffer(const FramebufferSpecification& spec);
		void Create(const FramebufferSpecification& spec);

		~Depthbuffer();

		void Bind();
		void Unbind();
		void Invalidate();

		const glm::ivec2& GetSize() const;
		void Resize(const glm::ivec2& size);

		uint32_t GetDepthAttachmentRendererID();
		const FramebufferSpecification& GetSpecification() const;
	};
}