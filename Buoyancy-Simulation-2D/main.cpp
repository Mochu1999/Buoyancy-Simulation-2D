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
#include <chrono>

using namespace std::chrono;

//Preincremented for loops?
//Cleaning up buffers after using them
//Fix cicles memory leak





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
	

	SimpleCircles circles2(3); //fourier positions
	

	SimpleCircles circles3(5); //insidepoints


	WettedSurface wettedSurface(polygon.positions,polygon.indices,fourier.positions);
	



	Text text("abcp 100,200.521", 100, 100);
	
	

	

	
	
	getPos(window);

	Shader shader("resources/shaders/shader1.shader");
	shader.Bind();

	//shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);


	glm::mat4 proj = glm::ortho(0.0f, windowHeight, 0.0f, windowWidth, -1.0f, 1.0f);
	shader.SetUniformMat4f("u_MVP", proj);

	int colorLocation = glGetUniformLocation(shader.m_RendererID, "u_Color");


	





	high_resolution_clock::time_point lastFrameTime = high_resolution_clock::now();	//overkill, depends on the pc clock and it might be nanoseconds
	float frameCount = 0;
	float timeAccumulator = 0.0f;
	while (!glfwWindowShouldClose(window))
	{
		//system("cls");
		
		//high_resolution_clock::time_point currentFrameTime = high_resolution_clock::now();
		//float deltaTime = duration_cast<duration<float>>(currentFrameTime - lastFrameTime).count(); // Time elapsed in seconds between while's iterations //converting from clock units to seconds
		//lastFrameTime = currentFrameTime;

		//frameCount++;
		//timeAccumulator += deltaTime;
		//if (timeAccumulator >= 1.0f) {
		//	float fps = frameCount / timeAccumulator;
		//	//std::cout << "FPS: " << fps << std::endl;

		//	// Reset for the next second
		//	frameCount = 0;
		//	timeAccumulator -= 1.0f;
		//}
		////cout << deltaTime << endl;



		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//glUniform4f(colorLocation, 0.3, 0.3, 0.3, 0.5);
		//background.closedDraw();

		//glUniform4f(colorLocation, 1.0, 1.0, 0.5, 1.0);
		//polygon.createPolygonsLines();
		//polygon.createClosedPolygon();
		////polygon.linesDraw();	//only for debugging purposes
		//polygon.closedDraw();
		//
		//
		//fourier.createWavePositions();
		//

		//glUniform4f(colorLocation, 0.0f, 0.5f, 0.0f, 1.0f);
		//circles.createCircles(polygon.positions);
		//circles.draw();

		//glUniform4f(colorLocation, 0.0f, 1.0f, 0.0f, 1.0f);
		//circles2.createCircles(fourier.positions);	//water				//memory leak
		//circles2.draw();
		//
		//

		//

		//glUniform4f(colorLocation, 1.0f, 0.5f, 1.0f, 1.0f);
		//fourier.draw();


		//glUniform4f(colorLocation, 1.0f, 0.0f, 0.0f, 1.0f);
		//wettedSurface.createWettedPositions(polygon.triangleIndices);



		//if (wettedSurface.positions.size()) {

		//	wettedSurface.createPolygonsLines();
		//	wettedSurface.createClosedPolygon();
		//	wettedSurface.linesDraw();
		//	wettedSurface.closedDraw();
		//}

		//

		//glUniform4f(colorLocation, 0.0f, 0.0f, 1.0f, 1.0f);
		//circles3.createCircles(wettedSurface.positions);
		//circles3.draw();

		//glUniform4f(colorLocation, 1.0, 1.0, 0.5, 1.0);

		//
		////physics
		//polygon.getDownwardForce();
		//wettedSurface.getUpwardForce();
		////cout << "polygon.force: " << polygon.force << endl;
		////cout << "wettedSurface.force: " << wettedSurface.force << endl;

		//float verticalForce = wettedSurface.force - polygon.force;
		////cout << "verticalForce: " << verticalForce << endl;

		//polygon.acceleration = verticalForce / polygon.mass;
		////cout << "polygon.acceleration: " << polygon.acceleration << endl;

		//polygon.pos += polygon.vel * deltaTime + 0.5f * polygon.acceleration * deltaTime * deltaTime;
		////cout << "polygon.vel: " << polygon.vel << endl;
		//
		//polygon.vel += polygon.acceleration * deltaTime;
		//polygon.transform(-0, polygon.pos);




		text.draw();


		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}