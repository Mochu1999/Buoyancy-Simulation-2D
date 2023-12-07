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
	background.createPolygonsLines();
	background.createClosedPolygon();

	

	Polygons polygon({ 600, 400, 600, 600, 400, 600, 400, 400, 600, 400 });
	polygon.createPolygonsLines();
	polygon.createClosedPolygon();

	polygon.transform(0, 400);
	std::vector<float> previousPositions = polygon.positions;

	FourierMesh fourier;
	fourier.createWavePositions();


	Circles circles(2); //polygon


	Circles circles2(3); //fourier positions


	Circles circles3(5); //insidepoints


	WettedSurface wettedSurface(polygon.positions, polygon.indices, fourier.dlines.positions);











	



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
		background.closedDraw();

		glUniform4f(colorLocation, 1.0, 1.0, 0.5, 1.0);
		polygon.createPolygonsLines();
		polygon.createClosedPolygon();
		//polygon.linesDraw();	//only for debugging purposes //does not even work
		polygon.closedDraw();


		fourier.createWavePositions();


		glUniform4f(colorLocation, 0.0f, 0.5f, 0.0f, 1.0f);
		circles.createCircles(polygon.positions);
		circles.draw();

		glUniform4f(colorLocation, 0.0f, 1.0f, 0.0f, 1.0f);
		circles2.createCircles(fourier.dlines.positions);	//water				//memory leak
		circles2.draw();





		glUniform4f(colorLocation, 40.0f / 255.0f, 239.9f / 255.0f, 239.0f / 255.0f, 1.0f);
		fourier.draw();


		glUniform4f(colorLocation, 1.0f, 0.0f, 0.0f, 1.0f);


		wettedSurface.createWettedPositions(polygon.triangleIndices);



		if (wettedSurface.positions.size()) {

			wettedSurface.createPolygonsLines();
			wettedSurface.createClosedPolygon();
			//wettedSurface.linesDraw();
			wettedSurface.closedDraw();
		}
		else {
			wettedSurface.area = 0;
		}



		glUniform4f(colorLocation, 0.0f, 0.0f, 1.0f, 1.0f);
		circles3.createCircles(wettedSurface.positions);
		circles3.draw();

		/*glUniform4f(colorLocation, 1.0f, 1.0f, 1.0f, 1.0f);
		cheese.createCircles(wettedSurface.positions);
		cheese.draw();*/











		//cout<<deltaTime << endl;

		//physics


		//cout << "polygon area: " << polygon.area << endl;
		//cout << "wetted area: " << wettedSurface.area << endl;

		//cout << "polygon.mass: " << polygon.mass << endl << endl;

		polygon.getDownwardForce();
		wettedSurface.getUpwardForce();

		float force[2];
		force[0] = wettedSurface.force[0] + polygon.force[0];
		force[1] = wettedSurface.force[1] + polygon.force[1];
		//cout << "verticalForce: " << verticalForce << endl << endl;



		polygon.acceleration[0] = force[0] / polygon.mass;
		polygon.acceleration[1] = force[1] / polygon.mass;



		for (int i = 1; i < polygon.positions.size(); i += 2) {
			//cout << "a " << 2 * polygon.positions[i] << " " << -previousPositions[i] << " " << polygon.acceleration * deltaTime * deltaTime << endl;

			float newPositionY = 2 * polygon.positions[i] - previousPositions[i] + polygon.acceleration[1] * deltaTime * deltaTime;
			previousPositions[i] = polygon.positions[i];
			polygon.positions[i] = newPositionY; // Update current position to new position
		}






		data.draw();




		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}