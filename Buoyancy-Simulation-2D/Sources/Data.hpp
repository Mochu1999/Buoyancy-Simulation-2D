#pragma once

#include "Polygons.hpp"

#include "NewWettedSurface.h"
#include "Lines.hpp"
#include "Text.h"
#include "Circles.hpp"

struct Graph {	//only accept stuff over time

	DLines* dlines;
	Text* text;
	SLines* slines;

	float& valueX;
	float& valueY;

	float x0, y0;
	float spanX, spanY;
	string strValueY;
	vector<float> valuesPositions;

	Graph(float& valueX_, float& valueY_, float x0_, float y0_, float spanX_, float spanY_, string strValueY_
		, DLines* dlines_, SLines* slines_, Text* text_)
		:dlines(dlines_), slines(slines_), text(text_), valueX(valueX_), valueY(valueY_), strValueY(strValueY_)
		, x0(x0_), y0(y0_), spanX(spanX_), spanY(spanY_) {


		slines->addSet({ x0,y0,x0 + spanX,y0,x0 + spanX,y0 + spanY,x0,y0 + spanY,x0,y0 }); //quad
		slines->addSet({ x0 + spanX * 0.1f,y0 + spanY * 0.8f,x0 + spanX * 0.1f,y0 + spanY * 0.2f }); //vertical

		slines->genBuffers();
	}

	float minHeight = -1; //do not set it above 0
	float maxHeight = 5;

	float graphLength = 20; //max value of valueX before removing valuesPositions start

	float previousValueX = 0;
	float deltaValueX, drawValueX;
	void updateDynamicPositions() {

		float graphHeight = maxHeight - minHeight;

		deltaValueX = valueX - previousValueX;
		previousValueX = valueX;

		float valueX1d = floor(valueX * 10) / 10;
		float valueYd = floor(valueY * 10) / 10;



		if (valueY < minHeight) {

			for (int i = 0; i < valuesPositions.size(); i += 2) {
				valuesPositions[i + 1] += (minHeight - valueY) * 0.6f * spanY / graphHeight;
			}
			minHeight = valueY;
		}

		if (valueY > maxHeight) {

			for (int i = 0; i < valuesPositions.size(); i += 2) {
				valuesPositions[i + 1] -= (valueY - maxHeight) * 0.6f * spanY / graphHeight;
			}
			maxHeight = valueY;
		}



		dlines->addSet({ x0 + spanX * 0.1f,y0 + 0.2f * spanY + abs(minHeight) * 0.6f * spanY / graphHeight
			,x0 + spanX * 0.9f,y0 + 0.2f * spanY + abs(minHeight) * 0.6f * spanY / graphHeight }); //horizontal

		if (valueX < graphLength) {
			valuesPositions.insert(valuesPositions.end(), { x0 + spanX * 0.1f + valueX * spanX * 0.8f / graphLength ,
				y0 + spanY * 0.2f + abs(minHeight) * 0.6f * spanY / graphHeight + valueY * spanY * 0.6f / graphHeight });

			//t
			text->addText(x0 + spanX * 0.1f + valueX * spanX * 0.8f / graphLength + 5
				, y0 + spanY * 0.2f + abs(minHeight) * 0.6f * spanY / graphHeight - 24
				, "t= ", valueX1d/*,"s"*/);

		}
		else {

			valuesPositions.insert(valuesPositions.end(), { x0 + spanX * 0.1f + (graphLength + deltaValueX) * spanX * 0.8f / graphLength ,
				y0 + spanY * 0.2f + abs(minHeight) * 0.6f * spanY / graphHeight + valueY * spanY * 0.6f / graphHeight });

			for (int i = 0; i < valuesPositions.size(); i += 2) {
				valuesPositions[i] -= deltaValueX * spanX * 0.8f / graphLength;
			}

			//instead of this erase, count size before reaching 20 and only draw the .size() final elements, once a size is reached deleted the total- .size()
			valuesPositions.erase(valuesPositions.begin(), valuesPositions.begin() + 2);


			//t
			text->addText(x0 + spanX * 0.9f + 5
				, y0 + spanY * 0.2f + abs(minHeight) * 0.6f * spanY / graphHeight - 24
				, "t= ", valueX1d/*,"s"*/);

		}
		//vertical at the end
		dlines->addSet({ valuesPositions[valuesPositions.size() - 2],y0 + spanY * 0.2f + abs(minHeight) * 0.6f * spanY / graphHeight
			,valuesPositions[valuesPositions.size() - 2],y0 + spanY * 0.2f + abs(minHeight) * 0.6f * spanY / graphHeight + valueY * spanY * 0.6f / graphHeight });



		dlines->addSet(valuesPositions);








		text->addText(x0 + spanX * 0.08f, y0 + spanY * 0.82f, strValueY, "= ", valueYd/*,"m^2/s"*/);



	}


};




struct Data {
	Polygons background;	//if you end with more polygons change it to have only one instance Polygons polygon
	DLines dlines;
	SLines slines;
	Circles cheese;
	Graph graph1;
	Graph graph2;

	Polygons& polygon; //instead of passing by reference consider passing by pointer (things get nasty when one of the instances is deleted otherwise)
	NewWettedSurface& wettedSurface;

	int& colorLocation; int& renderTypeLocation;
	float& elapsedTimeFloat;
	float& deltaTime;

	Text text;





	Data(int& colorLocation_, int& renderTypeLocation_, float& elapsedTimeFloat_
		, float& deltaTime_, Polygons& polygon_, NewWettedSurface& wettedSurface_)

		:colorLocation(colorLocation_), renderTypeLocation(renderTypeLocation_)
		, deltaTime(deltaTime_), elapsedTimeFloat(elapsedTimeFloat_), cheese(150)
		/*, background({ 1000,0,windowWidth,0,windowWidth,windowHeight,1000,windowHeight,1000,0 })*/
		, polygon(polygon_), wettedSurface(wettedSurface_)
		, graph1(elapsedTimeFloat_, polygon_.acceleration[1], 1000.0f, 500.0f, 900.0f, 400.0f, "a", &dlines, &slines, &text)
		, graph2(elapsedTimeFloat_, polygon_.velocity[1], 1000.0f, 100.0f, 900.0f, 400.0f, "v", &dlines, &slines, &text) {




		//background.createPolygonsLines();	//cambiar al nuevo metodo
		//background.createClosedPolygon();







	}



	void draw() {

		glUniform1i(renderTypeLocation, 0);

		dlines.clear();

		graph1.updateDynamicPositions();
		graph2.updateDynamicPositions();


		text.addText(1050, 1000, "Elapsed time: ", elapsedTimeFloat, "s");

		text.addText(1500, 1000, "FPS: ", deltaTime);


		text.addText(1050, 920, "Downward force: ", polygon.force[1] / 9.81);
		//text.addText(1400, 920, "Upward force: ", wettedSurface.force[1] / 9.81);


		glUniform1i(renderTypeLocation, 0);
		glUniform4f(colorLocation, 0.0f, 0.0f, 0.0f, 1.0f);
		background.clear();
		background.addSet({ 1000,0,windowWidth,0,windowWidth,windowHeight,1000,windowHeight,1000,0 });
		background.draw();

		glUniform4f(colorLocation, 1.0f, 1.0f, 1.0f, 1.0f);
		dlines.draw();
		slines.draw();



		glUniform4f(colorLocation, 1.0f, 1.0f, 1.0f, 1.0f);
		cheese.createCircles({ 1460, 200 });
		//cheese.draw();


		glUniform4f(colorLocation, 1.0f, 0.0f, 0.0f, 1.0f);
		glUniform1i(renderTypeLocation, 1); //1 for text
		text.draw();
	}
};