#define GLEW_DLL
#define GLFW_DLL
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Model.h"


// начальна€ позици€ камеры
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

// ѕозиции подвижных деталей станка
float tableX = 0.0f;   
float spindleHeadY = 0.0f;  
float saddleZ = 0.0f;        




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


	Shader myShader("ver_shader.glsl", "frag_shader.glsl");
	Model ourRTC("model.obj");


	glfwSetFramebufferSizeCallback(okno, framebufer_size_callback);
	glfwSetCursorPosCallback(okno, mouse_callback);
	glfwSetScrollCallback(okno, scroll_callback);
	glfwSetInputMode(okno, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	glm::vec3 Light_position = glm::vec3(0.0, 0.0, 1.0);


	// рендеринг
	while (!glfwWindowShouldClose(okno)) {
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		process_input(okno);

		float timeValue = glfwGetTime();
		delta_time = timeValue - last_frame;
		last_frame = timeValue;

		myShader.use();

		float period_red_value = sinf(timeValue * 0.7f + 0.0f) * 0.5f + 0.5f;
		float period_green_value = sinf(timeValue * 0.8f + 2.0f) * 0.5f + 0.5f;
		float period_blue_value = sinf(timeValue * 0.9f + 4.0f) * 0.5f + 0.5f;


		// ѕередаем позицию камеры дл€ бликов
		myShader.setVec3("Viewpos", camera_position.x, camera_position.y, camera_position.z);

		// Ќастройка света
		myShader.setVec3("light.position", 1.2f, 1.0f, 2.0f);
		myShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
		myShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
		myShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

		// Ќастройка материала
		myShader.setVec3("mat.ambient", 1.0f, 0.5f, 0.31f);
		myShader.setVec3("mat.diffuse", 1.0f, 0.5f, 0.31f);
		myShader.setVec3("mat.specular", 0.5f, 0.5f, 0.5f);

		glUniform1f(glGetUniformLocation(myShader.ID, "mat.shininess"), 32.0f);


		glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.0f, 100.0f);
		glm::mat4 view = glm::lookAt(camera_position, camera_position + camera_front, camera_up);
		glm::mat4 transform = glm::mat4(1.0f);

		myShader.setMat4("projection", projection);
		myShader.setMat4("view", view);
		myShader.setMat4("transform", transform);

		myShader.setVec4("uni_color", period_red_value, period_green_value, period_blue_value, 1.0f);


		// ќтрисовка модели (матрицы model и normalMatrix устанавливаютс€ внутри этого метода)
		ourRTC.Draw(myShader, tableX, spindleHeadY, saddleZ);


		glfwPollEvents();
		glfwSwapBuffers(okno);
	}

	glfwTerminate();
	return 0;
}


// перемещение с клавиатуры и управление детал€ми
void process_input(GLFWwindow* window) {
	const float camera_speed = 4.0f * delta_time;
	const float machine_speed = 2.0f * delta_time;

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

	// ƒвижение кожуха по X (стрелка влево / стрелка вправо)
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		tableX += machine_speed;
		if (tableX > 0.35f) tableX = 0.35f;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		tableX -= machine_speed;
		if (tableX < -0.25f) tableX = -0.25f;
	}

	// ƒвижение шпиндел€ (стрелка вверх / стрелка вниз)
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		spindleHeadY += machine_speed;
		if (spindleHeadY > 0.0f) spindleHeadY = 0.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		spindleHeadY -= machine_speed;
		if (spindleHeadY < -0.56f) spindleHeadY = -0.56f;
	}

	// ƒвижение кожуха по Z (R / F)
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		saddleZ -= machine_speed;
		if (saddleZ < 0.0f) saddleZ = 0.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		saddleZ += machine_speed;
		if (saddleZ > 0.6f) saddleZ = 0.6f;
	}
}


// движени€ мышью
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