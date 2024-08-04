#pragma once

#include "Common.cpp"
#include "Lines.hpp"
//instance rendering for the future

struct Circles {
	GLenum usageHint = GL_DYNAMIC_DRAW;

	bool isBufferUpdated = false;

	vector<p> positions;
	vector <unsigned int> indices;
	int r;

	int segments;

	unsigned int vertexBuffer;
	unsigned int vertexArray;
	unsigned int indexBuffer;

	//Setting initial buffers size
	size_t currentPositionsBufferSize = 0;
	size_t currentIndicesBufferSize = 0;
	size_t currentPositionsDataSize = 0;
	size_t currentIndicesDataSize = 0;

	float fraction_circle; //theta increased by each segments

	Lines circunference;

	Circles(int r_) :r(r_) {

		segments = r_ * 3;
		fraction_circle = 2 * PI / segments;

		genBuffers();
	}

	//only one set of per object for now
	void addSet(vector<p> centerPositions) {

		positions.clear(); indices.clear();

		positions.reserve((segments + 1) * centerPositions.size());

		//creates positions
		for (p center : centerPositions) {
			createCircle(center);
		}


		//creates indices
		unsigned int startIndex = 0; // center of each triangle
		for (size_t j = 0; j < centerPositions.size(); ++j) {
			for (unsigned int i = 0; i < segments; ++i) {

				indices.emplace_back(startIndex);
				indices.push_back(startIndex + i + 1);
				indices.emplace_back(startIndex + ((i + 1) % segments) + 1);
			}
			startIndex += segments + 1; // Move to the next circle's starting index
		}

		isBufferUpdated = true;
	
		vector<p> circunferencePositions = positions;
		circunferencePositions.erase(circunferencePositions.begin());
		circunferencePositions.emplace_back(circunferencePositions[0]);

		circunference.addSet(circunferencePositions);
		//printv2(positions);
		
	}

	void createCircle(const p& center) {
		//interts the center, then each segment point and then the second point to close the triangles

		float theta;


		positions.emplace_back(center);

		for (int i = 0; i < segments; i++) {
			theta = (fraction_circle * i);
			positions.insert(positions.end(), { center.x + r * cos(theta), center.y + r * sin(theta) });

		}

	}




	void draw() {

		glBindVertexArray(vertexArray);

		//flag activates only when an addSet is Called, otherwise the buffer remains the same
		if (isBufferUpdated)
		{
			currentPositionsDataSize = positions.size() * sizeof(p);
			currentIndicesDataSize = indices.size() * sizeof(unsigned int);

			//buffers are not equal
			if (currentPositionsDataSize > currentPositionsBufferSize)
			{
			currentPositionsBufferSize = currentPositionsDataSize * 2;
			currentIndicesBufferSize = currentIndicesDataSize * 2;

			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, currentPositionsBufferSize, nullptr, usageHint);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, currentIndicesBufferSize, nullptr, usageHint);
			}

			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
			glBufferSubData(GL_ARRAY_BUFFER, 0, currentPositionsDataSize, positions.data());


			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, currentIndicesDataSize, indices.data());

			isBufferUpdated = false;
		}
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);

	}
	void drawCircunference() {
		circunference.draw();
		
	}

	void genBuffers() {
		glGenVertexArrays(1, &vertexArray);
		glBindVertexArray(vertexArray);

		glGenBuffers(1, &vertexBuffer);
		glGenBuffers(1, &indexBuffer);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

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