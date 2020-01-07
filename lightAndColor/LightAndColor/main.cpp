#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

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
#include "shaders/shaders.cpp"
#include "camera.cpp"
#include "buffersAbstraction.cpp"
#include "texture.cpp"


static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

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

    // textures (deffuse)
    Texture box_deff("container_diff.png");
    if (box_deff.invalid) return 1;
    Texture box_spec("container_spec.png");
    if (box_spec.invalid) return 1;


    // preapare shaders
    Shader cube_shader("shaders/cube.shader");
    if (cube_shader.invalid) return 1;

    Shader light_shader("shaders/lighter.shader");
    if (light_shader.invalid) return 1;


    // coube vertices with normals
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    VertexBuffer cubeBuffer(vertices, sizeof(vertices), GL_STATIC_DRAW);

    VertexArray cubeVA;
    VertexBufferLayout layout;
    layout.push<float>(3);   // position
    layout.push<float>(3);   // normals
    layout.push<float>(2);   // texture coordinates
    cubeVA.addBuffer(cubeBuffer, layout);
  
    // bind lighter VAO
    VertexArray lighterVA;
    VertexBufferLayout lighterLayout;
    lighterLayout.push<float>(3);
    lighterLayout.push<float>(3);  // we use the same buffer but only his position
    lighterLayout.push<float>(2);
    lighterVA.addBuffer(cubeBuffer, lighterLayout);

    // cube and lighter position 
    glm::vec3 lightPos(2.5f, 0.0f, -6.0f);
    glm::vec3 cube_pos(0.0f, -1.0f, -4.5f);

    // uniforms
    // cube shader's uniforms
    // fragment light structure
    glm::vec3(0.5f, 0.5f, 0.5f); 
    glm::vec3(0.2f, 0.2f, 0.2f);

    cube_shader.setUni3f("light.ambient", glm::vec3(0.2f));
    cube_shader.setUni3f("light.diffuse", glm::vec3(0.4f));
    cube_shader.setUni3f("light.specular", glm::vec3(1.0f));
    
    // fragment material structure
    cube_shader.setUni1f("material.shininess", 64.0f);

    // some matrix staff
    glm::mat4 identity(1.0f);
    glm::mat4 projection = glm::perspective<float>(PI / 4, (float)window_width / window_height, 0.1f, 100.0f);
    glm::mat4 model(1.0f);
    glm::mat4 view(1.0f);


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
        float radius = 3.0f;
        lightPos.x = cube_pos.x + sinf(now) * radius;
        lightPos.z = cube_pos.z + cosf(now) * radius;

        // clear screen
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // lighter ---------------------------------------
        light_shader.use();
        // lighter calculation
        model = glm::translate(identity, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));

        // set uniform params
        light_shader.setUniMat4("model", model);
        light_shader.setUniMat4("view", view);
        light_shader.setUniMat4("projection", projection);

        // draw lighter
        lighterVA.bind();
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        
        // cube ------------------------------------------
        cube_shader.use();
        // bind the texture
        box_deff.bind(cube_shader, 0);
        box_spec.bind(cube_shader, 1);
        // set position
        model = glm::translate(identity, cube_pos);

        // set uniform params
        cube_shader.setUniMat4("model", model);
        cube_shader.setUniMat4("view", view);
        cube_shader.setUniMat4("projection", projection);// draw cube
        cube_shader.setUni3f("lightPos", lightPos);

        cubeVA.bind();
        glcall(glDrawArrays(GL_TRIANGLES, 0, 36));
        glBindVertexArray(0);


        glfwSwapBuffers(window);
    }

    // clear the space

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
    return 0;
}