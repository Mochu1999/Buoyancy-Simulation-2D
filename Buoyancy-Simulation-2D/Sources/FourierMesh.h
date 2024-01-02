#pragma once

#include "Lines.hpp"


struct FourierMesh {

	DLines dlines;

	vector<float> intermVector;

	int xn =20;
	int segments = xn - 1;
	float endXPosition = 1000;
	float interval = (endXPosition - 0) / segments;
	float intervalInverse = 1 / interval;

	float period = 400;
	float frecuency = 2 * PI / period;
	float amplitude = 400;
	float offset = 600;
	float phase = 0.0f;		//Esto no deja de crecer
	float phaseSpeed = 0.01;


	void createWavePositions() {
		float segmentsLength = endXPosition / segments;
		float interm = 0;

		dlines.clear();
		intermVector.clear();

		intermVector.reserve(2 * xn);
		for (int i = 0; i < xn; ++i, interm += segmentsLength) {
			intermVector.emplace_back(interm);
			intermVector.emplace_back(amplitude * sin(frecuency * interm + phase) + offset);
		}


		dlines.addSet(intermVector);	//it would be ideal that it could take a reference

		phase += phaseSpeed;
		//cout << "phase: "<<phase << endl;
		//offset++;
		//cout << offset << endl;
		
	}



	void draw() {
		
		dlines.draw();


	}

};