#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#define PI 3.14159265359f

template <typename T>
int sgn(T num) { return (num > T(0)) - (num < T(0)); }

bool input[512];
bool pressed(int code)
{
    return input[code];
}

#include "shaders/shaders.cpp"
#include "camera.cpp"


static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_W)
        input[GLFW_KEY_W] = action;
    if (key == GLFW_KEY_S)
        input[GLFW_KEY_S] = action;
    if (key == GLFW_KEY_A)
        input[GLFW_KEY_A] = action;
    if (key == GLFW_KEY_D)
        input[GLFW_KEY_D] = action;
    if (key == GLFW_KEY_D)
        input[GLFW_KEY_D] = action;

    if (key == GLFW_KEY_UP)
        input[GLFW_KEY_UP] = action;
    if (key == GLFW_KEY_DOWN)
        input[GLFW_KEY_DOWN] = action;
    if (key == GLFW_KEY_LEFT)
        input[GLFW_KEY_LEFT] = action;
    if (key == GLFW_KEY_RIGHT)
        input[GLFW_KEY_RIGHT] = action;

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

    // set callback function
    glfwSetKeyCallback(window, key_callback);

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

    uint32_t lighterVAO, cubeVAO, VBO;
    glGenVertexArrays(1, &lighterVAO);
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    // bind cube VAO
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    // bind lighter VAO
    glBindVertexArray(lighterVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

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
    glUniform3f(uni_lightPos, lightPos.x, lightPos.y, lightPos.z); // light pos


    glUseProgram(lighterShader);
    // lighter vertex
    int uni_lighter_modal = glGetUniformLocation(lighterShader, "model");
    int uni_lighter_veiw = glGetUniformLocation(lighterShader, "view");
    int uni_lighter_proj = glGetUniformLocation(lighterShader, "projection");


    // zbuffer anable
    glEnable(GL_DEPTH_TEST);

    float last_time = 0;
    float delta_time = 0;

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
        camera.update(delta_time);
        view = camera.GetViewMatrix();

        // move the lighter
        float radius = 5.0f;

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
        glBindVertexArray(lighterVAO);
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

        // draw cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);


        glfwSwapBuffers(window);
    }

    // clear the space
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lighterVAO);
    glDeleteBuffers(1, &VBO);

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
    return 0;
}