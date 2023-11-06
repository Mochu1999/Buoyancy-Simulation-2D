#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <vector>
#include <cmath>
#include <set>
#include <algorithm>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "utilities.h"


#include "Polygons.h"
#include "FourierMesh.h"
#include "simple_circles.h"

#include "Text.h"


//Preincremented for loops?
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


	Polygons background;
	background.positions = { 0,0,windowWidth,0,windowWidth,windowHeight,0,windowHeight,0,0 };
	background.createPolygonsLines();
	background.createClosedPolygon();


	Polygons polygon;
	polygon.createPolygonsLines();
	polygon.createClosedPolygon();
	//polygon.show();
	
	FourierMesh fourier(polygon.positions,polygon.indices,polygon.triangleIndices);
	fourier.FourierMeshCreation();
	
	fourier.show();

	SimpleCircles circles(4); //polygon
	circles.createCircles(polygon.positions);

	SimpleCircles circles2(3); //fourier positions
	

	SimpleCircles circles3(5); //insidepoints

	//Text text("A");

	

	

	
	
	getPos(window);

	Shader shader("resources/shaders/shader1.shader");
	shader.Bind();
	glm::mat4 proj = glm::ortho(0.0f, windowHeight, 0.0f, windowWidth, -1.0f, 1.0f);
	shader.SetUniformMat4f("u_MVP", proj);

	int colorLocation = glGetUniformLocation(shader.m_RendererID, "u_Color");

	GLint textureUniformLocation = glGetUniformLocation(shader.m_RendererID, "u_Texture");
	glUniform1i(textureUniformLocation, 1);



	bool flagChangeMode = 1;

	while (!glfwWindowShouldClose(window))
	{
		system("cls");
		
		if (flagChangeMode) {

			flagChangeMode = 0;
		}




		//glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUniform4f(colorLocation, 0.3, 0.3, 0.3, 0.5);
		background.closedDraw();

		glUniform4f(colorLocation, 1.0, 1.0, 0.5, 1.0);
		//polygon.linesDraw();
		polygon.closedDraw();
		
		glUniform4f(colorLocation, 1.0f, 0.5f, 1.0f, 1.0f);
		fourier.createWavePositions();
		fourier.createWettedPositions();
		fourier.draw();

		glUniform4f(colorLocation, 0.0f, 0.5f, 0.0f, 1.0f);
		circles.draw();

		glUniform4f(colorLocation, 0.0f, 1.0f, 0.0f, 1.0f);
		circles2.createCircles(fourier.positions);					//memory leak
		circles2.draw();
		
		glUniform4f(colorLocation, 1.0f, 0.0f, 0.0f, 1.0f);
		circles3.createCircles(fourier.intersectionPoints);
		circles3.draw();

		//text.draw();



		if (currentState == visualState) {

			

			
			
		}

		





		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}