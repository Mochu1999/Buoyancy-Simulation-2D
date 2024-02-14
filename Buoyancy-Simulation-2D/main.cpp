#include "Common.cpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"



#include "shader.h"
#include "utilities.cpp"

#include "Lines.hpp"
#include "newLines.hpp"


#include "Polygons.hpp"
#include "newPolygons.h"

#include "FourierMesh.h"
#include "newFourier.hpp"

#include "Wetted_Surface.hpp"
#include "newWetted_Surface.hpp"

#include "Circles.hpp"
#include "newCircles.hpp"

#include "Text.h"
#include "Data.hpp"
#include "BinariesManager.h"




struct AllPointers {
	Polygons* polygon;
	BinariesManager* binariesManager;
	NewPolygons* newPolygon;
};



//Preincremented for loops?
//que pasa con los const macho
//un �nico dlines?
//switch statements are much faster than if statements
//Hola, vas a tener que testear de una vez si insert es comparable a emplace_back
//Mirar triangulation hierarchy para un locate m�s r�pido O(sqrtN) en vez de O(n) https://doc.cgal.org/latest/Triangulation_2/index.html#Section_2D_Triangulations_Hierarchy



//Para parar wetted surface? Vaya nombre de mierda
bool continueRunning = true;

std::vector<float> modelPositions;



#include "KeyMouseInputs.h"


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

	modelPositions = binariesManager.readModel();




	Polygons polygon;

	NewPolygons newPolygon;
	vector<p> model = convertPositions(modelPositions);

	//printv2(model);

	if (binariesManager.currentProgramType == binariesManager.RUNNING)
	{
		polygon.addSet(modelPositions);
		newPolygon.dlines.addSet(model);
	}
	else
	{
		newPolygon.dlines.cadMode = true;
	}
	//polygon.sweepTriangulation();


	polygon.draw();






	Polygons background;
	background.addSet({ 0,0,windowWidth,0,windowWidth,windowHeight,0,windowHeight,0,0 }); //is outside the while because is static






	FourierMesh fourier;
	NewFourier newFourier;

	Circles circlesPolygon(2);
	Circles circlesFourier(3);
	Circles circlesWS(5);
	Circles circles0(3);

	NewCircles newCirclesWS(5);

	WettedSurface wettedSurface(polygon, fourier);
	NewWettedSurface newWettedSurface(newPolygon, newFourier);


	AllPointers allpointers;
	allpointers.polygon = &polygon;
	allpointers.binariesManager = &binariesManager;
	allpointers.newPolygon = &newPolygon;

	glfwSetWindowUserPointer(window, &allpointers);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetKeyCallback(window, keyCallback);






	//high_resolution_clock::time_point currentTime;
	//high_resolution_clock::time_point lastTime;
	//float elapsedTime;
	//currentTime = high_resolution_clock::now();
	//for (size_t i = 0; i < static_cast<size_t>(10e+7); i++)
	//{
	// 
	//}
	//
	//lastTime = high_resolution_clock::now();
	//elapsedTime = duration_cast<duration<float>>(lastTime - currentTime).count();
	//cout << "new format, time: " << elapsedTime << endl;
	//currentTime = high_resolution_clock::now();
	//for (size_t i = 0; i < static_cast<size_t>(10e+7); i++)
	//{
	//}
	//
	//lastTime = high_resolution_clock::now();
	//elapsedTime = duration_cast<duration<float>>(lastTime-currentTime).count();
	//cout << "old format, time: " << elapsedTime << endl;








	Shader shader("resources/shaders/shader1.shader");
	shader.Bind();

	//shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);


	glm::mat4 proj = glm::ortho(0.0f, windowWidth, 0.0f, windowHeight, -1.0f, 1.0f);
	shader.SetUniformMat4f("u_MVP", proj);


	int colorLocation = glGetUniformLocation(shader.m_RendererID, "u_Color");
	int renderTypeLocation = glGetUniformLocation(shader.m_RendererID, "u_RenderType");





	//make a struct for time
	float elapsedTimeFloat = 0, fps = 0;

	//Es un co�azo que esto dependa todo el rato de wettersurface o lo que sea, haz que el constructor solo meta colorLocation, renderTypeLocation
	// y lo dem�s que se meta con funciones a parte
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






		if (continueRunning)
		{

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUniform1i(renderTypeLocation, 0); //0 for geometry



			glUniform4f(colorLocation, 0.3, 0.3, 0.3, 0);


			background.draw();










			//socorro.draw();

			if (binariesManager.currentProgramType == binariesManager.CAD)
			{


				if (newPolygon.dlines.cadMode == true)
				{

					//	//cout << "cursorX: " << cursorX << ", cursorY: " << cursorY << endl;



					if (newPolygon.dlines.positions.size() != 0)
						newPolygon.dlines.positions.pop_back();

					newPolygon.dlines.cadAddSet(cursor);

					//va aqu� o antes de esto?
					newPolygon.dlines.cadHover();
				}
				newPolygon.dlines.draw();

			}
			else if (binariesManager.currentProgramType == binariesManager.RUNNING)
			{
			}



			
			

			


			high_resolution_clock::time_point currentTime;
			high_resolution_clock::time_point lastTime;
			float elapsedTime;


			currentTime = high_resolution_clock::now();
			for (size_t i = 0; i < static_cast<size_t>(30); i++)
			{
				
			}

			lastTime = high_resolution_clock::now();
			elapsedTime = duration_cast<duration<float>>(lastTime - currentTime).count();
			cout << "old format, time: " << elapsedTime << endl;



			currentTime = high_resolution_clock::now();
			for (size_t i = 0; i < static_cast<size_t>(30); i++)
			{

			}
			lastTime = high_resolution_clock::now();
			elapsedTime = duration_cast<duration<float>>(lastTime - currentTime).count();
			cout << "new format, time: " << elapsedTime << endl;











			

			



			bool newFormat = 0;
			if (!newFormat)// Old format
			{
				fourier.createWavePositions();
				wettedSurface.createWettedPositions();

				glUniform4f(colorLocation, 195.0f / 255.0f, 130.0f / 255.0f, 49.0f / 255.0f, 1.0f);
				polygon.clear();
				polygon.addSet(modelPositions);
				polygon.draw();

				glUniform4f(colorLocation, 115.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 1.0f);
				//wettedSurface.draw();

				glUniform4f(colorLocation, 40.0f / 255.0f, 239.9f / 255.0f, 239.0f / 255.0f, 1.0f);
				fourier.draw();

				glUniform4f(colorLocation, 0.0f, 0.5f, 0.0f, 1.0f);
				//circlesPolygon.createCircles(polygon.positions);
				//circlesPolygon.draw();

				//glUniform4f(colorLocation, 0.0f, 1.0f, 0.0f, 1.0f);
				//circlesFourier.createCircles(fourier.dlines.positions);	//water
				//circlesFourier.draw();

				glUniform4f(colorLocation, 0.0f, 0.0f, 1.0f, 1.0f);
				circlesWS.createCircles(wettedSurface.positions);
				circlesWS.draw();

				//glUniform4f(colorLocation, 1.0f, 0.0f, 0.0f, 1.0f);
				//circles0.createCircles({ polygon.positions[0],polygon.positions[1] });
				//circles0.draw();
			}

			else // New format
			{

				newFourier.createPositions();
				newWettedSurface.calculateIntersections();

				glUniform4f(colorLocation, 195.0f / 255.0f, 130.0f / 255.0f, 49.0f / 255.0f, 1.0f);
				newPolygon.dlines.clear();
				newPolygon.dlines.addSet(model);
				newPolygon.dlines.draw();


				glUniform4f(colorLocation, 115.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 1.0f);


				glUniform4f(colorLocation, 40.0f / 255.0f, 239.9f / 255.0f, 239.0f / 255.0f, 1.0f);
				newFourier.draw();


				glUniform4f(colorLocation, 0.0f, 0.0f, 1.0f, 1.0f);


				cout << "newWettedSurface.intersections" << endl;
				for (const auto& item : newWettedSurface.intersections)
				{
					cout << "{{" << item.point.x << "," << item.point.y << "},"
						<< item.segment << "," << item.imm << "}" << endl;
				}cout << endl;

				vector<p>itersectionPoints;
				for (auto item : newWettedSurface.intersections) {
					itersectionPoints.emplace_back(item.point);
					itersectionPoints.emplace_back(newWettedSurface.polygonPositions[item.imm]);
				}

				newCirclesWS.addSet(itersectionPoints);
				newCirclesWS.draw();
			}
		}
		continueRunning = false;


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