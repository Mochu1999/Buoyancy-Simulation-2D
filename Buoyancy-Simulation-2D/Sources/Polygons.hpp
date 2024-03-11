#pragma once


#include "Common.cpp"
#include "Lines.hpp"

//esto va a tener toda la funcionalidad. Si no la quieres cambiale el nombre a physicsPolygon y crea otra con menos cosas
struct Polygons {
	GLenum usageHint = GL_DYNAMIC_DRAW;

	bool isBufferUpdated = false;

	Lines lines;

	vector<p> model;
	vector<p>& positions = lines.positions;
	vector <unsigned int> indices; //triangle indices, do not mistake with the lines indices

	vector<p> oldPositions;

	unsigned int vertexBuffer;
	unsigned int vertexArray;
	unsigned int indexBuffer;


	//Setting initial buffers size
	size_t currentPositionsBufferSize = 0;
	size_t currentIndicesBufferSize = 0;
	size_t currentPositionsDataSize = 0;
	size_t currentIndicesDataSize = 0;


	//saves positions with its indices
	struct StructuredPoints {
		p point;
		unsigned int index;

		StructuredPoints() : point({ 0,0 }), index(0) {} //initializing paramenters to 0 if not specified 

		StructuredPoints(p point_, unsigned int index_)
			: point(point_), index(index_) {}

	};

	//I'm letting these outside so I don't create references for the auxiliars of sweep
	std::vector<std::deque<unsigned int>> chain;
	std::vector<StructuredPoints> sPoints;




	Polygons() {
		genBuffers();
	}

	~Polygons();

	void genBuffers();

	void draw();



	void addSet(vector<p> items);

	void clear();



	void sweepTriangulation(/*int i*/);

	//auxiliar functions for sweepTriangulation
	void trChainBack(const unsigned int& currentChain);
	void trChainFront(const unsigned int& currentChain);


	void ConvexTriangulation();





	/*vector <float> oldPositions;*/
	float area; //m^2
	float densityArea = 500; //kg/m^2
	float mass; //kg
	p centroid;
	p force; //N
	p velocity; //m/s
	p acceleration; //m/s^2
	float totalPolarInertia; //centroids

	p newTotalTranslation;
	p oldTotalTranslation = { 0,1 };

	/*float angle = 0;
	float oldAngle = 0;*/

	void getDownwardForce() {
		mass = area * densityArea;
		force.y = -9.81 * mass;
	}

	void getUpwardForce() {
		mass = area * 1025; //mass of the displaced water
		force.y = 9.81 * mass; 
	}

	void updateTranslation(p updateValue) {
		newTotalTranslation += updateValue;
	}

	//LINES no está cambiando, no sé si hará falta
	void translate() {
		for (int i = 0; i < model.size(); i++) {
			positions[i] = model[i] + newTotalTranslation;
		}
		isBufferUpdated = true;
	}


	/*void rotate(float centerX, float centerY, float angle) {

		float cosAngle = cos(angle);
		float sinAngle = sin(angle);

		for (int i = 0; i < positions.size(); i += 2) {
			float x = positions[i] - centerX;
			float y = positions[i + 1] - centerY;

			positions[i] = x * cosAngle - y * sinAngle + centerX;
			positions[i + 1] = x * sinAngle + y * cosAngle + centerY;
		}
	}*/

	void areaCalculation() {	//surveyor's formula
		area = 0;

		int n = positions.size();
		for (int i = 0; i < n - 1; ++i)
		{
			area += positions[i].x * positions[i + 1].y - positions[i + 1].x * positions[i].y;
		}
		area *= 1e-6 / 2; //*1E-6 to convert to meters

	}

	void centroidCalculation() {
		centroid = { 0,0 };


		for (int i = 0; i < positions.size() - 1; i++) {
			float factor = (positions[i].x * positions[i + 1].y - positions[i + 1].x * positions[i].y);

			centroid.x += (positions[i].x + positions[i + 1].x) * factor;
			centroid.y += (positions[i].y + positions[i + 1].y) * factor;
		}

		centroid.x *= (1e-6 / (6.0 * area));
		centroid.y *= (1e-6 / (6.0 * area));
	}

	//final inertias https://www.efunda.com/math/areas/triangle.cfm
	//calculator https://calcresource.com/moment-of-inertia-rect.html
	//breaks the polygon into triangles.Calculates the inertia of each and adds to the overall totalPolarInertia
	//Calculated with respect of the model. Should not change
	void polarAreaMomentOfInertia() { 

		totalPolarInertia = 0;


		p centroidTriangle;
		for (int i = 0; i < indices.size(); i += 3) {

			//centroid tr= (x1+x2+x3)/3
			centroidTriangle = (p{ model[indices[i]].x + model[indices[i + 1]].x + model[indices[i + 2]].x,
								  model[indices[i]].y + model[indices[i + 1]].y + model[indices[i + 2]].y } / 3) * 1e-3;




			p v1 = { model[indices[i + 1]].x - model[indices[i]].x,
					 model[indices[i + 1]].y - model[indices[i]].y };

			p v2 = { model[indices[i + 2]].x - model[indices[i]].x,
					 model[indices[i + 2]].y - model[indices[i]].y };

			//magnitude  of v1, base
			float b = sqrt(dot2(v1,v1));

			float b_inv = 1.0f / b;

			// cross2(v1,v2) is 2 times the area, 2 times the area/b is the height
			float h = std::abs(cross2(v1,v2))* b_inv;

			// a is the projection of v2 onto v1. dot2(v1,v2)=v1*v2*cos(theta),a=v2*cos(theta)
			float a = (dot2(v1,v2))* b_inv;


			b *= 1e-3f; h *= 1e-3f; a *= 1e-3f;

			float areaTriangle = 0.5f * b * h;

			// Calculate distance squared from the overall centroid to the triangle's centroid for Steiner
			p diff = centroid - centroidTriangle;
			float d2 = dot2(diff, diff);

			//I don't need these right now but I'm letting them here for the future
			/*float xInertia = bh * h * h * inv36;
			float yInertia = bh * (b * b + a * a - a * b) * inv36;
			float polarInertia = xInertia + yInertia;*/

			// Calculate polar inertia for the sub-triangle and accumulate
			float polarInertia = areaTriangle * (b * b + a * a - a * b + h * h) * inv36 + areaTriangle * d2;

			totalPolarInertia += polarInertia;
		}
	}
};
















