
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
    gl::Model crysis("models/crysis/nanosuit.obj");
    gl::Model plane("models/plane/scene.gltf");
    gl::Model box("models/box/box.obj");

    // lighters
    std::vector<gl::Light> lighters;
    lighters.push_back(gl::CreateDirLight(glm::normalize(glm::vec3(-0.2f, -0.2f, -0.5f))));
    lighters.push_back(gl::CreatePointLihgt(glm::vec3(0.4, 1, 1)));
    lighters.push_back(gl::CreatePointLihgt(glm::vec3(20, 1, 1)));
    lighters.push_back(gl::CreatePointLihgt(glm::vec3(45, 4, -10)));


    // skybox 
    gl::Cubemap skybox("cubemaps/skybox/skybox.jpg");
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
    gl::Framebuffer shadow_map({ {2048, 2048} });

    float near_plane = 0.1f, far_plane = 100.0f;
    float left = -50, right = 50, bot = -50, top = 50;
    glm::mat4 lightProjection = glm::ortho(left, right, bot, top, near_plane, far_plane);
    
    // nanosuits positions
    std::array<glm::vec3, 5> suits_pos
    { glm::vec3(0.0f, -2.0f, 0.0f),
      glm::vec3(50.0f, -2.0f, 0.0f),
      glm::vec3(-50.0f, -2.0f, 0.0f),
      glm::vec3(0.0f, -2.0f, 50.0f),
      glm::vec3(0.0f, -2.0f, -50.0f),
    };

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
        glm::mat4 lightView = glm::lookAt(camera.m_Position + camera.m_Front * (top * 0.7f) + -lighters[0].direction * far_plane/2.0f,
                                          camera.m_Position + camera.m_Front * (top * 0.7f),
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

        view = camera.GetLookatMat();
        projection = camera.GetPprojectionMat(window.GetSize().x, window.GetSize().y);
        

        // ============================ shadows map ==============================
        //glCullFace(GL_FRONT);
        // create shadow map
        depthShader.setUniMat4("lightSpaceMatrix", lightProjection * lightView);
        shadow_map.Bind();
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        // plane
        //model = glm::translate(identity, glm::vec3(0.0f, -1.0f, 0.0f)); // translate it down so it's at the center of the scene
        //model = glm::scale(model, glm::vec3(0.01f));	// it's a bit too big for our scene, so scale it down
        //depthShader.setUniMat4("model", model);
        //plane.Draw(depthShader);

        // nanosuits
        for (int i = 0; i < 5; i++)
        {
            model = glm::scale(identity, glm::vec3(0.5f));	// it's a bit too big for our scene, so scale it down
            model = glm::translate(model, glm::vec3(suits_pos[i]));
            depthShader.setUniMat4("model", model);
            crysis.Draw(depthShader);
        }

        // box
        model = glm::scale(identity, glm::vec3(0.4f));
        model = glm::translate(model, glm::vec3(120, 0, 50));
        depthShader.setUniMat4("model", model);
        box.Draw(depthShader);

        shadow_map.Unbind();
        //glCullFace(GL_BACK);
        // ====================================================================

        
        // ========================== Render shadows in scene ========================================
        int down_scale_shadow_quality = 2;

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
        draw_shadow.setUni1i("shadowMap", 6);
        glBindTexture(GL_TEXTURE_2D, shadow_map.m_DepthAttachment);


        // plane
        model = glm::translate(identity, glm::vec3(0.0f, -1.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.03f));	// it's a bit too big for our scene, so scale it down
        draw_shadow.setUniMat4("model", model);
        plane.Draw(draw_shadow);


        glActiveTexture(GL_TEXTURE0 + 6);
        draw_shadow.setUni1i("shadowMap", 6);
        glBindTexture(GL_TEXTURE_2D, shadow_map.m_DepthAttachment);

        // nanosuits
        for (int i = 0; i < 5; i++)
        {
            model = glm::scale(identity, glm::vec3(0.5f));	// it's a bit too big for our scene, so scale it down
            model = glm::translate(model, suits_pos[i]);
            draw_shadow.setUniMat4("model", model);
            crysis.Draw(draw_shadow);
        }

        glActiveTexture(GL_TEXTURE0 + 6);
        draw_shadow.setUni1i("shadowMap", 6);
        glBindTexture(GL_TEXTURE_2D, shadow_map.m_DepthAttachment);

        // box
        model = glm::scale(identity, glm::vec3(0.4f));
        model = glm::translate(model, glm::vec3(120, 0, 50));
        draw_shadow.setUniMat4("model", model);
        box.Draw(draw_shadow);


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

        blinn_phong.setUni1i("normalmapping_flag", 0);

        // plane
        model = glm::translate(identity, glm::vec3(0.0f, -1.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.03f));	// it's a bit too big for our scene, so scale it down
        blinn_phong.setUniMat4("model", model);
        plane.Draw(blinn_phong);

        blinn_phong.setUni1i("normalmapping_flag", normal_mamming_flag);

        glActiveTexture(GL_TEXTURE0 + 6);
        blinn_phong.setUni1i("shadowMap", 6);
        glBindTexture(GL_TEXTURE_2D, shadow_raw.m_ColorAttachment);

        // nanosuits
        for (int i = 0; i < 5; i++)
        {
            model = glm::scale(identity, glm::vec3(0.5f));	// it's a bit too big for our scene, so scale it down
            model = glm::translate(model, suits_pos[i]);
            blinn_phong.setUniMat4("model", model);
            crysis.Draw(blinn_phong);
        }


        normal_mamming_flag = false;
        blinn_phong.setUni1i("normalmapping_flag", normal_mamming_flag);

        // box
        model = glm::scale(identity, glm::vec3(0.4f));
        model = glm::translate(model, glm::vec3(120, 0, 50));
        blinn_phong.setUniMat4("model", model);
        box.Draw(blinn_phong);


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