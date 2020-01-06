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

#include "shaders/shaders.cpp"
#include "glstuff.cpp"
#include "camera.cpp"
#include "buffersAbstraction.cpp"


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

    // preapare shaders
    int cubeShader, lighterShader;
    if (!make_program("shaders/cube.shader", cubeShader))
        return 1;
    if (!make_program("shaders/lighter.shader", lighterShader))
        return 1;

    // coube vertices with normals
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };


    VertexBuffer cubeBuffer(vertices, sizeof(vertices), GL_STATIC_DRAW);

   VertexArray cubeVA;
    VertexBufferLayout layout;
    layout.push<float>(3);
    layout.push<float>(3);
    cubeVA.addBuffer(cubeBuffer, layout);
  
    // bind lighter VAO
    VertexArray lighterVA;
    VertexBufferLayout lighterLayout;
    lighterLayout.push<float>(3);
    lighterLayout.push<float>(3);
    lighterVA.addBuffer(cubeBuffer, lighterLayout);

    // cube and lighter position 
    glm::vec3 lightPos(2.5f, 0.0f, -6.0f);
    glm::vec3 cube_pos(0.0f, -1.0f, -4.5f);

    // uniforms
    glUseProgram(cubeShader);
    // cube vertex
    GLint viewPosLoc = glGetUniformLocation(cubeShader, "viewPos");
    int uni_cube_modal = glGetUniformLocation(cubeShader, "model");
    int uni_cube_veiw = glGetUniformLocation(cubeShader, "view");
    int uni_cube_proj = glGetUniformLocation(cubeShader, "projection");
    // fragment
    int uni_objectColorLoc = glGetUniformLocation(cubeShader, "objectColor");
    int uni_lightColorLoc = glGetUniformLocation(cubeShader, "lightColor");
    int uni_lightPos = glGetUniformLocation(cubeShader, "lightPos");
    glUniform3f(uni_objectColorLoc, 1.0f, 0.5f, 0.31f);
    glUniform3f(uni_lightColorLoc, 1.0f, 1.0f, 1.0f); // light color


    glUseProgram(lighterShader);
    // lighter vertex
    int uni_lighter_modal = glGetUniformLocation(lighterShader, "model");
    int uni_lighter_veiw = glGetUniformLocation(lighterShader, "view");
    int uni_lighter_proj = glGetUniformLocation(lighterShader, "projection");


    // zbuffer anable
    glEnable(GL_DEPTH_TEST);

    // some matrix staff
    glm::mat4 identity(1.0f);
    glm::mat4 projection = glm::perspective<float>(PI / 4, (float)window_width / window_height, 0.1f, 100.0f);
    glm::mat4 model(1.0f);
    glm::mat4 view(1.0f);

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
        glUseProgram(lighterShader);
        // lighter calculation
        model = glm::translate(identity, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));

        // set uniform params
        glUniformMatrix4fv(uni_lighter_modal, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uni_lighter_veiw, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(uni_lighter_proj, 1, GL_FALSE, glm::value_ptr(projection));
        
        // draw lighter
        //glBindVertexArray(lighterVAO);
        lighterVA.bind();
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        
        // cube ------------------------------------------
        glUseProgram(cubeShader);

        // set uniforms
        glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);

        // set position
        model = glm::translate(identity, cube_pos);

        // set uniform params
        glUniformMatrix4fv(uni_cube_modal, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uni_cube_veiw, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(uni_cube_proj, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3f(uni_objectColorLoc, 1.0f, 0.5f, 0.31f);
        glUniform3f(uni_lightColorLoc, 1.0f, 1.0f, 1.0f); // light color
        glUniform3f(uni_lightPos, lightPos.x, lightPos.y, lightPos.z); // light pos

        // draw cube
        //glBindVertexArray(cubeVAO);
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