
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

    gl::Shader crysis_shader("shaders/crysis.glsl");
    if (crysis_shader.invalid) return 1;


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
    lighters.push_back(gl::CreateDirLight(glm::normalize(glm::vec3(-0.2f, -0.2f, -0.5f))));

    // zbuffer anable
    glEnable(GL_DEPTH_TEST);

    gl::Timer timer;
    while (window.isOpen())
    {
        timer.Update();
        glfwPollEvents();

        // process input 
        ProcessCameraMovement(window, camera, timer.GetDeltaTime());

        // clear screen
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // update lighters
        phong.Use();

        gl::ApplyLightToShader(lighters, phong);

        view = camera.GetLookatMat();
        projection = camera.GetPprojectionMat(window.GetSize().x, window.GetSize().y);

        phong.setUni3f("viewPos", camera.m_Position);
        phong.setUniMat4("projection", projection);
        phong.setUniMat4("view", view);

        //crysis_shader.setUni3f("global_light.direction", glm::vec3(-0.2f, 0.1f, -0.5f));
        //crysis_shader.setUni3f("global_light.ambient", glm::vec3(0.3f));
        //crysis_shader.setUni3f("global_light.diffuse", glm::vec3(0.5f));
        //crysis_shader.setUni3f("global_light.specular", glm::vec3(0.8f));


        // render the loaded model
        model = glm::translate(identity, glm::vec3(0.0f, -1.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.01f));	// it's a bit too big for our scene, so scale it down
        phong.setUniMat4("model", model);

        ourModel.Draw(phong);

        
        model = glm::scale(identity, glm::vec3(0.5f));	// it's a bit too big for our scene, so scale it down
        model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
        phong.setUniMat4("model", model);

        crysis.Draw(phong);

        window.SwapBuffer();
    }

    return 0;
}