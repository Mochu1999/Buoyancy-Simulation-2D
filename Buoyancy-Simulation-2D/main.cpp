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

#include "Time.hpp"

#include "Aux.hpp"


struct AllPointers {
	BinariesManager* binariesManager;
	Polygons* polygon;
};



//que pasa con los const macho
//Hola, vas a tener que testear de una vez si insert es comparable a emplace_back
//Mirar triangulation hierarchy para un locate más rápido O(sqrtN) en vez de O(n) https://doc.cgal.org/latest/Triangulation_2/index.html#Section_2D_Triangulations_Hierarchy
//cambiar i++ a ++i en for loops, date una vuelta por todos lados buscando ++ y cambiandolos


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







//deltatime se calcula con el tiempo del frame anterior y el anterior a ese

int main(void)
{
	GLFWwindow* window = initialize();
	/*glDebugMessageCallback(MessageCallback, nullptr);
	glEnable(GL_DEBUG_OUTPUT);*/



	BinariesManager binariesManager;


	//vector<float> polygonPositions = { 200,300,700,300,700,600,200,600,200,300 };

	//binariesManager.writeModel(polygonPositions);

	//binariesManager.writeConfig();



	modelPositions = binariesManager.readModel();


	//modelPositions = { 200,300,700,300,700,600,200,600,200,300 };

	model = convertPositions(modelPositions);
	printv2(model);

	for (auto& i : model) {
		i.y += 300;
	}
	printv2(model);
	Polygons polygon;
	





	if (binariesManager.currentProgramType == binariesManager.RUNNING)
	{

	}
	else
	{
		polygon.lines.cadMode = true;
	}


	polygon.addSet(model);

	Lines centroidLine;
	centroidLine.addSet({ {polygon.centroid.x,0},{ polygon.centroid.x,1000 } });


	Polygons background;
	//background.addSet({ { 0,0 }, {windowWidth, 0}, {windowWidth, windowHeight}, {0, windowHeight}, {0, 0}});





	Fourier fourier;

	Circles circlesFourier(2);
	Circles circlesWS(5);
	Circles circles0(2);
	Circles circlesDEBUG(5);

	WettedSurface wettedSurface(polygon, fourier);

	Aux aux(polygon);

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


	/*polygon.totalTranslation = { 0,-10.773 };
	polygon.oldTotalTranslation = polygon.totalTranslation;

	fourier.phase = 39.4794;*/
	polygon.angle = PI / 4;
	polygon.oldAngle = polygon.angle;



	Shader shader("resources/shaders/shader1.shader");
	shader.Bind();



	glm::mat4 proj = glm::ortho(0.0f, windowWidth, 0.0f, windowHeight, -1.0f, 1.0f);
	shader.SetUniformMat4f("u_MVP", proj);


	int colorLocation = glGetUniformLocation(shader.m_RendererID, "u_Color");
	int renderTypeLocation = glGetUniformLocation(shader.m_RendererID, "u_RenderType");












	//Es un coñazo que esto dependa todo el rato de wettersurface o lo que sea, haz que el constructor solo meta colorLocation, renderTypeLocation
	// y lo demás que se meta con funciones a parte
	//Data data(colorLocation, renderTypeLocation, elapsedTimeFloat, fps, polygon, wettedSurface);





	Time time;
	while (!glfwWindowShouldClose(window))
	{
		time.update();




		//system("cls");
		getCursorPos(window);


		if (isRunning)
		{

			glClear(GL_COLOR_BUFFER_BIT);
			glUniform1i(renderTypeLocation, 0); //0 for geometry







			fourier.createPositions();

			vector<p> interPoints; //debug points
			/*wettedSurface.calculateIntersections();
			for (auto intersection : wettedSurface.intersections)
			{
				interPoints.emplace_back(intersection.point);
			}*/

			wettedSurface.calculatePositions();

			for (int i = 0; i < wettedSurface.validIndices.size(); i++)
			{
				cout << "**i: " << i << endl<<"  ";
				for (auto x : wettedSurface.validIndices[i])
				{
					cout << x << " ";
				}
				cout << endl;
			}






			


			circlesWS.addSet(wettedSurface.positions);
			circlesFourier.addSet(fourier.lines.positions);
			circles0.addSet({ polygon.positions[0] });
			//circlesDEBUG.addSet({ { 413.793,275.415 } });

			glUniform4f(colorLocation, 0.3, 0.3, 0.3, 0);
			background.draw();

			glUniform4f(colorLocation, 195.0f / 255.0f, 130.0f / 255.0f, 49.0f / 255.0f, 1.0f);
			//polygon.lines.draw();
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

			glUniform4f(colorLocation, 1.0f, 0.0f, 0.0f, 1.0f);
			//aux.updateAux();
			glUniform4f(colorLocation, 1.0f, 0.0f, 0.0f, 1.0f);
			//circlesDEBUG.addSet({ polygon.centroid+polygon.totalTranslation });
			//circlesDEBUG.addSet(interPoints);

			//circlesDEBUG.draw();
			//centroidLine.draw();
			/*polygon.updateTranslation(p{ 10,0 }*deltaTime);
			polygon.translate();

			printv2(polygon.model);
			printv2(polygon.positions);*/


			deltaTime = 0.0167629;


			printv(polygon.area);
			polygon.getDownwardForce();
			for (int i = 0; i < wettedSurface.outputPolygon.size(); i++)
			{
				printv(wettedSurface.outputPolygon[i].area);
				printv(fourier.offset);
				//if (wettedSurface.outputPolygon[i].area > polygon.area/2) isRunning = 0;
				
				wettedSurface.outputPolygon[i].getUpwardForce();
				polygon.force += wettedSurface.outputPolygon[i].force;

				/*glUniform4f(colorLocation, 0.0f, 1.0f, 0.0f, 1.0f);
				Aux arrow2(wettedSurface.outputPolygon[i]);
				arrow2.updateAux();

				glUniform4f(colorLocation, 1.0f, 1.0f, 1.0f, 1.0f);
				circlesDEBUG.addSet({ wettedSurface.outputPolygon[i].centroid });
				circlesDEBUG.draw();*/
			}


			


			polygon.acceleration = polygon.force / polygon.mass;


			float deltaTimeSq = deltaTime * deltaTime;

			p interm = (polygon.totalTranslation * 2 - polygon.oldTotalTranslation + polygon.acceleration * deltaTimeSq);
			polygon.oldTotalTranslation = polygon.totalTranslation;
			polygon.totalTranslation = interm;

			printp(interm);

			polygon.translate();

			

			////No es centroid es centroid+ totalTranslation! y centroid está en mm, no?
			polygon.torque = 0;
			for (int i = 0; i < wettedSurface.outputPolygon.size(); i++)
			{
				//cout << "	i: " << i << endl;
				polygon.torque += (wettedSurface.outputPolygon[i].centroid.x - polygon.centroid.x)*1e-3f * wettedSurface.outputPolygon[i].force.y;

				/*cout << "wettedSurface.outputPolygon[i].area " << wettedSurface.outputPolygon[i].area << endl;
				cout << "wettedSurface.outputPolygon[i].centroid.x " << wettedSurface.outputPolygon[i].centroid.x << endl;
				cout << "wettedSurface.outputPolygon[i].force.y " << wettedSurface.outputPolygon[i].force.y << endl;*/
			}

			float angularAcceleration = polygon.torque / polygon.totalPolarInertia;
			
			float interm2 = (polygon.angle * 2 - polygon.oldAngle + angularAcceleration * deltaTimeSq);
			
			//polygon.oldAngle = polygon.angle;
			//polygon.angle = interm2;


			//polygon.rotateAndTranslate();

			//
			//printp(polygon.centroid + polygon.totalTranslation);
			//printp(polygon.totalTranslation);
			//cout << "polygon.angle " << polygon.angle << endl;
		}

		//isRunning = false;



		//printp(polygon.totalTranslation);
		//cout << "polygon.angle " << polygon.angle << endl;

		//cout << "*******end" << endl;
		//printv2(polygon.positions);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}