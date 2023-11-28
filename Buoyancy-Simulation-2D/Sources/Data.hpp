#pragma once

#include "Polygons.h"
#include "Lines.hpp"
#include "Text.h"
#include "simple_circles.h"


struct Graph {

	Lines* lines;
	Text* text;

	float& valueX;
	float& valueY;

	float x0 = 1000.0f, y0 = 300.0f;
	float spanX = 900.0f, spanY = 500.0f;

	Graph(float& valueX_,float& valueY_, Lines* lines_, Text* text_) :lines(lines_), text(text_), valueX(valueX_),valueY(valueY_) {

		std::cout << "valueX: " << valueY_ << std::endl;

		lines->addSet({ x0,y0,x0 + spanX,y0,x0 + spanX,y0 + spanY,x0,y0 + spanY,x0,y0 });
		lines->addSet({ x0 + spanX * 0.2f,y0 + spanY * 0.8f,x0 + spanX * 0.2f,y0 + spanY * 0.2f,x0 + spanX * 0.8f,y0 + spanY * 0.2f });
	}
};




struct Data {
	Polygons background;
	Lines lines;
	SimpleCircles cheese;
	Graph graph1;

	int& colorLocation; int& renderTypeLocation;
	float& elapsedTimeFloat;
	float& deltaTime;
	float& acceleration;
	Text text;





	Data(int& colorLocation_, int& renderTypeLocation_, float& elapsedTimeFloat_, float& deltaTime_,float& acceleration_)
		:colorLocation(colorLocation_), renderTypeLocation(renderTypeLocation_),
		deltaTime(deltaTime_), elapsedTimeFloat(elapsedTimeFloat_), cheese(150),
		background({ 1000,0,windowWidth,0,windowWidth,windowHeight,1000,windowHeight }), acceleration(acceleration_),
		graph1(elapsedTimeFloat_, acceleration_, &lines, &text) {


		

		background.createPolygonsLines();
		background.createClosedPolygon();


		//lines.addSet({ 1200,400,1200,200,1700,200 });
		//lines.addSet({ 1200,900,1500,1000,1600,900 });
		lines.createPolygonsLines();

		

		/*cout << "indices" << endl;
		for (unsigned int i = 0; i < lines.indices.size(); i++) {
			cout << lines.indices[i] << ", ";
		}cout << endl;*/
	}



	void draw() {
		cout << graph1.valueX << endl;

		std::ostringstream st;
		st << "Elapsed time: " << elapsedTimeFloat << " s";
		text.addText(st.str(), 1050, 1000);

		st.str(std::string());
		st << "FPS: " << deltaTime;
		text.addText(st.str(), 1500, 1000);

		/*st.str(std::string());
		st << "Acceleration";
		text.addText(st.str(), 1100, 700);*/


		glUniform1i(renderTypeLocation, 0);
		glUniform4f(colorLocation, 0.0f, 0.0f, 0.0f, 1.0f);
		background.closedDraw();

		glUniform4f(colorLocation, 0.0f, 0.0f, 1.0f, 1.0f);
		lines.draw();

		glUniform4f(colorLocation, 1.0f, 0.0f, 0.0f, 1.0f);
		glUniform1i(renderTypeLocation, 1); //1 for text
		text.draw();

		glUniform1i(renderTypeLocation, 0);
		glUniform4f(colorLocation, 1.0f, 1.0f, 1.0f, 1.0f);
		cheese.createCircles({ 1460, 200 });
		//cheese.draw();

	}
};