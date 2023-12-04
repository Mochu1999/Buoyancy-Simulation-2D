#pragma once

#include "Lines.hpp"


struct FourierMesh {

	DLines dlines;

	vector<float> intermVector;

	int xn =2;
	int segments = xn - 1;
	float endXPosition = 1000;

	float period = 2000;
	float frecuency = 2 * PI / period;
	float amplitude = 0;
	float offset = 500;
	float phase = 0;		//Esto no deja de crecer
	float phaseSpeed = 0.010;


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
		//cout << phase << endl;
		//offset++;

	}



	void draw() {
		
		dlines.draw();


	}

};