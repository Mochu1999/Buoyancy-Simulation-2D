#pragma once



#include "shader.h"


#pragma once
#define PI 3.14159265358979323846	//constexpr?

extern float windowHeight = 1000;
extern float windowWidth = 1000;


#define CHECK_GL_ERROR() {\
    GLenum err = glGetError();\
    while (err != GL_NO_ERROR) {\
        std::cout << "OpenGL error: " << std::hex << err << std::dec << " line:" << __LINE__ << std::endl;\
        err = glGetError();\
    }\
}
std::set<std::string> printedErrors;

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) { //Only adds errors once
	std::string errorMsg(message);

	if (printedErrors.find(errorMsg) == printedErrors.end()) {
		std::cerr << message << std::endl;

		printedErrors.insert(errorMsg);
	}
}


GLFWwindow* initialize() {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_SAMPLES, 4);
	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Buoyancy Simulation 2D", NULL, NULL);

	glEnable(GL_MULTISAMPLE);

	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	glewInit();
	glfwSwapInterval(1);
	return window;
}


void displayMatrix(const std::vector<float>& matrix, int rows, int cols) {
	for (int row = 0; row < rows; ++row) {
		for (int col = 0; col < cols; ++col) {
			std::cout << matrix[row * cols + col] << "\t";
		}
		std::cout << std::endl;
	}
}






