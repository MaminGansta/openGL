
#include "opengl/gl_api.h"


int main(void)
{
    gl::Window window("OpenGL");

    // camera
    gl::Camera camera;

    // preapare shaders
    gl::Shader crysis_shader("shaders/crysis.shader");
    if (crysis_shader.invalid) return 1;


    // cube and lighter position
    glm::vec3 lightPos(2.5f, 0.0f, -6.0f);
    glm::vec3 cube_pos(0.0f, 0.0f, -3.5f);
    glm::vec3 light_dir(-0.2f, -1.0f, -0.3f);

    // some matrix staff
    glm::mat4 identity(1.0f);
    glm::mat4 projection = glm::perspective<float>(gl::PI / 4, (float)window.GetSize().x / window.GetSize().y, 0.1f, 100.0f);
    glm::mat4 model(1.0f);
    glm::mat4 view(1.0f);


    // model
    gl::Model crysis("models/crysis/nanosuit.obj");

    gl::Model ourModel("models/plane/scene.gltf");

    // zbuffer anable
    glEnable(GL_DEPTH_TEST);

    while (window.isOpen())
    {
        // time
        float now = glfwGetTime();
        gl::delta_time = now - gl::last_time;
        gl::last_time = now;

        glfwPollEvents();

        // process input 
        camera.UpdateCameraVectors();
        view = camera.GetLookatMat();
        projection = camera.GetPprojectionMat(window.GetSize().x, window.GetSize().y);

        // clear screen
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        crysis_shader.setUniMat4("projection", projection);
        crysis_shader.setUniMat4("view", view);

        // render the loaded model
        model = glm::translate(identity, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.01f));	// it's a bit too big for our scene, so scale it down
        crysis_shader.setUniMat4("model", model);

        crysis_shader.setUni3f("view_pos", camera.m_Position);
        crysis_shader.setUni3f("global_light.direction", glm::vec3(-0.2f, 0.1f, -0.5f));
        crysis_shader.setUni3f("global_light.ambient", glm::vec3(0.3f));
        crysis_shader.setUni3f("global_light.diffuse", glm::vec3(0.5f));
        crysis_shader.setUni3f("global_light.specular", glm::vec3(0.8f));

        ourModel.Draw(crysis_shader);


        model = glm::translate(identity, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.4f));	// it's a bit too big for our scene, so scale it down
        crysis_shader.setUniMat4("model", model);

        crysis.Draw(crysis_shader);

        window.SwapBuffer();
    }

    // clear the space
    glfwTerminate();
    return 0;
}