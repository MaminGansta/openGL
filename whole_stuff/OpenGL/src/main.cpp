
#include "opengl/gl_api.h"
#include "process_input/process_input.h"

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

    gl::Shader crysis_shader("shaders/crysis.glsl");
    if (crysis_shader.invalid) return 1;

    gl::Shader show_normals("shaders/show_normals.glsl");
    if (show_normals.invalid) return 1;

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
    gl::Model ourModel("models/plane/scene.gltf");

    // lighters
    std::vector<gl::Light> lighters;
    //lighters.push_back(gl::CreateDirLight(glm::normalize(glm::vec3(-0.2f, -0.2f, -0.5f))));
    lighters.push_back(gl::CreatePointLihgt(glm::vec3(0.4, 1, 1)));
    lighters.push_back(gl::CreatePointLihgt(glm::vec3(0.4, 4, -1)));
    lighters.push_back(gl::CreatePointLihgt(glm::vec3(1, 5, 1)));


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


    // zbuffer anable
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);

    bool normal_mamming_flag = false;

    gl::Timer timer;
    while (window.isOpen())
    {
        timer.Update();
        glfwPollEvents();

        // process input 
        ProcessCameraMovement(window, camera, timer.GetDeltaTime());

        // clear screen
        //glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);

        view = camera.GetLookatMat();
        projection = camera.GetPprojectionMat(window.GetSize().x, window.GetSize().y);


        // update lighters
        blinn_phong.Use();
        
        gl::ApplyLightToShader(lighters, blinn_phong);

        blinn_phong.setUni3f("viewPos", camera.m_Position);
        blinn_phong.setUniMat4("projection", projection);
        blinn_phong.setUniMat4("view", view);

        blinn_phong.setUni1i("normalmapping_flag", 0);

        // plane
        model = glm::translate(identity, glm::vec3(0.0f, -1.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.01f));	// it's a bit too big for our scene, so scale it down
        blinn_phong.setUniMat4("model", model);
        ourModel.Draw(blinn_phong);

        
        static float input_delay = 0.3f;
        input_delay -= timer.m_DeltaTime;
        
        if (input_delay < 0.0f && gl::clicked(GLFW_KEY_N))
        {
            normal_mamming_flag ^= true;
            input_delay = 0.3f;
        }
        blinn_phong.setUni1i("normalmapping_flag", normal_mamming_flag);
        
        
        // nanosuit
        model = glm::scale(identity, glm::vec3(0.5f));	// it's a bit too big for our scene, so scale it down
        model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
        blinn_phong.setUniMat4("model", model);
        crysis.Draw(blinn_phong);

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

        window.SwapBuffer();
    }

    return 0;
}