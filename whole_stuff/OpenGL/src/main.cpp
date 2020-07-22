
#include "opengl/gl_api.h"
#include "process_input/process_input.h"
#include <array>

int blur_passes = 0;
void gaussian_blur_func(gl::Framebuffer& framebuffer, gl::Framebuffer& buffer, gl::Shader& gauss, unsigned int FullScreenQuadVAO);

int main(void)
{
    gl::Window window("OpenGL");

    // camera
    gl::Camera camera;

    // preapare shaders
    gl::Shader phong("shaders/phong.glsl");
    if (phong.invalid) return 1;

    gl::Shader blinn_phong("shaders/blinn_phong.glsl");
    if (blinn_phong.invalid) return 1;

    gl::Shader show_normals("shaders/show_normals.glsl");
    if (show_normals.invalid) return 1;

    gl::Shader solid_color("shaders/lighter.glsl");
    if (solid_color.invalid) return 1;

    gl::Shader shadow_phong("shaders/shadow_phong.glsl");
    if (shadow_phong.invalid) return 1;

    gl::Shader image_shader("shaders/draw_image_shader.glsl");
    if (image_shader.invalid) return 1;

    gl::Shader draw_shadow("shaders/draw_only_shadow.glsl");
    if (draw_shadow.invalid) return 1;

    gl::Shader gaussian_blur("shaders/gaussian_blur.glsl");
    if (gaussian_blur.invalid) return 1;



    // cube and lighter position
    glm::vec3 lightPos(2.5f, 0.0f, -6.0f);
    glm::vec3 cube_pos(0.0f, 0.0f, -3.5f);
    glm::vec3 light_dir(-0.2f, -1.0f, -0.3f);

    // some matrix staff
    glm::mat4 identity(1.0f);
    glm::mat4 model(1.0f);
    glm::mat4 view(1.0f);
    glm::mat4 projection(1.0f);

    // model
    gl::Model crysis = gl::ModelLoader::LoadModel("models/crysis/nanosuit.obj");
    gl::Model plane = gl::ModelLoader::LoadModel("models/plane/scene.gltf", glm::vec3(0, -1, 0), glm::vec3(0.04f));
    gl::Model box = gl::ModelLoader::LoadModel("models/box/box.obj", glm::vec3(55, 0, 20), glm::vec3 (0.5f));

    std::vector<gl::Model> models;

    models.push_back(plane);
    models.push_back(box);

    // nanosuits positions
    std::array<glm::vec3, 5> suits_pos
    { glm::vec3(0.0f, -1.0f, 0.0f),
      glm::vec3(30.0f, -1.0f, 0.0f),
      glm::vec3(-30.0f, -1.0f, 0.0f),
      glm::vec3(0.0f, -1.0f, 30.0f),
      glm::vec3(0.0f, -1.0f, -30.0f),
    };

    for (int i = 0; i < 5; i++)
        models.push_back(gl::Model(crysis, suits_pos[i], glm::vec3(0.5f)));

    // lighters
    std::vector<gl::Light> lighters;
    lighters.push_back(gl::CreateDirLight(glm::normalize(glm::vec3(-0.2f, -0.2f, -0.5f))));
    lighters.push_back(gl::CreatePointLihgt(glm::vec3(0.4, 1, 1)));
    lighters.push_back(gl::CreatePointLihgt(glm::vec3(20, 1, 1)));
    lighters.push_back(gl::CreatePointLihgt(glm::vec3(55, 6, -20)));


    // skybox 
    gl::Skybox skybox("cubemaps/skybox/skybox.jpg");
    gl::Shader skyboxShader("shaders/skybox.glsl");

    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    uint32_t skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);

    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);


    // shadow map texture
    unsigned int ShadowQuadVAO = 0;
    unsigned int ShadowQuadVBO;
    float ShadowQuadVertices[] = {
        // positions        // texture Coords
        -1.0f,  -0.2f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         -0.2f,  -0.2f, 0.0f, 1.0f, 1.0f,
         -0.2f, -1.0f, 0.0f, 1.0f, 0.0f,
    };
     // setup plane VAO
     glGenVertexArrays(1, &ShadowQuadVAO);
     glGenBuffers(1, &ShadowQuadVBO);
     glBindVertexArray(ShadowQuadVAO);
     glBindBuffer(GL_ARRAY_BUFFER, ShadowQuadVBO);
     glBufferData(GL_ARRAY_BUFFER, sizeof(ShadowQuadVertices), &ShadowQuadVertices, GL_STATIC_DRAW);
     glEnableVertexAttribArray(0);
     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
     glEnableVertexAttribArray(1);
     glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));


     unsigned int FullScreenQuadVAO = 0;
     unsigned int FullScreenQuadVBO;
     float quadVertices[] = {
         // positions        // texture Coords
         -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
         -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
          1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
          1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
     };
     // setup plane VAO
     glGenVertexArrays(1, &FullScreenQuadVAO);
     glGenBuffers(1, &FullScreenQuadVBO);
     glBindVertexArray(FullScreenQuadVAO);
     glBindBuffer(GL_ARRAY_BUFFER, FullScreenQuadVBO);
     glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
     glEnableVertexAttribArray(0);
     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
     glEnableVertexAttribArray(1);
     glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));


    // zbuffer anable
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);

    bool normal_mamming_flag = false;
    bool shadow_flag = false;
    bool shadow_map_flag = false;
    bool shadow_blur_flag = true;

    // shadow maping
    gl::Shader depthShader("shaders/depth_shader.glsl");
    if (depthShader.invalid) return 1;
    gl::Framebuffer shadow_map({ {2048, 2048} });

    float near_plane = 0.1f, far_plane = 100.0f;
    float left = -100, right = 100, bot = -100, top = 100;
    glm::mat4 lightProjection = glm::ortho(left, right, bot, top, near_plane, far_plane);

    // shadow cubemap
    gl::Shader cubemapDepthShader("shaders/cubemap_depth_shader.glsl");
    if (cubemapDepthShader.invalid) return 1;

    gl::Cubemap shadowCubemap(1024, 1024);
    uint32_t depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowCubemap.textureID, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // blur stuff
    gl::Framebuffer shadow_raw({ {800, 600} });
    gl::Framebuffer shadow_blur_buffer({ {800, 600} });


    gl::Timer timer;
    while (window.isOpen())
    {
        timer.Update();
        glfwPollEvents();

        // process input 
        ProcessCameraMovement(window, camera, timer.GetDeltaTime());

        // Input setting
        static float input_delay = 0.3f;
        input_delay -= timer.m_DeltaTime;
        if (input_delay < 0.0f && gl::clicked(GLFW_KEY_N))
        {
            normal_mamming_flag ^= true;
            input_delay = 0.3f;
        }
        if (input_delay < 0.0f && gl::clicked(GLFW_KEY_M))
        {
            shadow_flag ^= true;
            input_delay = 0.3f;
        }
        if (input_delay < 0.0f && gl::clicked(GLFW_KEY_B))
        {
            shadow_blur_flag ^= true;
            input_delay = 0.3f;
        }
        if (input_delay < 0.0f && gl::clicked(GLFW_KEY_V))
        {
            shadow_map_flag ^= true;
            input_delay = 0.3f;
        }
        if (input_delay < 0.0f && gl::clicked(GLFW_KEY_UP))
        {
            blur_passes = __min(blur_passes + 1, 20);
            printf("blur passes: %d\n", blur_passes);
            input_delay = 0.3f;
        }
        if (input_delay < 0.0f && gl::clicked(GLFW_KEY_DOWN))
        {
            blur_passes = __max(blur_passes - 1, 1);
            printf("blur passes: %d\n", blur_passes);
            input_delay = 0.3f;
        }

        // Shadow map lookAt
        glm::mat4 lightView = glm::lookAt(camera.m_Position + camera.m_Front * top * 0.2f + -lighters[0].direction * far_plane / 2.0f,
                                          camera.m_Position + camera.m_Front * top * 0.2f,
                                          glm::vec3(0.0f, 1.0f, 0.0f));
        // clear screen
        //glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);

        // update lighters
        gl::ApplyLightToShader(lighters, blinn_phong);
        gl::ApplyLightToShader(lighters, draw_shadow);

        // move global light
        float radius = 5.0f;
        float lightX = sinf(glfwGetTime() * 0.2f) * radius;
        float lightZ = cosf(glfwGetTime() * 0.2f) * radius;
        lighters[0].direction = glm::normalize(-glm::vec3(lightX, 3.0f, lightZ));
        lighters[3].position = glm::vec3(55 + lightX, 6, -20);

        view = camera.GetLookatMat();
        projection = camera.GetPprojectionMat(window.GetSize().x, window.GetSize().y);
        

        // ============================ shadows map ==============================
        //glCullFace(GL_FRONT);
        // create shadow map
        depthShader.setUniMat4("lightSpaceMatrix", lightProjection * lightView);
        shadow_map.Bind();
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        for (auto& model : models)
            model.Draw(depthShader);

        shadow_map.Unbind();
        //glCullFace(GL_BACK);
        // ====================================================================

        // ========================= shadow cubeMap =============================
        //glCullFace(GL_FRONT);
        
        float aspect = (float)1024 / (float)1024;
        float near = 1.0f;
        float far = 25.0f;
        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, far);

        std::vector<glm::mat4> shadowTransforms;
        shadowTransforms.push_back(shadowProj *
            glm::lookAt(lighters[3].position, lighters[3].position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(shadowProj *
            glm::lookAt(lighters[3].position, lighters[3].position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(shadowProj *
            glm::lookAt(lighters[3].position, lighters[3].position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
        shadowTransforms.push_back(shadowProj *
            glm::lookAt(lighters[3].position, lighters[3].position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
        shadowTransforms.push_back(shadowProj *
            glm::lookAt(lighters[3].position, lighters[3].position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(shadowProj *
            glm::lookAt(lighters[3].position, lighters[3].position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
        
        for (int i = 0; i < 6; i++)
            cubemapDepthShader.setUniMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);

        cubemapDepthShader.setUni3f("lightPos", lighters[3].position);
        cubemapDepthShader.setUni1f("far_plane", far);
        
        glViewport(0, 0, 1024, 1024);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        for (auto& model : models)
            model.Draw(cubemapDepthShader);

        //glCullFace(GL_BACK);
        // ======================================================================

        
        // ========================== Render shadows in scene ========================================
        int down_scale_shadow_quality = 1;
        if (window.GetSize() / down_scale_shadow_quality != shadow_raw.GetSize())
        {
            shadow_raw.Resize(window.GetSize() / down_scale_shadow_quality);
            shadow_blur_buffer.Resize(window.GetSize() / down_scale_shadow_quality);
        }

        shadow_raw.Bind();
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        draw_shadow.setUni3f("viewPos", camera.m_Position);
        draw_shadow.setUniMat4("projection", projection);
        draw_shadow.setUniMat4("view", view);
        draw_shadow.setUniMat4("lightSpaceMatrix", lightProjection* lightView);

        glActiveTexture(GL_TEXTURE0 + 6);
        glBindTexture(GL_TEXTURE_2D, shadow_map.m_DepthAttachment);
        draw_shadow.setUni1i("shadowMap", 6);

        // attach cubemap
        draw_shadow.setUni1f("far_plane", far);
        glActiveTexture(GL_TEXTURE0 + 7);
        glBindTexture(GL_TEXTURE_CUBE_MAP, shadowCubemap.textureID);
        draw_shadow.setUni1i("depthMap", 7);

        for (auto& model : models)
            model.Draw(draw_shadow);

        
        if (shadow_blur_flag)
            gaussian_blur_func(shadow_raw, shadow_blur_buffer, gaussian_blur, FullScreenQuadVAO);

        // ================================ Render scene ===================================
        window.UseViewPort();

        blinn_phong.setUni3f("viewPos", camera.m_Position);
        blinn_phong.setUniMat4("projection", projection);
        blinn_phong.setUniMat4("view", view);
        blinn_phong.setUniMat4("lightSpaceMatrix", lightProjection * lightView);
        blinn_phong.setUni2f("viewportSize", window.GetSize());

        glActiveTexture(GL_TEXTURE0 + 6);
        blinn_phong.setUni1i("shadowMap", 6);
        glBindTexture(GL_TEXTURE_2D, shadow_raw.m_ColorAttachment);


        for (auto& model : models)
            model.Draw(blinn_phong);


        // draw skybox
        glDepthMask(GL_FALSE);
        skyboxShader.Use();

        skyboxShader.setUniMat4("projection", projection);
        skyboxShader.setUniMat4("view", glm::mat3(view));
        glBindVertexArray(skyboxVAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.textureID);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);

        
        // draw shadows buffer on the screen
        if (shadow_flag)
        {
            shadow_raw.Unbind();
            image_shader.Use();
            glActiveTexture(GL_TEXTURE0 + 6);
            image_shader.setUni1i("depthMap", 6);
            glBindTexture(GL_TEXTURE_2D, shadow_raw.m_ColorAttachment);
            glBindVertexArray(FullScreenQuadVAO);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glBindVertexArray(0);
        }

        // draw shadowmap
        if (shadow_map_flag)
        {
            image_shader.Use();
            glActiveTexture(GL_TEXTURE0 + 6);
            image_shader.setUni1i("depthMap", 6);
            glBindTexture(GL_TEXTURE_2D, shadow_map.m_ColorAttachment);
            glBindVertexArray(ShadowQuadVAO);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glBindVertexArray(0);
        }

        window.SwapBuffer();
    }

    return 0;
}




void gaussian_blur_func(gl::Framebuffer& framebuffer, gl::Framebuffer& buffer, gl::Shader& gauss, unsigned int FullScreenQuadVAO)
{
    glDisable(GL_DEPTH_TEST);

    for (int i = 0; i < blur_passes; i++)
    {
        // horizontal
        buffer.Bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, framebuffer.m_ColorAttachment);
        gauss.setUni1i("image", 0);
        gauss.setUni1i("horizontal", 0);

        glBindVertexArray(FullScreenQuadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);

        // vertical
        framebuffer.Bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, buffer.m_ColorAttachment);
        gauss.setUni1i("image", 0);
        gauss.setUni1i("horizontal", 1);

        glBindVertexArray(FullScreenQuadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }

    glEnable(GL_DEPTH_TEST);
}