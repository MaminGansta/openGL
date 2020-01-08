#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstdint>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>

#include "glstuff.cpp"
#include "shaders.cpp"
#include "camera.cpp"
#include "buffersAbstraction.cpp"
//#include "texture.cpp"
#include "mesh.cpp"
#include "model.cpp"




int main(void)
{
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // set errorr callback
    glfwSetErrorCallback(error_callback);

    GLFWwindow* window = glfwCreateWindow(800, 600, "light", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);

    // set callback functions
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetCursorPos(window, lastX, lastY);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //glfwSwapInterval(1);

    // set glew
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        printf("GLEW INIT FAIL");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // veiwport
    int window_width, window_height;
    glfwGetFramebufferSize(window, &window_width, &window_height);
    glViewport(0, 0, window_width, window_height);

    // camera
    Camera camera;

    // preapare shaders
    Shader crysis_shader("shaders/crysis.shader");
    if (crysis_shader.invalid) return 1;


    // cube and lighter position
    glm::vec3 lightPos(2.5f, 0.0f, -6.0f);
    glm::vec3 cube_pos(0.0f, 0.0f, -3.5f);
    glm::vec3 light_dir(-0.2f, -1.0f, -0.3f);

    // some matrix staff
    glm::mat4 identity(1.0f);
    glm::mat4 projection = glm::perspective<float>(PI / 4, (float)window_width / window_height, 0.1f, 100.0f);
    glm::mat4 model(1.0f);
    glm::mat4 view(1.0f);


    // model
    Model ourModel("models/crysis/nanosuit.obj");


    // zbuffer anable
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        // time
        float now = glfwGetTime();
        delta_time = now - last_time;
        last_time = now;

        glfwPollEvents();

        // process input 
        camera.update();
        view = camera.GetViewMatrix();

        // move the lighter
        //float radius = 4.5f;
        //lightPos.x = cube_pos.x + sinf(now) * radius;
        //lightPos.z = cube_pos.z + cosf(now) * radius;

        // clear screen
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        crysis_shader.setUniMat4("projection", projection);
        crysis_shader.setUniMat4("view", view);

        // render the loaded model
        model = glm::translate(identity, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
        crysis_shader.setUniMat4("model", model);
        ourModel.Draw(crysis_shader);


        glfwSwapBuffers(window);
    }

    // clear the space
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
    return 0;
}