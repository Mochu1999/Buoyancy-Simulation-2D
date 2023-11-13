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
#include "wetted_surface.h"

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
	
	FourierMesh fourier;
	fourier.FourierMeshCreation();
	
	fourier.show();

	SimpleCircles circles(2); //polygon
	circles.createCircles(polygon.positions);

	SimpleCircles circles2(3); //fourier positions
	

	SimpleCircles circles3(5); //insidepoints


	WettedSurface wettedSurface(polygon.positions,polygon.indices,fourier.positions);
	



	Text text("C",100,100);
	
	

	

	
	
	getPos(window);

	Shader shader("resources/shaders/shader1.shader");
	shader.Bind();

	//shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);


	glm::mat4 proj = glm::ortho(0.0f, windowHeight, 0.0f, windowWidth, -1.0f, 1.0f);
	shader.SetUniformMat4f("u_MVP", proj);

	int colorLocation = glGetUniformLocation(shader.m_RendererID, "u_Color");


	





	bool flagChangeMode = 1;

	while (!glfwWindowShouldClose(window))
	{
		//system("cls");
		
		if (flagChangeMode) {

			flagChangeMode = 0;
		}




		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUniform4f(colorLocation, 0.3, 0.3, 0.3, 0.5);
		//background.closedDraw();

		//glUniform4f(colorLocation, 1.0, 1.0, 0.5, 1.0);
		////polygon.linesDraw();
		//polygon.closedDraw();
		//
		//
		//fourier.createWavePositions();
		//

		//glUniform4f(colorLocation, 0.0f, 0.5f, 0.0f, 1.0f);
		//circles.draw();

		//glUniform4f(colorLocation, 0.0f, 1.0f, 0.0f, 1.0f);
		//circles2.createCircles(fourier.positions);					//memory leak
		//circles2.draw();
		//
		//

		//

		//glUniform4f(colorLocation, 1.0f, 0.5f, 1.0f, 1.0f);
		//fourier.draw();

		//glUniform4f(colorLocation, 1.0f, 0.0f, 0.0f, 1.0f);
		//wettedSurface.createWettedPositions();
		//if (wettedSurface.positions.size()) {
		//	wettedSurface.createPolygonsLines();
		//	wettedSurface.createClosedPolygon();
		//	wettedSurface.linesDraw();
		//	wettedSurface.closedDraw();
		//}

		//glUniform4f(colorLocation, 0.0f, 0.0f, 1.0f, 1.0f);
		//circles3.createCircles(wettedSurface.positions);
		//circles3.draw();

		//glUniform4f(colorLocation, 1.0, 1.0, 0.5, 1.0);

		text.draw();





		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}