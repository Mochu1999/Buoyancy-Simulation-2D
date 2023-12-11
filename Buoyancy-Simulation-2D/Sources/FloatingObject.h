#pragma once

#include "newPolygons.hpp"

struct FloatingObject {

	NewPolygons polygons;
	vector<float> positions;

	FloatingObject(vector <float> positions_) :positions(positions_), polygons(positions_) {
		
	}


	

	void draw() {
		polygons.draw();
	}

};