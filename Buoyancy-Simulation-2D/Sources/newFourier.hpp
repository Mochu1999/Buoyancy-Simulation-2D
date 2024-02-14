#pragma once

#include "Common.cpp"
#include "newLines.hpp"


struct NewFourier {

	newDLines dlines;

	vector<p> intermVector;

	int xn = 20;
	int segments = xn - 1;
	float endXPosition = 1000;
	float interval = (endXPosition - 0) / segments;
	float intervalInverse = 1 / interval;

	float period = 1000;
	float frecuency = 2 * PI / period;
	float amplitude = 100;
	float offset = 400;
	float phase = 0.0f;
	float phaseSpeed = 0.01;


	void createPositions() {
		float segmentsLength = endXPosition / segments;
		float x = 0;

		dlines.clear();
		intermVector.clear();

		intermVector.reserve( xn);
		for (int i = 0; i < xn; ++i ) 
		{
			intermVector.emplace_back(x, amplitude * sin(frecuency * x + phase) + offset);

			x += segmentsLength;
		}


		dlines.addSet(intermVector);

		//phase += phaseSpeed;
		//cout << "phase: "<<phase << endl;
		//offset++;
		//cout << offset << endl;

	}



	void draw() {

		dlines.draw();


	}

};