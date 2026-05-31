#define GLEW_DLL
#define GLFW_DLL
#include <iostream>
#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include <cmath>

int main() {
	if (!glfwInit()) {
		fprintf(stderr, "Error: could not start GLFW3.\n");
		return 1;
	}



	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 1);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	GLFWwindow* window = glfwCreateWindow(1024, 1024, "MainWindow", NULL, NULL);

	if (!window) {
		glfwTerminate();
		return -1;
	}




	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;

	GLenum ret = glewInit();

	if (GLEW_OK != ret) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(ret));
		return 1;
	}


	while (!glfwWindowShouldClose(window)) {

		glClearColor(1.0, 1.0, 1.0, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		glBegin(GL_POLYGON);
		glColor3f(0.0f, 1.0f, 0.0f);

		int vershini = 5;
		float radius = 0.5f;

		for (int i = 0; i < vershini; i++) {

			float ugol = 2.0f * 3.1415926f * (float)i / (float)vershini;
			float x = radius * cosf(ugol);
			float y = radius * sinf(ugol);

			glVertex2f(x, y);
		}

		glEnd();
		glfwPollEvents();
		glfwSwapBuffers(window);
	}
	glfwTerminate();

	return 0;
}