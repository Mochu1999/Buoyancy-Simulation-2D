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


/*
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Triangulation_2.h>

#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>


//typedef CGAL::Triangulation_2<K>         Triangulation;

//typedef CGAL::Delaunay_triangulation_2<K> Triangulation;




#include <CGAL/Partition_traits_2.h>
#include <CGAL/partition_2.h>


typedef CGAL::Simple_cartesian<float> K;
typedef CGAL::Triangulation_2<K> Triangulation;
typedef K::Point_2 Point;
typedef CGAL::Partition_traits_2<K>::Polygon_2 Polygon_2;
typedef std::vector<Polygon_2> PolygonVector;
*/


//Preincremented for loops?
//que pasa con los const macho
//un único dlines?
//switch statements are much faster than if statements
//Hola, vas a tener que testear de una vez si insert es comparable a emplace_back
//Mirar triangulation hierarchy para un locate más rápido O(sqrtN) en vez de O(n) https://doc.cgal.org/latest/Triangulation_2/index.html#Section_2D_Triangulations_Hierarchy

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
{ 100,100,900,100,950,350,820,460,700,350,800,200,600,400,800,600,500,800, 150,600, 400,400,500,200,100,100 }

//espiral
//{ 400, 600, 350, 600, 350, 350, 650, 350, 650, 750, 250, 750, 250, 250, 750, 250, 750, 600, 700, 600, 700, 300, 300, 300, 300, 700, 600, 700, 600, 400, 400, 400, 400, 600 }

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

int counterI = 0;
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	 
	AllPointers* allpointers = static_cast<AllPointers*>(glfwGetWindowUserPointer(window));
	Polygons* polygon = allpointers->polygon;
	BinariesManager* binariesManager = allpointers->binariesManager;

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		changePositions(polygonPositions);
	}

	if (key == GLFW_KEY_P && action == GLFW_PRESS && continueRunning)
	{
		continueRunning = false;
	}
	else if (key == GLFW_KEY_P && action == GLFW_PRESS && !continueRunning)
	{
		continueRunning = true;
	}


	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		//glfwSetWindowShouldClose(window, GLFW_TRUE);
		polygon->addSet(binariesManager->readModel());
		counterI = 0;
	}
	if (key == GLFW_KEY_C && action == GLFW_PRESS)
	{


		if (counterI < polygon->sortedPoints.size())
		{
			polygon->sweepTriangulation(counterI);
			counterI++;
		}

	}
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{

		for (size_t i = 0; i < polygon->sortedPoints.size(); ++i)
		{
			polygon->sweepTriangulation(i);
		}
	}

	if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS &&
		key == GLFW_KEY_N && action == GLFW_PRESS) {
		
		binariesManager->writeConfig();

		if (binariesManager->currentProgramType == 1)
		{
			polygon->addSet(binariesManager->readModel());
		}
		
	}

	if (key == GLFW_KEY_O && action == GLFW_PRESS)
	{

		binariesManager->readConfig();
	}
}




int main(void)
{
	GLFWwindow* window = initialize();
	/*glDebugMessageCallback(MessageCallback, nullptr);
	glEnable(GL_DEBUG_OUTPUT);*/


	//vector<float> truePoints =
	//	//{200,200,600,100,500,400,450,250,400,200,200,200}
	//	//{ 100,400,150,350,250,350,300,300,250,250,150,250,100,200,150,150,250,150,500,300,250,450,150,450,100,400 }
	//{ 100,400,150,200,250,225,175,175,500,300,200,400,400,500,450,450,520,520,450,540,530,580,400,580,430,550,150,600,100,400 }
	//;



	


	


	



	




	BinariesManager binariesManager;
	//binariesManager.writeConfig();
	//binariesManager.readConfig();

	std::vector<float> modelPositions = binariesManager.readModel();



	//cout << "modelPositions: " << endl;
	//for (int val : modelPositions) {
	//	std::cout << val << " ";
	//}
	//std::cout << std::endl;


	Polygons truePolygon;
	truePolygon.addSet(modelPositions);
	//truePolygon.sweepTriangulation();




	//return 0;








	/*cout << "truePolygon.indices" << endl;
	for (unsigned int i = 0; i < truePolygon.indices.size(); i++) {
		if (i % 3 == 0 && i != 0)cout << endl;
		cout << truePolygon.indices[i] << ", ";
	}cout << endl;*/











	truePolygon.draw();






	//cgal
	/*
	std::vector<Point> points = { Point(100,100), Point(200,100), Point(200,200), Point(150,150), Point(100,200) };
	//std::vector<Point> points = { Point(0,0), Point(10,0), Point(10,10), Point(0,10) };
	//Triangulation t;
	//t.insert(points.begin(), points.end()); //triangulation occurs here
	//for (Triangulation::Finite_faces_iterator it = t.finite_faces_begin(); it != t.finite_faces_end(); ++it) {
	//	Triangulation::Triangle tr = t.triangle(it); //tr is each triangle
	//	std::cout << "Triangle: " << tr[0] << ", " << tr[1] << ", " << tr[2] << std::endl;
	//}
	//std::vector<Point> points = { Point(0,0), Point(10,0),Point(5,5), Point(10,10), Point(0,10) };
	//Polygon_2 polygon_2;
	//for (const Point& p : points) {
	//	polygon_2.push_back(p);
	//}
	//PolygonVector partitionedPolygons;
	//CGAL::approx_convex_partition_2(polygon_2.vertices_begin(), polygon_2.vertices_end(), std::back_inserter(partitionedPolygons));
	//// Triangulate each convex sub-polygon
	//int polygonCounter = 0;
	//for (const auto& subPolygon : partitionedPolygons) {
	//	std::vector<Point> subPolygonPoints(subPolygon.container().begin(), subPolygon.container().end());
	//	// Perform triangulation
	//	Triangulation triangulation;
	//	triangulation.insert(subPolygonPoints.begin(), subPolygonPoints.end());
	//	// Output the triangles
	//	cout << "polygonCounter: " << polygonCounter << endl;
	//	polygonCounter++;
	//	for (auto it = triangulation.finite_faces_begin(); it != triangulation.finite_faces_end(); ++it) {
	//		Triangulation::Triangle tr = triangulation.triangle(it);
	//		std::cout << "Triangle: " << tr[0] << ", " << tr[1] << ", " << tr[2] << std::endl;
	//	}
	//	cout << endl;
	//}


	*/








	//Polygons background;
	//background.addSet({ 0,0,windowWidth,0,windowWidth,windowHeight,0,windowHeight,0,0 }); //is outside the while because is static


	//Polygons polygon;




	//FourierMesh fourier;
	//fourier.createWavePositions();


	//Circles circlesPolygon(2);


	//Circles circlesFourier(3);


	//Circles circlesWS(5);

	//Circles circles0(3);


	//WettedSurface wettedSurface(polygon, fourier);	

	AllPointers allpointers;
	allpointers.polygon = &truePolygon;
	allpointers.binariesManager = &binariesManager;

	glfwSetWindowUserPointer(window, &allpointers);

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


		//background.draw();






		glUniform4f(colorLocation, 195.0f / 255.0f, 130.0f / 255.0f, 49.0f / 255.0f, 1.0f);




		truePolygon.draw();


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

		//data.draw();




		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}