#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <vector>
#include <cmath>
#include <set>


#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "utilities.h"




#include <algorithm>

//GPT told me something about cleaning up buffers after using them




float xpos, ypos;
double xpos1, ypos1;

void getPos(GLFWwindow* window) {
	glfwGetCursorPos(window, &xpos1, &ypos1);
	ypos1 = windowHeight - ypos1;
	xpos = (float)xpos1;
	ypos = (float)ypos1;
}


int main(void)
{
	GLFWwindow* window = initialize();

	/*glDebugMessageCallback(MessageCallback, nullptr);
	glEnable(GL_DEBUG_OUTPUT);*/


	

	enum interfaceState {

		
		visualState
	};
	interfaceState currentState = visualState;





	Shader shader("resources/shaders/shader1.shader");
	shader.Bind();
	




	
	



	

	
	
	getPos(window);

	


	bool flagChangeMode = 1;

	while (!glfwWindowShouldClose(window))
	{
		//system("cls");
		
		if (flagChangeMode) {

			flagChangeMode = 0;
		}




		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		

		

		


		

		



		if (currentState == visualState) {

			glUniform4f(shader.colorLocation, 1.0, 1.0, 0.5, 1.0);
			//lines.draw();

			
			
		}

		





		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}