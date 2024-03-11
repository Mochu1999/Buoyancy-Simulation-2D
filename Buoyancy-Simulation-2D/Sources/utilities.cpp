#pragma once






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



//A ver, donde necesitas esto? Porque si no necesitas estrictamente angulos hazlo con el producto vectorial
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

//}






#define CHECK_GL_ERROR() {\
    GLenum err = glGetError();\
    while (err != GL_NO_ERROR) {\
        std::cout << "OpenGL error: " << std::hex << err << std::dec << " line:" << __LINE__ << std::endl;\
        err = glGetError();\
    }\
}
std::set<std::string> printedErrors;

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar * message, const void* userParam) { //Only adds errors once
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


void displayMatrix(const std::vector<float>&matrix, int rows, int cols) {
	for (int row = 0; row < rows; ++row) {
		for (int col = 0; col < cols; ++col) {
			std::cout << matrix[row * cols + col] << "\t";
		}
		std::cout << std::endl;
	}
}

void printMatrix(const std::vector<float>&matrix, const std::string & matrixName, int rows, int cols) {
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
int binarySearchX(float& indexToFind, vector<float>&range) {
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



	

/*
		concave arrow {200,200,600,100,500,400,450,250,400,200,200,200}
	 goliath	//{ 100,400,150,350,250,350,250,250,150,250,100,200,150,150,250,150,500,300,250,450,150,450,100,400 }
	improper start { 100,400,150,200,250,225,175,175,500,300,200,400,400,500,450,450,520,520,450,540,530,580,400,580,430,550,150,600,100,400 }
	

		//gato
	{ 100,100,900,100,950,350,820,460,700,350,800,200,600,400,800,600,500,800, 150,600, 400,400,500,200,100,100 }

		//espiral
		//{ 400, 600, 350, 600, 350, 350, 650, 350, 650, 750, 250, 750, 250, 250, 750, 250, 750, 600, 700, 600, 700, 300, 300, 300, 300, 700, 600, 700, 600, 400, 400, 400, 400, 600 }

		//semi espiral
		//{  350, 600, 350, 350, 650, 350, 650, 750, 600, 700, 600, 400, 400, 400, 400, 600 ,350, 600 }

		//complejidad
		//{ 100,700,100,400,300,400,350,350,400,400,400,600,600,600,600,400,650,350,700,400,700,700,360,700,360,450,340,450,340,700,330,700,330,440,370,440,370,520,380,520,380,430,320,430,320,700,300,700,300,450,200,450,200,700,175,700,175,480,125,480,125,700,100,700 }

		//complejidad reducido
		//{ 300,400,350,350,400,400,400,600,600,600,600,400,650,350,700,400,700,700,360,700,360,450,340,450,340,700,330,700,330,440,370,440,370,520,380,520,380,430,320,430,320,700,300,700,300,400}

		//catamaran
		//{ 200,200,300,400,300,600,700,600,700,400,800,200,900,400,900,600,800,800,200,800,100,600,100,400,200,200 }

		//cuadrado pico
		//{ 300,600,300,300,450,100,600,300,600,600,300,600}

		//tridente
		//{ 350, 600, 350, 350, 650, 350, 650, 750, 600, 700, 600, 400,550,400,550,600,500,600,500,400, 400, 400, 400, 600 ,350, 600 }

		//{100,900,100,400,900,400,900,900,100,900}
		//square { {300,600},{300,300}, {600,300},{600,600},{300,600} }

	;
	*/



