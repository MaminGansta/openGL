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
    Texture emission("matrix.jpg");
    if (emission.invalid) return 1;


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

    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
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
    glm::vec3 light_dir(-0.2f, -1.0f, -0.3f); 

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


        // clear screen
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        // cubes ------------------------------------------
        cube_shader.use();
        // bind the texture
        box_deff.bind(cube_shader, "material.diffuse", 0);
        box_spec.bind(cube_shader, "material.specular", 1);
        //emission.bind(cube_shader, "material.emission", 2);
        // set uniform params
        cube_shader.setUniMat4("view", view);
        cube_shader.setUniMat4("projection", projection);// draw cube
        cube_shader.setUni3f("light.direction", light_dir);

        cubeVA.bind();
        for (int i = 0; i < 10; i++)
        {
            model = glm::translate(identity, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
            cube_shader.setUniMat4("model", model);
            glcall(glDrawArrays(GL_TRIANGLES, 0, 36));
        }

        glfwSwapBuffers(window);
    }

    // clear the space

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
    return 0;
}