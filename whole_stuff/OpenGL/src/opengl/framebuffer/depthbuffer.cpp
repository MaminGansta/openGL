
#include "depthbuffer.h"


namespace gl
{
	Depthbuffer::Depthbuffer(const FramebufferSpecification& spec)
		: m_Specification(spec)
	{
		Invalidate();
	}

	Depthbuffer::Depthbuffer(Depthbuffer&& other)
	{
		m_RendererID = other.m_RendererID;
		m_DepthAttachment = other.m_DepthAttachment;
		m_Specification = other.m_Specification;

		// make invalid
		other.m_RendererID = 0;
	}


	Depthbuffer& Depthbuffer::operator= (Depthbuffer&& other)
	{
		m_RendererID = other.m_RendererID;
		m_DepthAttachment = other.m_DepthAttachment;
		m_Specification = other.m_Specification;

		// make invalid
		other.m_RendererID = 0;
		return *this;
	}


	void Depthbuffer::Create(const FramebufferSpecification& spec)
	{
		m_Specification = spec;
		Invalidate();
	}

	Depthbuffer::~Depthbuffer()
	{
		if (m_RendererID)
		{
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures(1, &m_DepthAttachment);
		}
	}

	void Depthbuffer::Invalidate()
	{
		if (m_RendererID)
		{
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures(1, &m_DepthAttachment);
		}
		glCreateFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		// Depth
		//glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);
		//glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
		//glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_Specification.Size.x, m_Specification.Size.y);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);

		glGenTextures(1, &m_DepthAttachment);
		glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			m_Specification.Size.x, m_Specification.Size.x, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);

		assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE && "Depthbuffer is incomplete!");
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Depthbuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Specification.Size.x, m_Specification.Size.y);
	}

	void Depthbuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	const glm::ivec2& Depthbuffer::GetSize() const
	{
		return m_Specification.Size;
	}

	void Depthbuffer::Resize(const glm::ivec2& size)
	{
		// we dont need actually resize it
		if ((size.x & size.y) == 0)
			return;

		m_Specification.Size = size;
		Invalidate();
	}

	uint32_t Depthbuffer::GetDepthAttachmentRendererID()
	{
		return m_DepthAttachment;
	}

}