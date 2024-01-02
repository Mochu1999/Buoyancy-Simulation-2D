#pragma once



#include "shader.h"
#include <chrono>
using namespace chrono;

#pragma once
#define PI 3.14159265358979323846	//constexpr?

extern float windowHeight = 1080;
extern float windowWidth = 1920;


//cout << "positions: " << endl;
//for (int i = 0; i < positions.size(); i += 2) {
//	cout << positions[i] << ", " << positions[i + 1] << "," << endl;
//}cout << endl;
//cout << "indices" << endl;
//for (unsigned int i = 0; i < indices.size(); i++) {
//	cout << indices[i] << ", ";
//}cout << endl;


//Esto es de old polygons
float absoluteAngle(float x, float y) {
	/*if (x == 0 && y == 0) {
		std::cout << "Error triggered: 0/0 is undefined" << std::endl;
		std::exit(EXIT_FAILURE);
	}*/

	float angleInRadians = atan2(y, x);
	float angleInDegrees = angleInRadians * 180 / PI;


	if (angleInDegrees < 0) {
		angleInDegrees += 360;
	}

	return angleInDegrees;
}

bool isConcave(float angle1, float angle2) {
	float angleDifference = angle2 - angle1;

	while (angleDifference < 0) {
		angleDifference += 360;
	}
	while (angleDifference >= 360) {
		angleDifference -= 360;
	}

	return angleDifference <= 180;
}

bool checkBarycentric(float x, float y, float x1, float y1, float x2, float y2, float x3, float y3) {

	float alpha = ((y2 - y3) * (x - x3) + (x3 - x2) * (y - y3)) / ((y2 - y3) * (x1 - x3) + (x3 - x2) * (y1 - y3)); //meassures the influence of x1,k1, if it were 1 and the rest  0 x,y would be on x1,y1
	float beta = ((y3 - y1) * (x - x3) + (x1 - x3) * (y - y3)) / ((y2 - y3) * (x1 - x3) + (x3 - x2) * (y1 - y3));
	float gamma = 1 - alpha - beta;
	if (alpha >= 0 && alpha <= 1 && beta >= 0 && beta <= 1 && gamma >= 0 && gamma <= 1) //if any of them is greater than 1 or less than 1 they won't be inside the triangle
		return true;	//it is inside
	else
		return false;

}
//bool checkBarycentric(float x, float y, float x1, float y1, float x2, float y2, float x3, float y3) {
//	float epsilon = 1e-2; // Tolerance value, can be adjusted based on your precision requirements
//
//	float denominator = (y2 - y3) * (x1 - x3) + (x3 - x2) * (y1 - y3);
//	if (std::abs(denominator) < epsilon) {
//		return false; // The triangle is degenerate (points are collinear or too close)
//	}
//
//	float alpha = ((y2 - y3) * (x - x3) + (x3 - x2) * (y - y3)) / denominator;
//	float beta = ((y3 - y1) * (x - x3) + (x1 - x3) * (y - y3)) / denominator;
//	float gamma = 1 - alpha - beta;
//
//	return (alpha >= -epsilon && alpha <= 1 + epsilon) &&
//		(beta >= -epsilon && beta <= 1 + epsilon) &&
//		(gamma >= -epsilon && gamma <= 1 + epsilon);
//}

//bool checkBarycentric(float x, float y, float x1, float y1, float x2, float y2, float x3, float y3) {		//suposed to be faster but not checked
//	// Inline cross product calculation and check for each edge
//	// Edge 1 (v0: x1->x2, y1->y2)
//	float c1 = (x2 - x1) * (y - y1) - (y2 - y1) * (x - x1);
//	// Edge 2 (v1: x1->x3, y1->y3)
//	float c2 = (x3 - x1) * (y - y1) - (y3 - y1) * (x - x1);
//	// If c1 and c2 are not on the same side, return false early
//	if ((c1 < 0 || c2 < 0) && (c1 > 0 || c2 > 0)) return false;
//	// Edge 3 (v2: x3->x2, y3->y2)
//	float c3 = (x2 - x3) * (y - y3) - (y2 - y3) * (x - x3);
//	// Final check
//	return (c1 >= 0 && c2 >= 0 && c3 >= 0) || (c1 <= 0 && c2 <= 0 && c3 <= 0);
//}


//void hop(float& xpos, float& ypos) {
//
//	if (abs(xpos - positions[0]) <= 20 && abs(ypos - positions[1]) <= 20) {
//		xpos = positions[0];
//		ypos = positions[1];
//	}
//	positions[positions.size() - 2] = xpos;
//	positions.back() = ypos;
//
//}






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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

void printMatrix(const std::vector<float>& matrix, const std::string& matrixName, int rows, int cols) {
	if (matrix.size() != rows * cols) {
		std::cout << "Invalid matrix size. Should be " << rows * cols << " elements for a " << rows << "x" << cols << " matrix." << std::endl;
		return;
	}
	std::cout << matrixName << std::endl;

	for (int row = 0; row < rows; ++row) {
		for (int col = 0; col < cols; ++col) {
			std::cout << matrix[row * cols + col] << "\t";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}


//this is in my weird format
int binarySearchX(float& indexToFind, vector<float>& range) {
	int start = 0, end = range.size() / 2 - 1;//-1 to be an x coordinate
	//defines the current search range, they define all the possible range at the start

	int output;

	// Binary search for firstX
	while (start <= end)
	{
		int mid = start + (end - start) / 2;
		if (range[2 * mid] >= indexToFind)
		{
			output = 2 * mid;
			end = mid - 1;
		}
		else
		{
			start = mid + 1;
		}
	}
	return output;
}

void timeMeThis()
{
	high_resolution_clock::time_point time0 = high_resolution_clock::now();
	high_resolution_clock::time_point time1 = high_resolution_clock::now();
	float deltaTime = duration_cast<duration<float>>(time1 - time0).count();
	cout << "deltaTime: " << deltaTime << " s" << endl;
}


