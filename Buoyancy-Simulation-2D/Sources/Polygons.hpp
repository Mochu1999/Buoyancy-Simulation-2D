#pragma once

#include "Lines.hpp"


struct Polygons {

	DLines dlines;

	vector<float> positions;
	vector <unsigned int> indices; //old triangleIndices
	vector <unsigned int> indicesAll;
	vector <unsigned int> indicesRemaining;


	unsigned int vertexBuffer;
	unsigned int vertexArray;
	unsigned int indexBuffer;

	

	size_t currentBufferSize = 1000 * sizeof(float);

	Polygons(vector <float> positions_) :positions(positions_) {
		genBuffers();

		
		oldPositions = positions_;
	}
	vector <float> oldPositions;
	float area; //m^2
	float densityArea = 500; //kg/m^2
	float mass; //kg
	float centroid[2] = { 0,0 };
	float force[2] = { 0,0 }; //N
	float velocity[2] = { 0,0 }; //m/s
	float acceleration[2] = { 0,0 }; //m/s^2
	float totalPolarInertia; //centroids

	float angle = 0;
	float oldAngle = 0;

	void getDownwardForce() { //inheritance?
		mass = area * densityArea;
		force[1] = -9.81 * mass;
	}

	void transform(float translationX, float translationY) {
		for (int i = 0; i < positions.size(); i += 2) {
			positions[i] += translationX;
			positions[i + 1] += translationY;
		}
	}

	void rotate(float centerX, float centerY, float angle) {

		float cosAngle = cos(angle);
		float sinAngle = sin(angle);

		for (int i = 0; i < positions.size(); i += 2) {
			float x = positions[i] - centerX;
			float y = positions[i + 1] - centerY;

			positions[i] = x * cosAngle - y * sinAngle + centerX;
			positions[i + 1] = x * sinAngle + y * cosAngle + centerY;
		}
	}

	void areaCalculation() {	//surveyor's formula
		area = 0;
		for (int i = 0; i < positions.size() - 2; i += 2) {
			area -= (positions[i + 2] - positions[i]) * (positions[i + 3] + positions[i + 1]) * 1e-6; //1e-6 to pass to meters
		}
		area /= 2.0;

	}

	void centroidCalculation() {
		std::fill(centroid, centroid + 2, 0);


		for (int i = 0; i < positions.size() - 2; i += 2) {
			float Ai = (positions[i] * positions[i + 3] - positions[i + 2] * positions[i + 1]) * 1e-6;

			centroid[0] += (positions[i] + positions[i + 2]) * 1e-3 * Ai;
			centroid[1] += (positions[i + 1] + positions[i + 3]) * 1e-3 * Ai;
		}

		centroid[0] /= (6.0 * area);
		centroid[1] /= (6.0 * area);
	}

	
	void polarAreaMomentOfInertia() {

		totalPolarInertia = 0;
		float inv36 = 1.0f / 36; //sacar a utilities

		float centroidSubTriangles[2];
		for (int i = 0; i < indices.size(); i += 3) {

			centroidSubTriangles[0] = (positions[indices[i] * 2] + positions[indices[i + 1] * 2] + positions[indices[i + 2] * 2]) / 3 * 1e-3;
			centroidSubTriangles[1] = (positions[indices[i] * 2 + 1] + positions[indices[i + 1] * 2 + 1] + positions[indices[i + 2] * 2 + 1]) / 3 * 1e-3;



			float x01 = positions[indices[i + 1] * 2] - positions[indices[i] * 2];
			float y01 = positions[indices[i + 1] * 2 + 1] - positions[indices[i] * 2 + 1];

			float x02 = positions[indices[i + 2] * 2] - positions[indices[i] * 2];
			float y02 = positions[indices[i + 2] * 2 + 1] - positions[indices[i] * 2 + 1];

			//b 01
			float b = sqrt(x01 * x01 + y01 * y01); //modulus

			//h (cross product 01,02)/01
			float h = std::abs(-x01 * y02 + x02 * y01) / b;

			//a projection of 02 into 01
			float a = (x01 * x02 + y01 * y02) / b;

			b *= 1e-3; h *= 1e-3; a *= 1e-3;

			float bh = b * h;

			float areaSubTriangle = bh * 0.5f;

			/*float xInertia = bh * h * h * inv36;
			float yInertia = bh * (b * b + a * a - a * b) * inv36;
			float polarInertia = xInertia + yInertia;*/




			float d2 = ((centroid[0] - centroidSubTriangles[0]) * (centroid[0] - centroidSubTriangles[0])
				+ (centroid[1] - centroidSubTriangles[1]) * (centroid[1] - centroidSubTriangles[1]));

			float polarInertia = bh * (b * b + a * a - a * b + h * h) * inv36 + areaSubTriangle * d2;

			totalPolarInertia += polarInertia;

		}
	}

	void draw() {

		dlines.clear();
		dlines.addSet(positions);
		//dlines.draw();	//you don't want to draw its lines


		createIndices();



		areaCalculation();
		if (area < 0) {
			for (int i = 0; i < positions.size() / 2; i += 2) {
				std::swap(positions[i], positions[positions.size() - 2 - i]);
				std::swap(positions[i + 1], positions[positions.size() - 1 - i]);
			}
			areaCalculation();
		}
		centroidCalculation();


		polarAreaMomentOfInertia();





		glBindVertexArray(vertexArray);

		if (positions.size() * sizeof(float) > currentBufferSize) { //aren't triangle indices bigger? Do they have anything to do with this?

			currentBufferSize += positions.size() * sizeof(float);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, currentBufferSize, positions.data(), GL_DYNAMIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, currentBufferSize, nullptr, GL_DYNAMIC_DRAW);
		}





		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * sizeof(float), positions.data());

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(unsigned int), indices.data());


		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
	}

	void createIndices() {	//grouping indices
		indices.clear(); indicesAll.clear(); indicesRemaining.clear();

		for (unsigned int i = 0; i < positions.size() / 2 - 1; i++) {
			indicesAll.insert(indicesAll.end(), { i });

		}
		indicesRemaining = indicesAll;



		while (!indicesRemaining.empty()) {

			for (int i = 0; i < indicesRemaining.size(); i++) {

				if (indicesRemaining.size() == 3) {		//ends while loop, closes polygon

					indices.insert(indices.end(), { indicesRemaining[0],indicesRemaining[1],indicesRemaining[2] });
					indicesRemaining.clear();
					break;
				}

				else {
					/*cout <<endl<< "indices:" << endl;
					for (int i = 0; i < indices.size(); i += 3) {
						cout << indices[i] << " ";
						cout << indices[i + 1] << " ";
						cout << indices[i + 2] << endl;
					}
					cout << endl;*/

					unsigned int b = indicesRemaining[i];	//current index is b, nearests indices a and c
					unsigned int a, c;
					if (b == indicesRemaining[0])
						a = indicesRemaining.back();
					else
						a = indicesRemaining[i - 1];

					if (b == indicesRemaining.back())
						c = indicesRemaining[0];
					else
						c = indicesRemaining[i + 1];
					//b = 0; a = 1; c = 2;
					//cout << "a: " << a << " b: " << b << " c: " << c << endl;

					bool barycentricFlag = 0;
					for (int k = 0; k < indicesAll.size(); k++) {	//are there points inside abc
						if (k != a && k != b && k != c) {
							if (checkBarycentric(positions[k * 2], positions[k * 2 + 1], positions[a * 2], positions[a * 2 + 1], positions[b * 2], positions[b * 2 + 1], positions[c * 2], positions[c * 2 + 1])) {
								//cout << k << endl;
								barycentricFlag = 1;
								break;
							}
						}
					}
					/*cout << "barycentricFlag " << barycentricFlag << endl;
					cout <<"isConcave: "<< isConcave(absoluteAngle(positions[b * 2] - positions[a * 2], positions[b * 2 + 1] - positions[a * 2 + 1]),
						absoluteAngle(positions[c * 2] - positions[b * 2], positions[c * 2 + 1] - positions[b * 2 + 1])) << endl;*/
					if (!barycentricFlag) {
						if (isConcave(absoluteAngle(positions[b * 2] - positions[a * 2], positions[b * 2 + 1] - positions[a * 2 + 1]),
							absoluteAngle(positions[c * 2] - positions[b * 2], positions[c * 2 + 1] - positions[b * 2 + 1]))) {				//is concave

							indicesRemaining.erase(indicesRemaining.begin() + i);		//errasing b and adding triangle
							indices.insert(indices.end(), { a,b,c });
							break;
						}
					}


				}
			}
		}
	}

	void genBuffers() {
		glGenVertexArrays(1, &vertexArray);
		glBindVertexArray(vertexArray);

		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, currentBufferSize, /*positions.data()*/ nullptr, GL_DYNAMIC_DRAW);

		glGenBuffers(1, &indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, currentBufferSize, nullptr, GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindVertexArray(0);
	}

};