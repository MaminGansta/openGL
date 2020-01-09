
struct Texture
{
    uint32_t id;
    bool invalid = false;

    Texture(const char* filename)
    {
        int width, height, chanels;
        //unsigned char* image = SOIL_load_image(filename, &width, &height, 0, SOIL_LOAD_RGB);

        glGenTextures(1, &id);


        uint8_t* image = stbi_load(filename, &width, &height, &chanels, 0);
        if (image == NULL || height == 0)
        {
            printf("image failled : %s", filename);
            invalid = true;
        }

        GLenum format;
        if (chanels == 1)
            format = GL_RED;
        else if (chanels == 3)
            format = GL_RGB;
        else if (chanels == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // free data
        //glcall(SOIL_free_image_data(image));
        stbi_image_free(image);
        glcall(glBindTexture(GL_TEXTURE_2D, 0));
    }

    void bind(Shader& shader, const char* name, uint32_t slot)
    {
        shader.setUni1i(name, slot);
        glcall(glActiveTexture(GL_TEXTURE0 + slot));
        glcall(glBindTexture(GL_TEXTURE_2D, id));
    }
    void unbind() { glBindTexture(GL_TEXTURE_2D, 0); }
};
