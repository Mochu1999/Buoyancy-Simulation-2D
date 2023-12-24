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


#include "Polygons.hpp"

#include "wetted_surface.h"
#include "NewWettedSurface.h"

#include "FourierMesh.h"
#include "Circles.hpp"

#include "Text.h"
#include "Data.hpp"
#include "Lines.hpp"

#include <chrono>

using namespace std::chrono;

//Preincremented for loops?
//que pasa con los const macho

//un único dlines?

//switch statements are much faster than if statements

float xpos, ypos;
double xpos1, ypos1;

void getPos(GLFWwindow* window) {
	glfwGetCursorPos(window, &xpos1, &ypos1);
	ypos1 = windowHeight - ypos1;
	xpos = (float)xpos1;
	ypos = (float)ypos1;
}

void changePositions(vector<float>& polygonPositions) {
	polygonPositions.erase(polygonPositions.begin(), polygonPositions.begin() + 2);

	polygonPositions.insert(polygonPositions.end(), { polygonPositions[0],polygonPositions[1] });
}


bool continueRunning = false;

vector<float> polygonPositions =




//gato
//{ 100,100,900,100,950,350,820,460,700,350,800,200,600,400,800,600,500,800, 150,600, 400,400,500,200,100,100 }

//espiral
{ 400, 600, 350, 600, 350, 350, 650, 350, 650, 750, 250, 750, 250, 250, 750, 250, 750, 600, 700, 600, 700, 300, 300, 300, 300, 700, 600, 700, 600, 400, 400, 400, 400, 600 }

//semi espiral
//{  350, 600, 350, 350, 650, 350, 650, 750, 600, 700, 600, 400, 400, 400, 400, 600 ,350, 600 }

//complejidad
//{ 100,700,100,400,300,400,350,350,400,400,400,600,600,600,600,400,650,350,700,400,700,700,360,700,360,450,340,450,340,700,330,700,330,440,370,440,370,520,380,520,380,430,320,430,320,700,300,700,300,450,200,450,200,700,175,700,175,480,125,480,125,700,100,700 }

//catamaran
//{ 200,200,300,400,300,600,700,600,700,400,800,200,900,400,900,600,800,800,200,800,100,600,100,400,200,200 }

//cuadrado pico
//{ 300,600,300,300,450,100,600,300,600,600,300,600}

//tridente
//{ 350, 600, 350, 350, 650, 350, 650, 750, 600, 700, 600, 400,550,400,550,600,500,600,500,400, 400, 400, 400, 600 ,350, 600 }

//{100,900,100,400,900,400,900,900,100,900}
//{ 300,600,300,300, 600,300,600,600,300,600 }

;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	Polygons* polygon2 = static_cast<Polygons*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		changePositions(polygonPositions);
	}

	if (key == GLFW_KEY_C && action == GLFW_PRESS && continueRunning)
	{
		continueRunning = false;
	}
	else if (key == GLFW_KEY_C && action == GLFW_PRESS && !continueRunning)
	{
		continueRunning = true;
	}


	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

int main(void)
{
	GLFWwindow* window = initialize();



	/*glDebugMessageCallback(MessageCallback, nullptr);
	glEnable(GL_DEBUG_OUTPUT);*/







	Polygons background;


	Polygons polygon;

	Polygons polygon2;




	FourierMesh fourier;
	fourier.createWavePositions();


	Circles circlesPolygon(2);


	Circles circlesFourier(3);


	Circles circlesWS(5);

	Circles circles0(3);

	WettedSurface wettedSurface(polygon.positions, polygon.dlines.indices, fourier.dlines.positions);

	NewWettedSurface newWettedSurface(polygon2.positions, polygon2.dlines.indices, fourier.dlines.positions);


	glfwSetWindowUserPointer(window, &polygon2);
	glfwSetKeyCallback(window, keyCallback);









	getPos(window);

	Shader shader("resources/shaders/shader1.shader");
	shader.Bind();

	//shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);


	glm::mat4 proj = glm::ortho(0.0f, windowWidth, 0.0f, windowHeight, -1.0f, 1.0f);
	shader.SetUniformMat4f("u_MVP", proj);

	int colorLocation = glGetUniformLocation(shader.m_RendererID, "u_Color");

	int renderTypeLocation = glGetUniformLocation(shader.m_RendererID, "u_RenderType");





	//make a struct for time
	float elapsedTimeFloat = 0, fps = 0;


	Data data(colorLocation, renderTypeLocation, elapsedTimeFloat, fps, polygon, wettedSurface);





	high_resolution_clock::time_point lastFrameTime = high_resolution_clock::now();	//overkill, depends on the pc clock and it might be nanoseconds
	high_resolution_clock::time_point startElapsedTime = lastFrameTime;
	float frameCount = 0;
	float timeAccumulator = 0.0f;
	while (!glfwWindowShouldClose(window))
	{
		//system("cls");



		high_resolution_clock::time_point currentFrameTime = high_resolution_clock::now();

		elapsedTimeFloat = duration_cast<duration<float>>(currentFrameTime - startElapsedTime).count();
		//cout << elapsedTimeFloat << endl;

		float deltaTime = duration_cast<duration<float>>(currentFrameTime - lastFrameTime).count(); // Time elapsed in seconds between while's iterations //converting from clock units to seconds
		lastFrameTime = currentFrameTime;

		frameCount++;
		timeAccumulator += deltaTime;

		if (timeAccumulator >= 1.0f) {
			fps = frameCount / timeAccumulator;
			frameCount = 0;
			timeAccumulator -= 1.0f;
		}
		////cout << deltaTime << endl;





		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUniform1i(renderTypeLocation, 0); //0 for geometry



		glUniform4f(colorLocation, 0.3, 0.3, 0.3, 0);
		background.clear();
		background.addSet({ 0,0,windowWidth,0,windowWidth,windowHeight,0,windowHeight,0,0 });
		background.draw();






		glUniform4f(colorLocation, 195.0f / 255.0f, 130.0f / 255.0f, 49.0f / 255.0f, 1.0f);
		//polygon.clear();
		//polygon.addSet({ 600, 500, 600, 700, 400, 700, 400, 300, 600, 500 });
		//polygon.draw();






		polygon2.clear();
		//polygon2.addSet({ 400, 600, 350, 600, 350, 350, 650, 350, 650, 750, 250, 750, 250, 250, 750, 250, 750, 600, 700, 600, 700, 300, 300, 300, 300, 700, 600, 700, 600, 400, 400, 400, 400, 600 });
		polygon2.addSet(

			polygonPositions

		);
		polygon2.draw();











		glUniform4f(colorLocation, 1.0f, 0.0f, 0.0f, 1.0f);


		//wettedSurface.createWettedPositions(polygon.indices);
		//if (wettedSurface.positions.size()) {

		//	wettedSurface.createPolygonsLines();
		//	wettedSurface.createClosedPolygon();

		//	wettedSurface.closedDraw();
		//}
		//else {
		//	wettedSurface.area = 0;
		//}
		if (continueRunning)
		{
			fourier.createWavePositions();
			newWettedSurface.draw();
		}
		glUniform4f(colorLocation, 40.0f / 255.0f, 239.9f / 255.0f, 239.0f / 255.0f, 1.0f);
		fourier.draw();

		glUniform4f(colorLocation, 0.0f, 0.5f, 0.0f, 1.0f);
		circlesPolygon.createCircles(polygon2.positions);
		circlesPolygon.draw();

		glUniform4f(colorLocation, 0.0f, 1.0f, 0.0f, 1.0f);
		circlesFourier.createCircles(fourier.dlines.positions);	//water	
		circlesFourier.draw();

		glUniform4f(colorLocation, 0.0f, 0.0f, 1.0f, 1.0f);
		circlesWS.createCircles(newWettedSurface.positions);
		circlesWS.draw();

		glUniform4f(colorLocation, 1.0f, 0.0f, 0.0f, 1.0f);
		circles0.createCircles({ polygon2.positions[0],polygon2.positions[1] });
		circles0.draw();







		deltaTime = 0.0167629;


		//polygon.getDownwardForce();
		//wettedSurface.getUpwardForce();

		//float totalForce[2];
		//totalForce[0] = wettedSurface.force[0] + polygon.force[0];
		//totalForce[1] = wettedSurface.force[1] + polygon.force[1];



		//polygon.acceleration[0] = totalForce[0] / polygon.mass;
		//polygon.acceleration[1] = totalForce[1] / polygon.mass;


		//float deltaTimeSq = deltaTime * deltaTime;

		//float newPositionX, newPositionY;
		//for (int i = 0; i < polygon.positions.size(); i += 2) {
		//	// Horizontal Movement
		//	newPositionX = 2 * polygon.positions[i] - polygon.oldPositions[i] + polygon.acceleration[0] * deltaTimeSq;
		//	polygon.oldPositions[i] = polygon.positions[i];
		//	polygon.positions[i] = newPositionX;

		//	// Vertical Movement
		//	newPositionY = 2 * polygon.positions[i + 1] - polygon.oldPositions[i + 1] + polygon.acceleration[1] * deltaTimeSq;
		//	polygon.oldPositions[i + 1] = polygon.positions[i + 1];
		//	polygon.positions[i + 1] = newPositionY;
		//}
		////should be measured from the CG//////////////////////////////////////////////
		//polygon.velocity[0] = (polygon.positions[0] - polygon.oldPositions[0]) / deltaTime;
		//polygon.velocity[1] = (polygon.positions[1] - polygon.oldPositions[1]) / deltaTime;// +polygon.acceleration[1] * deltaTime;	//??? which one it is


		//float torque = (wettedSurface.centroid[1] - polygon.centroid[1]) * wettedSurface.force[0] -
		//	(wettedSurface.centroid[0] - polygon.centroid[0]) * wettedSurface.force[1]; //check
		//
		////cout << torque << endl;

		//float angularAcceleration = torque / polygon.totalPolarInertia;

		//float newAngle = 2 * polygon.angle - polygon.oldAngle + 0.5f * angularAcceleration * deltaTimeSq;

		//polygon.oldAngle = polygon.angle;
		//polygon.angle = newAngle;

		////debug centroid position with arrows ///////////////
		//polygon.rotate(polygon.centroid[0]*1e3+ newPositionX, polygon.centroid[1] * 1e3+ newPositionY, 0.01);

		data.draw();




		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}