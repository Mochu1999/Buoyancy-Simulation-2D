#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <cmath>
#include <algorithm>


#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"



#include <array>
#include <vector>
#include <unordered_set>
#include <set>
#include <unordered_map>
#include <list>
#include <deque>

#include "utilities.h"
#include "Lines.hpp"
#include "Polygons.hpp"
#include "Wetted_Surface.hpp"

#include "FourierMesh.h"
#include "Circles.hpp"

#include "Text.h"
#include "Data.hpp"
#include "BinariesManager.h"

#include <chrono>

using namespace std::chrono;


struct AllPointers {
	Polygons* polygon;
	BinariesManager* binariesManager;
};



//Preincremented for loops?
//que pasa con los const macho
//un único dlines?
//switch statements are much faster than if statements
//Hola, vas a tener que testear de una vez si insert es comparable a emplace_back
//Mirar triangulation hierarchy para un locate más rápido O(sqrtN) en vez de O(n) https://doc.cgal.org/latest/Triangulation_2/index.html#Section_2D_Triangulations_Hierarchy




bool continueRunning = false;





#include "KeyMouseInputs.h"

struct p
{
	float x;
	float y;
};
std::vector<p> convertPositions(const std::vector<float>& modelPositions) {
	std::vector<p> output;


	for (size_t i = 0; i < modelPositions.size(); i += 2) {
		p point;
		point.x = modelPositions[i];
		point.y = modelPositions[i + 1];
		output.push_back(point);
	}

	return output;
}


vector<float> convexTriangulation(vector<float> input) {

	vector<float> indices;

	for (float i = 1; i < input.size() / 2 - 2; i++)
	{
		indices.insert(indices.end(), { 0,i,i + 1 });
	}

	return indices;
}

struct tIndices {
	int a;
	int b;
	int c;
};

vector<tIndices> newConvexTriangulation(vector<p> input) {
	vector<tIndices> indices;


	for (int i = 1; i < input.size() - 2; i++) {
		indices.push_back({ 0, i, i + 1 });
	}

	return indices;
}

int main(void)
{
	GLFWwindow* window = initialize();
	/*glDebugMessageCallback(MessageCallback, nullptr);
	glEnable(GL_DEBUG_OUTPUT);*/


	BinariesManager binariesManager;


	/*vector<float> polygonPositions = {300,300,600,300,600,600,300,600,300,300};
	binariesManager.writeModel(polygonPositions);*/

	//binariesManager.writeConfig();

	std::vector<float> modelPositions = binariesManager.readModel();





	Polygons polygon;
	if (binariesManager.currentProgramType == binariesManager.RUNNING)
	{

		polygon.addSet(modelPositions);
	}
	else
	{
		polygon.dlines.addSet({ cursorX, cursorY, });
	}
	//polygon.sweepTriangulation();


	polygon.draw();






	Polygons background;
	background.addSet({ 0,0,windowWidth,0,windowWidth,windowHeight,0,windowHeight,0,0 }); //is outside the while because is static


	//Polygons polygon;




	//FourierMesh fourier;
	//fourier.createWavePositions();


	//Circles circlesPolygon(2);


	//Circles circlesFourier(3);


	//Circles circlesWS(5);

	//Circles circles0(3);


	//WettedSurface wettedSurface(polygon, fourier);	


	AllPointers allpointers;
	allpointers.polygon = &polygon;
	allpointers.binariesManager = &binariesManager;

	glfwSetWindowUserPointer(window, &allpointers);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetKeyCallback(window, keyCallback);











	std::vector<p> newPositions = convertPositions(modelPositions);




	/*
	cout << "modelPositions: " << endl;
	for (int i = 0; i < modelPositions.size(); i += 2) {
		cout << modelPositions[i] << ", " << modelPositions[i + 1] << "," << endl;
	}cout << endl;

	for (auto& pos : newPositions) {
		std::cout << "x: " << pos.x << ", y: " << pos.y << std::endl;
	}cout << endl;


	high_resolution_clock::time_point currentTime;
	high_resolution_clock::time_point lastTime;
	float elapsedTime;

	//warm up
	for (size_t i = 0; i < 10000; i++)
	{
		vector<float> indices = convexTriangulation(modelPositions);
	}




	currentTime = high_resolution_clock::now();


	for (size_t i = 0; i < 10e+7; i++)
	{
		//vector<tIndices> indices = newConvexTriangulation(newPositions);
		float thirdElement = newPositions[2].y;

	}

	lastTime = high_resolution_clock::now();
	elapsedTime = duration_cast<duration<float>>(lastTime - currentTime).count();

	cout << "new format, time: " << elapsedTime << endl;


	currentTime = high_resolution_clock::now();

	for (size_t i = 0; i < 10e+7; i++)
	{
		//vector<float> indices = convexTriangulation(modelPositions);
		float thirdElement = modelPositions[5];
	}

	lastTime = high_resolution_clock::now();
	elapsedTime = duration_cast<duration<float>>(lastTime-currentTime).count();

	cout << "old format, time: " << elapsedTime << endl;






	*/

	/*for (auto& pos : indices) {
		std::cout << "a: " << pos.a << ", b: " << pos.b << ", c: " << pos.c << std::endl;
	}*/


	/*cout << "indices:" << endl;
	for (int i = 0; i < indices.size(); i += 3) {
		cout << indices[i] << " ";
		cout << indices[i + 1] << " ";
		cout << indices[i + 2] << endl;
	}
	cout << endl;*/







	Shader shader("resources/shaders/shader1.shader");
	shader.Bind();

	//shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);


	glm::mat4 proj = glm::ortho(0.0f, windowWidth, 0.0f, windowHeight, -1.0f, 1.0f);
	shader.SetUniformMat4f("u_MVP", proj);


	int colorLocation = glGetUniformLocation(shader.m_RendererID, "u_Color");
	int renderTypeLocation = glGetUniformLocation(shader.m_RendererID, "u_RenderType");





	//make a struct for time
	float elapsedTimeFloat = 0, fps = 0;

	//Es un coñazo que esto dependa todo el rato de wettersurface o lo que sea, haz que el constructor solo meta colorLocation, renderTypeLocation
	// y lo demás que se meta con funciones a parte
	//Data data(colorLocation, renderTypeLocation, elapsedTimeFloat, fps, polygon, wettedSurface);



	/**/

	high_resolution_clock::time_point lastFrameTime = high_resolution_clock::now();	//overkill, depends on the pc clock and it might be nanoseconds
	high_resolution_clock::time_point startElapsedTime = lastFrameTime;
	float frameCount = 0;
	float timeAccumulator = 0.0f;
	while (!glfwWindowShouldClose(window))
	{
		//system("cls");
		getCursorPos(window);


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








		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUniform1i(renderTypeLocation, 0); //0 for geometry



		glUniform4f(colorLocation, 0.3, 0.3, 0.3, 0);


		background.draw();








		glUniform4f(colorLocation, 195.0f / 255.0f, 130.0f / 255.0f, 49.0f / 255.0f, 1.0f);

		if (binariesManager.currentProgramType == binariesManager.RUNNING)
		{

		}
		else if (binariesManager.currentProgramType == binariesManager.CAD)
		{

			//He roto el modo CAD, no sé porque no crea

			if (isCreating)
			{
				polygon.hop();

				polygon.positions.pop_back();
				polygon.positions.pop_back();



				polygon.addSet({ cursorX,cursorY });
			}

		}
		

		polygon.draw();

		{

			//polygon.clear();
			//polygon.addSet(polygonPositions);
			//polygon.draw();


			//glUniform4f(colorLocation, 1.0f, 0.0f, 0.0f, 1.0f);


			//if (continueRunning)
			//{
			//	fourier.createWavePositions();
			//	wettedSurface.draw();
			//}
			//glUniform4f(colorLocation, 40.0f / 255.0f, 239.9f / 255.0f, 239.0f / 255.0f, 1.0f);
			//fourier.draw();

			//glUniform4f(colorLocation, 0.0f, 0.5f, 0.0f, 1.0f);
			//circlesPolygon.createCircles(polygon.positions);
			//circlesPolygon.draw();

			//glUniform4f(colorLocation, 0.0f, 1.0f, 0.0f, 1.0f);
			//circlesFourier.createCircles(fourier.dlines.positions);	//water
			//circlesFourier.draw();

			//glUniform4f(colorLocation, 0.0f, 0.0f, 1.0f, 1.0f);
			//circlesWS.createCircles(wettedSurface.positions);
			//circlesWS.draw();

			//glUniform4f(colorLocation, 1.0f, 0.0f, 0.0f, 1.0f);
			//circles0.createCircles({ polygon.positions[0],polygon.positions[1] });
			//circles0.draw();
			//
		}






		deltaTime = 0.0167629;

		{
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

			//data.draw();
		}



		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}