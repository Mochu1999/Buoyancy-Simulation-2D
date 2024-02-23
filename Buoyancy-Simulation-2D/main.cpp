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
//un único dlines?
//switch statements are much faster than if statements
//Hola, vas a tener que testear de una vez si insert es comparable a emplace_back
//Mirar triangulation hierarchy para un locate más rápido O(sqrtN) en vez de O(n) https://doc.cgal.org/latest/Triangulation_2/index.html#Section_2D_Triangulations_Hierarchy



//Para parar wetted surface? Vaya nombre de mierda


std::vector<float> modelPositions;
vector<p> model;

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

#include "KeyMouseInputs.h"









int main(void)
{
	GLFWwindow* window = initialize();
	/*glDebugMessageCallback(MessageCallback, nullptr);
	glEnable(GL_DEBUG_OUTPUT);*/



	BinariesManager binariesManager;


	/*vector<float> polygonPositions = {100,300,900,300,900,600,100,600,100,300};
	binariesManager.writeModel(polygonPositions);*/

	//binariesManager.writeConfig();



	modelPositions = binariesManager.readModel();




	Polygons polygon;

	NewPolygons newPolygon;
	model = convertPositions(modelPositions);
	//model = { {300,600},{300,300},{450,300}, {600,300},{600,600},{300,600} };

	printv2(model);

	//switchOnePosition(model);
	//switchOnePosition(model);
	//switchOnePosition(model);

	//printv2(model);

	//bcontainer inicial: 8,9

	if (binariesManager.currentProgramType == binariesManager.RUNNING)
	{
		//polygon.dlines.addSet(modelPositions); //está con dlines para hacer debug a sweep en el while
		//newPolygon.dlines.addSet(model);

		newPolygon.clear();
		newPolygon.addSet(model);
		newPolygon.sweepTriangulation();

	}
	else
	{
		newPolygon.dlines.cadMode = true;
	}


	//polygon.draw();






	Polygons background;
	//background.addSet({ 0,0,windowWidth,0,windowWidth,windowHeight,0,windowHeight,0,0 }); //is outside the while because is static






	FourierMesh fourier;
	NewFourier newFourier;

	Circles circlesPolygon(2);
	Circles circlesFourier(3);
	Circles circlesWS(5);
	Circles circles0(3);

	NewCircles newcirclesFourier(2);
	NewCircles newCirclesWS(5);
	NewCircles newcircles0(3);

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

	//Es un coñazo que esto dependa todo el rato de wettersurface o lo que sea, haz que el constructor solo meta colorLocation, renderTypeLocation
	// y lo demás que se meta con funciones a parte
	//Data data(colorLocation, renderTypeLocation, elapsedTimeFloat, fps, polygon, wettedSurface);



	

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


			//background.draw();



			glUniform4f(colorLocation, 195.0f / 255.0f, 130.0f / 255.0f, 49.0f / 255.0f, 1.0f);
			//newPolygon.sweepTriangulation();
			newPolygon.dlines.draw();
			newPolygon.draw();







			if (binariesManager.currentProgramType == binariesManager.CAD)
			{


				if (newPolygon.dlines.cadMode == true)
				{

					//	//cout << "cursorX: " << cursorX << ", cursorY: " << cursorY << endl;



					if (newPolygon.dlines.positions.size() != 0)
						newPolygon.dlines.positions.pop_back();

					newPolygon.dlines.cadAddSet(cursor);

					//va aquí o antes de esto?
					newPolygon.dlines.cadHover();
				}
				newPolygon.dlines.draw();

			}
			else if (binariesManager.currentProgramType == binariesManager.RUNNING)
			{
			}






			//fourier.createWavePositions();
			//newFourier.createPositions();
			//
			//
			//high_resolution_clock::time_point currentTime;
			//high_resolution_clock::time_point lastTime;
			//float elapsedTime;
			//for (size_t i = 0; i < static_cast<size_t>(1); i++)
			//{
			//	newWettedSurface.calculatePositions();
			//}
			//currentTime = high_resolution_clock::now();
			//for (size_t i = 0; i < static_cast<size_t>(1); i++)
			//{
			//	wettedSurface.createWettedPositions();
			//}
			//lastTime = high_resolution_clock::now();
			//elapsedTime = duration_cast<duration<float>>(lastTime - currentTime).count();
			//cout << "old format, time: " << elapsedTime << endl;
			//currentTime = high_resolution_clock::now();
			//for (size_t i = 0; i < static_cast<size_t>(1); i++)
			//{
			//	newWettedSurface.calculatePositions();
			//}
			//lastTime = high_resolution_clock::now();
			//elapsedTime = duration_cast<duration<float>>(lastTime - currentTime).count();
			//cout << "new format, time: " << elapsedTime << endl;


			//cout << "newWettedSurface.positions.size(): " << newWettedSurface.positions.size() << ", wettedSurface.positions.size(): " 
			//	<< wettedSurface.positions.size() << endl;

			/*cout << "newWettedSurface.intersections.size(): " << newWettedSurface.intersections.size() << ", wettedSurface.intersections.size(): "
				<< wettedSurface.intersections.size() << endl;*/

				//printv2(newWettedSurface.positions);
				//printflat2(wettedSurface.positions);



			glUniform4f(colorLocation, 195.0f / 255.0f, 130.0f / 255.0f, 49.0f / 255.0f, 1.0f);

			/*newPolygon.clear();
			model = convertPositions(modelPositions);
			newPolygon.addSet(model);
			newPolygon.draw();*/

			/*polygon.clear();
			polygon.addSet(modelPositions);
			polygon.draw();*/




			bool newFormat = 1;
			if (!newFormat)// Old format
			{
				cout << "old format" << endl;
				

				glUniform4f(colorLocation, 195.0f / 255.0f, 130.0f / 255.0f, 49.0f / 255.0f, 1.0f);
				polygon.clear();
				polygon.addSet(modelPositions);
				polygon.draw();

				fourier.createWavePositions();
				wettedSurface.createWettedPositions();

				glUniform4f(colorLocation, 115.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 1.0f);
				//wettedSurface.draw();

				glUniform4f(colorLocation, 40.0f / 255.0f, 239.9f / 255.0f, 239.0f / 255.0f, 1.0f);
				fourier.draw();

				glUniform4f(colorLocation, 0.0f, 0.5f, 0.0f, 1.0f);
				circlesPolygon.createCircles(polygon.positions);
				circlesPolygon.draw();

				glUniform4f(colorLocation, 0.0f, 1.0f, 0.0f, 1.0f);
				circlesFourier.createCircles(fourier.dlines.positions);	//water
				circlesFourier.draw();

				glUniform4f(colorLocation, 0.0f, 0.0f, 1.0f, 1.0f);
				circlesWS.createCircles(wettedSurface.positions);
				circlesWS.draw();

				glUniform4f(colorLocation, 1.0f, 0.0f, 0.0f, 1.0f);
				circles0.createCircles({ polygon.positions[0],polygon.positions[1] });
				circles0.draw();
			}

			else /*if (!newFormat)*/  // New format
			{

				
				//newWettedSurface.calculatePositions();

				glUniform4f(colorLocation, 195.0f / 255.0f, 130.0f / 255.0f, 49.0f / 255.0f, 1.0f);
				newPolygon.dlines.clear();
				newPolygon.dlines.addSet(model);
				newPolygon.dlines.draw();

				
				
				newFourier.createPositions();
				newWettedSurface.calculatePositions();

				/*for (auto i : newWettedSurface.intersections)
				{
					newWettedSurface.positions.insert(newWettedSurface.positions.end(), { i.point.x,i.point.y });
				}*/

				glUniform4f(colorLocation, 115.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 1.0f);


				glUniform4f(colorLocation, 40.0f / 255.0f, 239.9f / 255.0f, 239.0f / 255.0f, 1.0f);
				newFourier.draw();


				glUniform4f(colorLocation, 0.0f, 0.0f, 1.0f, 1.0f);


				/*cout << "newWettedSurface.intersections" << endl;
				for (const auto& item : newWettedSurface.intersections)
				{
					cout << "{{" << item.point.x << "," << item.point.y << "},"
						<< item.segment << "," << item.imm << "}" << endl;
				}cout << endl;*/

				newCirclesWS.addSet(newWettedSurface.positions);
				newCirclesWS.draw();

				glUniform4f(colorLocation, 0.0f, 1.0f, 0.0f, 1.0f);
				newcirclesFourier.addSet(newFourier.dlines.positions);
				newcirclesFourier.draw();

				glUniform4f(colorLocation, 1.0f, 0.0f, 0.0f, 1.0f);
				newcircles0.addSet({ newPolygon.positions[0]});
				newcircles0.draw();

			}


		}

		//continueRunning = false;


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