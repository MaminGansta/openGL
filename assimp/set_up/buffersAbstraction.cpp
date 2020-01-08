

struct VertexBuffer
{
	uint32_t id = 0;

	VertexBuffer(void* data, uint32_t size, int draw_type)
	{
		glcall(glGenBuffers(1, &id));
		glcall(glBindBuffer(GL_ARRAY_BUFFER, id));
		glcall(glBufferData(GL_ARRAY_BUFFER, size, data, draw_type));
	}
	~VertexBuffer() { glcall(glDeleteBuffers(1, &id)); }
	void bind() const { glcall(glBindBuffer(GL_ARRAY_BUFFER, id)); }
	void unbind() const { glcall(glBindBuffer(GL_ARRAY_BUFFER, 0)); }
};

struct IndexBuffer
{
	uint32_t id;
	uint32_t count;

	IndexBuffer(uint32_t* data, uint32_t count, int draw_type) : count(count)
	{
		glcall(glGenBuffers(1, &id));
		glcall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id));
		glcall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t)*count, data, draw_type));
	}
	~IndexBuffer() { glcall(glDeleteBuffers(1, &id)); }
	void bind() const { glcall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id)); }
	void unbind() const { glcall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0)); }
};


struct VertexBufferElement
{
	uint32_t type;
	uint32_t count;
	uint8_t normalized;
	uint8_t type_size;
};

struct VertexBufferLayout
{
	std::vector<VertexBufferElement> elements;
	int stride = 0;

	template <typename T>
	void push(uint32_t count)
	{
		static_assert(false);
	}

	template <>
	void push<float>(uint32_t count)
	{
		elements.push_back({ GL_FLOAT, count, GL_FALSE, sizeof(float) });
		stride += sizeof(float) * count;
	}

	template <>
	void push<uint32_t>(uint32_t count)
	{
		elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE, sizeof(uint32_t) });
		stride += sizeof(uint32_t) * count;
	}

	template <>
	void push<uint8_t>(uint32_t count)
	{
		elements.push_back({ GL_UNSIGNED_BYTE, count, GL_FALSE, sizeof(uint8_t) });
		stride += sizeof(uint8_t) * count;
	}
};


struct VertexArray
{
	uint32_t id;

	VertexArray()
	{
		glcall(glGenVertexArrays(1, &id));
		glcall(glBindVertexArray(id));
	}

	~VertexArray()
	{
		glcall(glDeleteVertexArrays(1, &id));
	}

	void addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
	{
		vb.bind();
		uint32_t offset = 0;
		for (int i = 0; i < layout.elements.size(); i++)
		{
			VertexBufferElement const& element = layout.elements[i];
			glcall(glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.stride, (void*)offset));
			glcall(glEnableVertexAttribArray(i));
			offset += element.count * element.type_size;
		}
		unbind();
	}

	void bind() const { glcall(glBindVertexArray(id)); }
	void unbind() const { glcall(glBindVertexArray(0)); }

};