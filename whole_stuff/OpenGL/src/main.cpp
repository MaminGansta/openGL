
#include "opengl/gl_api.h"
#include "process_input/process_input.h"
#include <array>

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

    // lighters
    std::vector<gl::Light> lighters;
    lighters.push_back(gl::CreateDirLight(glm::normalize(glm::vec3(-0.2f, -0.2f, -0.5f))));
    //lighters.push_back(gl::CreatePointLihgt(glm::vec3(0.4, 1, 1)));
    //lighters.push_back(gl::CreatePointLihgt(glm::vec3(0.4, 4, -1)));
    //lighters.push_back(gl::CreatePointLihgt(glm::vec3(1, 5, 1)));


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
    unsigned int quadVAO = 0;
    unsigned int quadVBO;
    float quadVertices[] = {
        // positions        // texture Coords
        -1.0f,  0.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         0.0f,  0.0f, 0.0f, 1.0f, 1.0f,
         0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };
     // setup plane VAO
     glGenVertexArrays(1, &quadVAO);
     glGenBuffers(1, &quadVBO);
     glBindVertexArray(quadVAO);
     glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
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


    // shadow maping
    gl::Shader depthShader("shaders/depth_shader.glsl");
    gl::Framebuffer shadow_map({ {1024, 1024} });

    float near_plane = 0.1f, far_plane = 100.0f;
    float left = -40, right = 40, bot = -40, top = 40;
    glm::mat4 lightProjection = glm::ortho(left, right, bot, top, near_plane, far_plane);
    
    // nanosuits positions
    std::array<glm::vec3, 5> suits_pos
    { glm::vec3(0.0f, -2.0f, 0.0f),
      glm::vec3(50.0f, -2.0f, 0.0f),
      glm::vec3(-50.0f, -2.0f, 0.0f),
      glm::vec3(0.0f, -2.0f, 50.0f),
      glm::vec3(0.0f, -2.0f, -50.0f),
    };

    gl::Timer timer;
    while (window.isOpen())
    {
        timer.Update();
        glfwPollEvents();

        // process input 
        ProcessCameraMovement(window, camera, timer.GetDeltaTime());


        glm::vec3 pos = camera.m_Position;
        pos.y += 10.0f;
        
        glm::mat4 lightView = glm::lookAt(pos + camera.m_Front * right + -lighters[0].direction * far_plane/2.0f,
                                          pos + camera.m_Front * right,
                                          glm::vec3(0.0f, 1.0f, 0.0f));

        // clear screen
        //glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);

        // update lighters
        gl::ApplyLightToShader(lighters, blinn_phong);

        view = camera.GetLookatMat();
        projection = camera.GetPprojectionMat(window.GetSize().x, window.GetSize().y);

        

        // ============================ shadows ==============================
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
        shadow_map.Unbind();
        // ====================================================================


        glCullFace(GL_BACK);
        window.UseViewPort();
        
        blinn_phong.setUni3f("viewPos", camera.m_Position);
        blinn_phong.setUniMat4("projection", projection);
        blinn_phong.setUniMat4("view", view);
        blinn_phong.setUniMat4("lightSpaceMatrix", lightProjection* lightView);

        glActiveTexture(GL_TEXTURE0 + 6);
        blinn_phong.setUni1i("shadowMap", 6);
        glBindTexture(GL_TEXTURE_2D, shadow_map.m_DepthAttachment);

        blinn_phong.setUni1i("normalmapping_flag", 0);

        // plane
        model = glm::translate(identity, glm::vec3(0.0f, -1.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.03f));	// it's a bit too big for our scene, so scale it down
        blinn_phong.setUniMat4("model", model);
        plane.Draw(blinn_phong);
        
        static float input_delay = 0.3f;
        input_delay -= timer.m_DeltaTime;
        if (input_delay < 0.0f && gl::clicked(GLFW_KEY_N))
        {
            normal_mamming_flag ^= true;
            input_delay = 0.3f;
        }
        blinn_phong.setUni1i("normalmapping_flag", normal_mamming_flag);
        

        glActiveTexture(GL_TEXTURE0 + 6);
        blinn_phong.setUni1i("shadowMap", 6);
        glBindTexture(GL_TEXTURE_2D, shadow_map.m_DepthAttachment);

        // nanosuits
        for (int i = 0; i < 5; i++)
        {
            model = glm::scale(identity, glm::vec3(0.5f));	// it's a bit too big for our scene, so scale it down
            model = glm::translate(model, suits_pos[i]);
            blinn_phong.setUniMat4("model", model);
            crysis.Draw(blinn_phong);
        }


        //// draw normals
        //show_normals.setUniMat4("model", model);
        //show_normals.setUniMat4("projection", projection);
        //show_normals.setUniMat4("view", view);
        //crysis.Draw(show_normals);

        
        // draw skybox
        glDepthMask(GL_FALSE);
        skyboxShader.Use();

        skyboxShader.setUniMat4("projection", projection);
        skyboxShader.setUniMat4("view", glm::mat3(view));
        glBindVertexArray(skyboxVAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.textureID);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);


        // draw shadowmap texture
        image_shader.Use();
        glActiveTexture(GL_TEXTURE0 + 6);
        image_shader.setUni1i("depthMap", 6);
        glBindTexture(GL_TEXTURE_2D, shadow_map.m_ColorAttachment);

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);



        window.SwapBuffer();
    }

    return 0;
}