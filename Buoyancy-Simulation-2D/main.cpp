#include "Common.cpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"



#include "shader.h"
#include "utilities.cpp"

#include "Lines.hpp"



#include "Polygons.hpp"

#include "Fourier.hpp"

#include "Wetted_Surface.hpp"

#include "Circles.hpp"

#include "Text.h"
#include "Data.hpp"
#include "BinariesManager.h"




struct AllPointers {
	BinariesManager* binariesManager;
	Polygons* polygon;
};



//Preincremented for loops?
//que pasa con los const macho
//un único lines?
//switch statements are much faster than if statements
//Hola, vas a tener que testear de una vez si insert es comparable a emplace_back
//Mirar triangulation hierarchy para un locate más rápido O(sqrtN) en vez de O(n) https://doc.cgal.org/latest/Triangulation_2/index.html#Section_2D_Triangulations_Hierarchy

//cambiar i++ a ++i, date una vuelta por todos lados buscando ++ y cambiandolos


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


	/*vector<float> polygonPositions = { 150,200,200,150,250,200,150,200, 750,200,800,150,850,200,750,200 };
	binariesManager.writeModel(polygonPositions);*/

	//binariesManager.writeConfig();



	modelPositions = binariesManager.readModel();





	Polygons polygon;
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


	}
	else
	{
		polygon.lines.cadMode = true;
	}


	//polygon.draw();






	//Polygons background;
	//background.addSet({ 0,0,windowWidth,0,windowWidth,windowHeight,0,windowHeight,0,0 }); //is outside the while because is static






	Fourier fourier;

	Circles circlesFourier(2);
	Circles circlesWS(5);
	Circles circles0(3);
	Circles circlesDEBUG(5);

	WettedSurface wettedSurface(polygon, fourier);


	AllPointers allpointers;
	allpointers.binariesManager = &binariesManager;
	allpointers.polygon = &polygon;

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


			//glUniform4f(colorLocation, 0.3, 0.3, 0.3, 0);
			//background.draw();







			//fourier.createWavePositions();
			//fourier.createPositions();
			//
			//
			//high_resolution_clock::time_point currentTime;
			//high_resolution_clock::time_point lastTime;
			//float elapsedTime;
			//for (size_t i = 0; i < static_cast<size_t>(1); i++)
			//{
			//	wettedSurface.calculatePositions();
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
			//	wettedSurface.calculatePositions();
			//}
			//lastTime = high_resolution_clock::now();
			//elapsedTime = duration_cast<duration<float>>(lastTime - currentTime).count();
			//cout << "new format, time: " << elapsedTime << endl;





			polygon.clear();
			polygon.lines.clear();
			polygon.lines.addSet(model);


			polygon.interm();
			polygon.sweepTriangulation();

			fourier.createPositions();
			wettedSurface.calculatePositions();


			circlesWS.addSet(wettedSurface.positions);
			circlesFourier.addSet(fourier.lines.positions);
			circles0.addSet({ polygon.positions[0] });
			//circlesDEBUG.addSet({ { 413.793,275.415 } });



			glUniform4f(colorLocation, 195.0f / 255.0f, 130.0f / 255.0f, 49.0f / 255.0f, 1.0f);
			polygon.lines.draw();
			polygon.draw();

			glUniform4f(colorLocation, 115.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 1.0f);
			wettedSurface.draw();

			glUniform4f(colorLocation, 40.0f / 255.0f, 239.9f / 255.0f, 239.0f / 255.0f, 1.0f);
			fourier.draw();

			glUniform4f(colorLocation, 1.0f, 0.0f, 0.0f, 1.0f);
			circlesWS.draw();

			glUniform4f(colorLocation, 0.0f, 1.0f, 0.0f, 1.0f);
			circlesFourier.draw();

			glUniform4f(colorLocation, 1.0f, 1.0f, 1.0f, 1.0f);
			circles0.draw();

			glUniform4f(colorLocation, 0.0f, 0.0f, 1.0f, 1.0f);
			//circlesDEBUG.draw();









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