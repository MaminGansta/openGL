
#include "framebuffer.h"

namespace gl
{
	Framebuffer::Framebuffer(const FramebufferSpecification& spec)
		: m_Specification(spec)
	{
		Invalidate();
	}

	Framebuffer::Framebuffer(Framebuffer&& other)
	{
		m_RendererID = other.m_RendererID;
		m_ColorAttachment = other.m_ColorAttachment;
		m_DepthAttachment = other.m_DepthAttachment;
		m_Specification = other.m_Specification;

		// make invalid
		other.m_RendererID = 0;
	}


	Framebuffer& Framebuffer::operator= (Framebuffer&& other)
	{
		m_RendererID = other.m_RendererID;
		m_ColorAttachment = other.m_ColorAttachment;
		m_DepthAttachment = other.m_DepthAttachment;
		m_Specification = other.m_Specification;

		// make invalid
		other.m_RendererID = 0;
		return *this;
	}


	void Framebuffer::Create(const FramebufferSpecification& spec)
	{
		m_Specification = spec;
		Invalidate();
	}

	Framebuffer::~Framebuffer()
	{
		if (m_RendererID)
		{
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures(1, &m_ColorAttachment);
			glDeleteTextures(1, &m_DepthAttachment);
		}
	}

	void Framebuffer::Invalidate()
	{
		if (m_RendererID)
		{
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures(1, &m_ColorAttachment);
			glDeleteTextures(1, &m_DepthAttachment);
		}

		glCreateFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		// Color
		glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachment);
		glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Specification.Size.x, m_Specification.Size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);

		// Depth
		glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);
		glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_Specification.Size.x, m_Specification.Size.y);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);

		assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE && "Framebuffer is incomplete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Framebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Specification.Size.x, m_Specification.Size.y);
	}

	void Framebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	const glm::ivec2& Framebuffer::GetSize() const 
	{
		return m_Specification.Size;
	}

	void Framebuffer::Resize(const glm::ivec2& size)
	{
		// we dont need actually resize it
		if ((size.x & size.y ) == 0)
			return;

		m_Specification.Size = size;
		Invalidate();
	}

	const FramebufferSpecification& Framebuffer::GetSpecification() const
	{
		return m_Specification;
	}

	uint32_t Framebuffer::GetColorAttachmentRendererID()
	{
		return m_ColorAttachment;
	}
}