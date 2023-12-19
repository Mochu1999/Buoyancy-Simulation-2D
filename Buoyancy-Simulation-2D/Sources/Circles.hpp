

#pragma once
#include "utilities.h"
#include "GL/glew.h"
#include <cmath>
using namespace std;







struct Circles {

	vector<float> positions;
	vector <unsigned int> indices;
	int r;

	int segments = 40;	//there's a bug where small circles aren't shown if segments is set to high (80 for r=5)

	unsigned int vertexArray;
	unsigned int vertexBuffer;
	unsigned int indexBuffer;

	size_t currentBufferSize = 100000 * sizeof(float);
	/////////////////////////////////////////////////////////////////////////////////////////////
	// Efectivamente no funciona, si metes 200 circulos por ejemplo en fourier, se va a la mierda
	/////////////////////////////////////////////////////////////////////////////////////////////

	float fraction_circle; //theta increased by each segments



	Circles(int r_) :r(r_) {
		
		//segments = r_ * 3;	//this also bugs for cheese in data
		fraction_circle = 2 * PI / segments;

		genBuffers();
	}


	void createCircles(vector<float> centerPositions) {

		positions.clear(); indices.clear();


		//creates positions
		for (int i = 0; i < centerPositions.size(); i += 2) {
			createPositions(centerPositions[i], centerPositions[i + 1]);
		}


		//creates indices
		unsigned int circleCount = 0; //increased after indexing each circle
		unsigned int segmentCount = 1;//increased after indexing each segment
		for (int j = 0; j < centerPositions.size() / 2; j++) {

			for (int i = 0; i < segments * 3; i += 3)	//outputs the indices for one circle
			{
				indices.insert(indices.end(), { circleCount,circleCount + segmentCount,circleCount + segmentCount + 1 });
				segmentCount++;
			}
			segmentCount = 1;
			circleCount+= segments + 2;
		}
		

	}

	void createPositions(float centerx, float centery) {	//calculates positions
		//interts the center, then each precision point and then the second point to close the triangles

		float theta;


		positions.insert(positions.end(), { centerx, centery });

		for (int i = 0; i < segments; i++) {
			theta = (fraction_circle * i);
			positions.insert(positions.end(), { centerx + r * cos(theta), centery + r * sin(theta) });

		}
		positions.insert(positions.end(), positions.end() - segments * 2, positions.end() - segments * 2 + 2);

	}



	


	void draw() {

		glBindVertexArray(vertexArray);

		if (positions.size() * sizeof(float) > currentBufferSize) {		//Sin testear //se sigue manteniendo que currentBufferSize va a ser mayor para positions en vez de para indices?

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


	void genBuffers() {

		glGenVertexArrays(1, &vertexArray);
		glBindVertexArray(vertexArray);

		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, currentBufferSize, /* data.data() */ nullptr, GL_DYNAMIC_DRAW);

		glGenBuffers(1, &indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, currentBufferSize, nullptr, GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindVertexArray(0);
	}


	~Circles() {

		glDeleteVertexArrays(1, &vertexArray);
		glDeleteBuffers(1, &vertexBuffer);
		glDeleteBuffers(1, &indexBuffer);

	}

};
