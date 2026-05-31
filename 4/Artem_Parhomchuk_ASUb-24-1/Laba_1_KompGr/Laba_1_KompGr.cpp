#define GLEW_DLL
#define GLFW_DLL
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


// начальная позиция камеры
glm::vec3 camera_position = glm::vec3(0.0, 0.0, 5.0);
glm::vec3 camera_front = glm::vec3(0.0, 0.0, -1.0);
glm::vec3 camera_up = glm::vec3(0.0, 1.0, 0.0);

// настройки мыши
void framebufer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* winodow, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// размер окна
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 1024;
float last_x = SCR_WIDTH / 2.0f;
float last_y = SCR_HEIGHT / 2.0f;

// вращение камеры
bool first_mouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float fov = 45.0f;


float delta_time = 0.0f;
float last_frame = 0.0f;

void process_input(GLFWwindow* window);


// вершинный шейдер
const char* vertex_shader_source =
"#version 410 core\n"
"layout (location = 0) in vec3 vp;\n"
"uniform mat4 transform;\n"
"void main() {\n"
"    gl_Position = transform * vec4(vp, 1.0);\n"
"}\n";


// фрагментный шейдер
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

    // создание окна
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


    glEnable(GL_DEPTH_TEST);

    // создание вершинного шейдера
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


    // создание фрагментного шейдера
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader_source, NULL);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(fs, 1024, NULL, infoLog);
        std::cout << "ERROR: FRAGMENT SHADER COMPILATION_ERROR\n" << infoLog << "\n";
    }


    // линовка шейдеров
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


    // пятиугольник
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


    // генерация и настройка объектов памяти
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


    glfwSetFramebufferSizeCallback(okno, framebufer_size_callback);
    glfwSetCursorPosCallback(okno, mouse_callback);
    glfwSetScrollCallback(okno, scroll_callback);
    glfwSetInputMode(okno, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    // рендеринг
    while (!glfwWindowShouldClose(okno)) {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        process_input(okno);

        float timeValue = glfwGetTime();
        delta_time = timeValue - last_frame;
        last_frame = timeValue;

        glUseProgram(shader_program);

        float greenValue = (sinf(timeValue) / 2.0f) + 0.5f;
        float blueValue = (cosf(timeValue) / 2.0f) + 0.5f;


        GLint vertexColorLocation = glGetUniformLocation(shader_program, "ourColor");
        glUniform4f(vertexColorLocation, 0.0f, greenValue, blueValue, 1.0f);


        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(camera_position, camera_position + camera_front, camera_up);

        // вращение фигуры
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, timeValue * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));

        glm::mat4 transform = projection * view * model;

        GLint transformLoc = glGetUniformLocation(shader_program, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));


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


// перемещение с клавиатуры
void process_input(GLFWwindow* window) {
    const float camera_speed = 4.0f * delta_time;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera_position += camera_speed * camera_front;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera_position -= camera_speed * camera_front;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera_position -= glm::normalize(glm::cross(camera_front, camera_up)) * camera_speed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera_position += glm::normalize(glm::cross(camera_front, camera_up)) * camera_speed;
    }
}



// движения мышью
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    float x_pos = static_cast<float>(xpos);
    float y_pos = static_cast<float>(ypos);

    if (first_mouse) {
        last_x = x_pos;
        last_y = y_pos;
        first_mouse = false;
    }

    float xoffset = x_pos - last_x;
    float yoffset = last_y - y_pos;
    last_x = x_pos;
    last_y = y_pos;

    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    camera_front = glm::normalize(front);
}

// колесо мыши
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    fov -= (float)yoffset;
    if (fov < 1.0f) fov = 1.0f;
    if (fov > 45.0f) fov = 45.0f;
}

// изменение размеров окна
void framebufer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}