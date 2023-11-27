#pragma once

#include "Polygons.h"
#include "Lines.hpp"
#include "Text.h"
#include "simple_circles.h"

struct Data {
	Polygons background;
	Lines lines;
	SimpleCircles cheese;


	int& colorLocation; int& renderTypeLocation;
	float& elapsedTimeFloat;
	float& deltaTime;

	Text text;


	float myFloat = 20.21;


	Data(int& colorLocation_, int& renderTypeLocation_, float& elapsedTimeFloat_, float& deltaTime_)
		:colorLocation(colorLocation_), renderTypeLocation(renderTypeLocation_),
		deltaTime(deltaTime_),elapsedTimeFloat(elapsedTimeFloat_), cheese(150),
		background({ 1000,0,windowWidth,0,windowWidth,windowHeight,1000,windowHeight }) {

		background.createPolygonsLines();
		background.createClosedPolygon();

		lines.addSet({ 1200,700,1200,500,1200,500,1700,500 });
		lines.addSet({ 1200,400,1200,200,1200,200,1700,200 });
		lines.createPolygonsLines();


	}
	


	void draw() {
		myFloat += 0.01;
		std::ostringstream st;
		st << "Elapsed time: " << elapsedTimeFloat<<" s";
		text.addText(st.str(), 1050, 1000);

		st.str(std::string());
		st << "FPS: "<< deltaTime;
		text.addText(st.str(), 1500, 1000);

		st.str(std::string());
		st << "Acceleration";
		text.addText(st.str(), 1100, 700);


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
		cheese.createCircles({1460, 200});
		cheese.draw();

	}
};