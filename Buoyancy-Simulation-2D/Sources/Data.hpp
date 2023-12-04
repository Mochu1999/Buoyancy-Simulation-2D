#pragma once

#include "Polygons.h"
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


		slines->addSet({ x0,y0,x0 + spanX,y0,x0 + spanX,y0 + spanY,x0,y0 + spanY,x0,y0 });
		slines->addSet({ x0 + spanX * 0.1f,y0 + spanY * 0.8f,x0 + spanX * 0.1f,y0 + spanY * 0.2f,x0 + spanX * 0.9f,y0 + spanY * 0.2f });
		slines->genBuffers();
	}

	int graphLength = 10; //max value of valueX before removing valuesPositions start

	float previousValueX = 0;
	float deltaValueX, drawValueX;
	void updateDynamicPositions() {

		deltaValueX = valueX - previousValueX;
		previousValueX = valueX;

		float valueX1d = floor(valueX * 10) / 10;
		float valueYd = floor(valueY * 10) / 10;


		if (valueX < graphLength) {
			valuesPositions.insert(valuesPositions.end(), { x0 + spanX * 0.1f + valueX * spanX * 0.8f / graphLength ,
				y0 + spanY * 0.4f + valueY * spanY * 0.6f / graphLength });

			//dlines->addSet({ x0 + spanX * 0.1f + valueX * spanX * 0.8f / graphLength,y0 + spanY * 0.8f
			//	,x0 + spanX * 0.1f + valueX * spanX * 0.8f / graphLength,y0 + spanY * 0.2f });
			//text->addText(x0 + spanX * 0.1f + valueX * spanX * 0.8f / graphLength, y0 + spanY * 0.15f, "t= ", valueX1d/*,"s"*/);
		}
		else {

			valuesPositions.insert(valuesPositions.end(), { x0 + spanX * 0.1f + (graphLength + deltaValueX) * spanX * 0.8f / graphLength ,
				y0 + spanY * 0.4f + valueY * spanY * 0.6f / graphLength });

			for (int i = 0; i < valuesPositions.size(); i += 2) {
				valuesPositions[i] -= deltaValueX * spanX * 0.8f / graphLength;
			}

			//instead of this erase, count size before reaching 20 and only draw the .size() final elements, once a size is reached deleted the total- .size()
			valuesPositions.erase(valuesPositions.begin(), valuesPositions.begin() + 2);
		}

		dlines->addSet({ x0 + spanX * 0.1f , y0 + spanY * 0.4f
				,x0 + spanX * 0.9f, y0 + spanY * 0.4f });

		dlines->addSet(valuesPositions);








		text->addText(x0 + spanX * 0.08f, y0 + spanY * 0.82f, strValueY, "= ", valueYd/*,"m^2/s"*/);
		text->addText(x0 + spanX * 0.85f, y0 + spanY * 0.15f, "t= ", valueX1d/*,"s"*/);


	}


};




struct Data {
	Polygons background;
	DLines dlines;
	SLines slines;
	Circles cheese;
	Graph graph1;
	Graph graph2;

	int& colorLocation; int& renderTypeLocation;
	float& elapsedTimeFloat;
	float& deltaTime;
	float& acceleration;
	float& velocity;
	Text text;





	Data(int& colorLocation_, int& renderTypeLocation_, float& elapsedTimeFloat_
		, float& deltaTime_, float& acceleration_, float& velocity_)

		:colorLocation(colorLocation_), renderTypeLocation(renderTypeLocation_)
		, deltaTime(deltaTime_), elapsedTimeFloat(elapsedTimeFloat_), cheese(150)
		, background({ 1000,0,windowWidth,0,windowWidth,windowHeight,1000,windowHeight })
		, acceleration(acceleration_), velocity(velocity_)
		, graph1(elapsedTimeFloat_, acceleration_, 1000.0f, 500.0f, 900.0f, 400.0f, "a", &dlines, &slines, &text)
		, graph2(elapsedTimeFloat_, velocity_, 1000.0f, 100.0f, 900.0f, 400.0f, "v", &dlines, &slines, &text) {




		background.createPolygonsLines();	//cambiar al nuevo metodo
		background.createClosedPolygon();






		/**/
	}



	void draw() {
		/*cout << "background.positions: " << endl;
		for (int i = 0; i < background.positions.size(); i += 2) {
			cout << background.positions[i] << ", " << background.positions[i + 1] << "," << endl;
		}cout << endl;
		cout << "indices" << endl;
		for (unsigned int i = 0; i < background.indices.size(); i++) {
			cout << background.indices[i] << ", ";
		}cout << endl;*/

		glUniform1i(renderTypeLocation, 0);

		dlines.clear();

		graph1.updateDynamicPositions();
		graph2.updateDynamicPositions();

		text.addText(1050, 1000, "Elapsed time: ", elapsedTimeFloat, "s");

		text.addText(1500, 1000, "FPS: ", deltaTime);






		glUniform1i(renderTypeLocation, 0);
		glUniform4f(colorLocation, 0.0f, 0.0f, 0.0f, 1.0f);
		background.closedDraw();

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