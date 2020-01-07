
struct Texture
{
    uint32_t id;
    bool invalid = false;

    Texture(const char* filename)
    {
        int width, height;
        unsigned char* image = SOIL_load_image(filename, &width, &height, 0, SOIL_LOAD_RGB);
        if (image == NULL || height == 0)
        {
            printf("image failled : %s", filename);
            invalid = true;
        }

        glcall(glGenTextures(1, &id));
        glcall(glBindTexture(GL_TEXTURE_2D, id));

        glcall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image));
        glcall(glGenerateMipmap(GL_TEXTURE_2D));

        // free data
        glcall(SOIL_free_image_data(image));
        glcall(glBindTexture(GL_TEXTURE_2D, 0));
    }

    void bind(Shader& shader, uint32_t slot)
    {
        shader.setUni1i("material.specular", slot);
        glcall(glActiveTexture(GL_TEXTURE0 + slot));
        glcall(glBindTexture(GL_TEXTURE_2D, id));
    }
    void unbind() { glBindTexture(GL_TEXTURE_2D, 0); }
};
