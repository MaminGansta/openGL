
//#ifndef STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_IMPLEMENTATION
//#endif 
#include "stb_image.h"

#include "skybox.h"

namespace gl
{
	Skybox::Skybox(const std::string& name)
	{
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		size_t dot_position = name.find_last_of(".");
		std::string path = name.substr(0, dot_position);
		std::string ext = name.substr(dot_position);

		int width, height, nrChannels;
		unsigned char* data;
		for (GLuint i = 0; i < 6; i++)
		{
			data = stbi_load((path + std::to_string(i) + ext).c_str(), &width, &height, &nrChannels, 0);

			if (data)
			{
				glTexImage2D(
					GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
				);
				stbi_image_free(data);
			}
			else
				printf("cant load cubemap %s\n", (path + std::to_string(i) + ext).c_str());
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}
}