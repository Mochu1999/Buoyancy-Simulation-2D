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

#include "FourierMesh.h"
#include "Circles.hpp"

#include "Text.h"
#include "Data.hpp"
#include "Lines.hpp"
#include "wetted_surface.h"
#include <chrono>

using namespace std::chrono;

//Preincremented for loops?
//que pasa con los const macho




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







	Polygons background({ 0,0,windowWidth,0,windowWidth,windowHeight,0,windowHeight,0,0 });


	//vector<float> positions{ 100,100,900,100,950,350,820,460,700,350,800,200,600,400,800,600,500,800, 150,600, 400,400,500,200,100,100 };//{ 400,400,500,200,600,400,700,600,150,600,400,400 };
	//vector<float> positions{ 400, 600, 350, 600, 350, 350, 650, 350, 650, 750, 250, 750, 250, 250, 750, 250, 750, 600, 700, 600, 700, 300, 300, 300, 300, 700, 600, 700, 600, 400, 400, 400, 400, 600 };
	//vector<float> positions{ 400, 600, 350, 600, 350, 350, 650, 350, 650, 750, 600, 700, 600, 400, 400, 400, 400, 600 };
	//vector<float> positions{ 100,700,100,400,300,400,350,350,400,400,400,600,600,600,600,400,650,350,700,400,700,700,360,700,360,450,340,450,340,700,330,700,330,440,370,440,370,520,380,520,380,430,320,430,320,700,300,700,300,450,200,450,200,700,175,700,175,480,125,480,125,700,100,700 };


	//vector<float> positions = { 200,200,300,400,300,600		,700,600,700,400,800,200,900,400,900,600,800,800		,200,800,100,600,100,400,200,200 };
	//vector<float> positions = { 100,700,100,400,300,500,700,400,700,700,100,700 };




	Polygons polygon({ 600, 500, 600, 700, 400, 700, 400, 500, 600, 500 });
	
	


	

	FourierMesh fourier;
	fourier.createWavePositions();


	Circles circles(2); //polygon			//renombrar por circlesPolygon y eso


	Circles circles2(3); //fourier positions


	Circles circles3(5); //insidepoints


	WettedSurface wettedSurface(polygon.positions, polygon.dlines.indices, fourier.dlines.positions);





	









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
		background.draw();
		





		glUniform4f(colorLocation, 195.0f / 255.0f, 130.0f / 255.0f, 49.0f / 255.0f, 1.0f);
		polygon.draw();

		fourier.createWavePositions();


		glUniform4f(colorLocation, 0.0f, 0.5f, 0.0f, 1.0f);
		circles.createCircles(polygon.positions);
		circles.draw();

		glUniform4f(colorLocation, 0.0f, 1.0f, 0.0f, 1.0f);
		circles2.createCircles(fourier.dlines.positions);	//water	
		circles2.draw();





		glUniform4f(colorLocation, 40.0f / 255.0f, 239.9f / 255.0f, 239.0f / 255.0f, 1.0f);
		fourier.draw();


		glUniform4f(colorLocation, 1.0f, 0.0f, 0.0f, 1.0f);


		wettedSurface.createWettedPositions(polygon.indices);



		if (wettedSurface.positions.size()) {

			wettedSurface.createPolygonsLines();
			wettedSurface.createClosedPolygon();

			wettedSurface.closedDraw();
		}
		else {
			wettedSurface.area = 0;
		}



		glUniform4f(colorLocation, 0.0f, 0.0f, 1.0f, 1.0f);
		circles3.createCircles(wettedSurface.positions);
		circles3.draw();

		




		



		deltaTime = 0.0167629;


		polygon.getDownwardForce();
		wettedSurface.getUpwardForce();

		float totalForce[2];
		totalForce[0] = wettedSurface.force[0] + polygon.force[0];
		totalForce[1] = wettedSurface.force[1] + polygon.force[1];



		polygon.acceleration[0] = totalForce[0] / polygon.mass;
		polygon.acceleration[1] = totalForce[1] / polygon.mass;


		float deltaTimeSq = deltaTime * deltaTime;

		float newPositionX, newPositionY;
		for (int i = 0; i < polygon.positions.size(); i += 2) {
			// Horizontal Movement
			newPositionX = 2 * polygon.positions[i] - polygon.oldPositions[i] + polygon.acceleration[0] * deltaTimeSq;
			polygon.oldPositions[i] = polygon.positions[i];
			polygon.positions[i] = newPositionX;

			// Vertical Movement
			newPositionY = 2 * polygon.positions[i + 1] - polygon.oldPositions[i + 1] + polygon.acceleration[1] * deltaTimeSq;
			polygon.oldPositions[i + 1] = polygon.positions[i + 1];
			polygon.positions[i + 1] = newPositionY;
		}
		//should be measured from the CG//////////////////////////////////////////////
		polygon.velocity[0] = (polygon.positions[0] - polygon.oldPositions[0]) / deltaTime;
		polygon.velocity[1] = (polygon.positions[1] - polygon.oldPositions[1]) / deltaTime;// +polygon.acceleration[1] * deltaTime;	//??? which one it is

		//show centroids with arrows

		float torque = (wettedSurface.centroid[1] - polygon.centroid[1]) * wettedSurface.force[0] -
			(wettedSurface.centroid[0] - polygon.centroid[0]) * wettedSurface.force[1]; //check
		
		//cout << torque << endl;

		float angularAcceleration = torque / polygon.totalPolarInertia;

		float newAngle = 2 * polygon.angle - polygon.oldAngle + 0.5f * angularAcceleration * deltaTimeSq;

		polygon.oldAngle = polygon.angle;
		polygon.angle = newAngle;

		//debug centroid position
		//polygon.rotate(polygon.centroid[0]*1e3+ newPositionX, polygon.centroid[1] * 1e3+ newPositionY, 0.01);

		data.draw();




		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}