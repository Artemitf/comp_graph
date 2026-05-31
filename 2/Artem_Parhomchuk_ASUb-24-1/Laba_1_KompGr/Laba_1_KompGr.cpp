#define GLEW_DLL
#define GLFW_DLL
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>



const char* vertex_shader_source =
"#version 410 core\n"
"layout (location = 0) in vec3 vp;\n"
"void main() {\n"
"    gl_Position = vec4(vp, 1.0);\n"
"}\n";


const char* fragment_shader_source =
"#version 410 core\n"
"out vec4 frag_colour;\n"
"uniform vec4 ourColor;\n"
"void main() {\n"
"    frag_colour = ourColor;\n"
"}\n";




int main() {
    if (!glfwInit()) {
        fprintf(stderr, "Error: could not start GLFW3.\n");
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* okno = glfwCreateWindow(1024, 1024, "MainWindow", NULL, NULL);
    if (!okno) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(okno);
    glewExperimental = GL_TRUE;

    GLenum ret = glewInit();
    if (GLEW_OK != ret) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(ret));
        return 1;
    }


    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader_source, NULL);
    glCompileShader(vs);


    GLint success;
    GLchar infoLog[1024];
    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vs, 1024, NULL, infoLog);
        std::cout << "ERROR: VERTEX SHADER COMPILATION_ERROR\n" << infoLog << "\n";
    }


    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader_source, NULL);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(fs, 1024, NULL, infoLog);
        std::cout << "ERROR: FRAGMENT SHADER COMPILATION_ERROR\n" << infoLog << "\n";
    }


    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vs);
    glAttachShader(shader_program, fs);
    glLinkProgram(shader_program);


    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, 1024, NULL, infoLog);
        std::cout << "ERROR: SHADER PROGRAM LINK_ERROR\n" << infoLog << "\n";
    }


    glDeleteShader(vs);
    glDeleteShader(fs);


    const int vershini = 5;
    float radius = 0.5f;


    float points[vershini * 3];
    for (int i = 0; i < vershini; i++) {
        float ugol = 2.0f * 3.1415926f * (float)i / (float)vershini;
        points[i * 3] = radius * cosf(ugol);
        points[i * 3 + 1] = radius * sinf(ugol);
        points[i * 3 + 2] = 0.0f;
    }


    GLuint indices[] = {
        0, 1, 2,
        0, 2, 3,
        0, 3, 4
    };
    int total_indices = sizeof(indices) / sizeof(indices[0]);





    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);



    while (!glfwWindowShouldClose(okno)) {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        glUseProgram(shader_program);

        float timeValue = glfwGetTime();
        float greenValue = (sinf(timeValue) / 2.0f) + 0.5f;
        float blueValue = (cosf(timeValue) / 2.0f) + 0.5f;


        GLint vertexColorLocation = glGetUniformLocation(shader_program, "ourColor");
        glUniform4f(vertexColorLocation, 0.0f, greenValue, blueValue, 1.0f);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, total_indices, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwPollEvents();
        glfwSwapBuffers(okno);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shader_program);

    glfwTerminate();
    return 0;
}