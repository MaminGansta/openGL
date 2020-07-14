#pragma once

#include "framebuffer_specification.h"
#include "glm.hpp"
#include "GL/glew.h"
#include <cstdint>
#include <cassert>

namespace gl
{
	struct Framebuffer
	{
		uint32_t m_RendererID = 0;
		uint32_t m_ColorAttachment = 0, m_DepthAttachment = 0;
		FramebufferSpecification m_Specification;

		Framebuffer() = default;
		Framebuffer(const Framebuffer&) = delete;
		Framebuffer(Framebuffer&& other);
		Framebuffer& operator = (Framebuffer&& other);

		Framebuffer(const FramebufferSpecification& spec);
		void Create(const FramebufferSpecification& spec);

		~Framebuffer();

		void Bind();
		void Unbind();
		void Invalidate();

		const glm::ivec2& GetSize() const;
		void Resize(const glm::ivec2& size);

		uint32_t GetColorAttachmentRendererID();
		const FramebufferSpecification& GetSpecification() const;

	};
}