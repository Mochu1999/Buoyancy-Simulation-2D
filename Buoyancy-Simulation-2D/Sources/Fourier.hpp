#pragma once

#include "Common.cpp"
#include "Lines.hpp"


struct Fourier {

	Lines lines;

	vector<p> intermVector;

	int xn = 20;
	int segments = xn - 1;
	float endXPosition = 1000;
	float interval = (endXPosition - 0) / segments;
	float intervalInverse = 1 / interval;

	float period = 3000;
	float frecuency = 2 * PI / period;
	float amplitude = 100;
	float offset = 450;// 612.599;
	float phase = 1.28;
	float phaseSpeed = 0.001;


	void createPositions() {
		float segmentsLength = endXPosition / segments;
		float x = 0;

		lines.clear();
		intermVector.clear();

		intermVector.reserve( xn);
		for (int i = 0; i < xn; ++i ) 
		{
			intermVector.emplace_back(x, amplitude * sin(frecuency * x + phase) + offset);

			x += segmentsLength;
		}


		lines.addSet(intermVector);

		//phase += phaseSpeed;
		//cout << "phase: "<<phase << endl;
		//offset--;
		//cout << offset << endl;

	}



	void draw() {

		lines.draw();


	}

};