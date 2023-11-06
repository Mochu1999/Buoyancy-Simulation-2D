

#pragma once
#include "utilities.h"
#include "GL/glew.h"
#include <cmath>
using namespace std;


#define C_SEGMENTS 40
#define C_SEGMENTS_TOTAL (C_SEGMENTS+2)*2





struct SimpleCircles {
	vector<float> positions = {};
	vector <unsigned int> indices;
	float fraction_circle = 2 * PI / C_SEGMENTS;

	int actual_n_circ = 0;
	int n_circ = 0; //number of circles drawn

	unsigned int VACircle;
	unsigned int m_circle;

	int m_R;

	SimpleCircles(int m_R_):m_R(m_R_) {


		CircleBuffer();
		CreateCircleibo();
	}

	void createCircles(vector<float> insidePositions) {
		positions.clear();
		indices.clear();
		n_circ = 0;

		for (int i = 0; i < insidePositions.size(); i+=2) {
			CircleDataFan(insidePositions[i], insidePositions[i+1]);
		}
		
		CircleBuffer();			//por que no puedo quitar buffer?
		CreateCircleibo();
	}

	void CircleDataFan(float centerx, float centery) {
		n_circ++;
		float theta;


		//positions.reserve(C_SEGMENTS_TOTAL); //una función reserve sabiendo cuantos círculos van a haber

		positions.insert(positions.end(), { centerx, centery });//emplace_back

		for (int i = 0; i < C_SEGMENTS; i++) {
			theta = (fraction_circle * i);
			positions.insert(positions.end(), { centerx + m_R * cos(theta), centery + m_R * sin(theta) });

		}
		positions.insert(positions.end(), { positions[(n_circ - 1) * C_SEGMENTS_TOTAL + 2], positions[(n_circ - 1) * C_SEGMENTS_TOTAL + 3] });


	}


	void CircleBuffer() {
		glGenVertexArrays(1, &VACircle);
		glBindVertexArray(VACircle);

		glGenBuffers(1, &m_circle);
		glBindBuffer(GL_ARRAY_BUFFER, m_circle);
		glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), /* data.data() */ nullptr, GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0); //en Batch Rendering - Colors, min 5:55 explica si tienes más que solo la posición (como el color), lo del stride o lo que sea
	}

	void CreateCircleibo()
	{
		vector<unsigned int> indices_ind;
		unsigned int count = 1;

		for (int i = 0; i < (C_SEGMENTS) * 3; i += 3)
		{
			indices_ind.insert(indices_ind.end(), { 0,count,count + 1 });
			count++;
		}


		for (int i = 1; i < n_circ + 1; i++)
		{
			indices.insert(indices.end(), indices_ind.begin(), indices_ind.end());

			for (int j = 0; j < indices_ind.size(); j++)
			{
				indices_ind[j] += (C_SEGMENTS + 2);
			}
		}

		unsigned int ibo;
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, n_circ * C_SEGMENTS * 3 * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);
	}



	void draw() {

		glBindVertexArray(0);
		glBindVertexArray(VACircle);
		glBindBuffer(GL_ARRAY_BUFFER, m_circle);
		glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * 4, positions.data());

		glDrawElements(GL_TRIANGLES, (C_SEGMENTS) * 3 * n_circ, GL_UNSIGNED_INT, nullptr);


	}
	void showPositions() {
		std::cout << "Positions: " << endl;
		//cout << "size: " << positions.size() << endl;
		for (int i = 0; i < positions.size(); i++) {
			if (i % (C_SEGMENTS_TOTAL) == 0) {
				std::cout << endl;
			}
			if (i % 2 == 0)
				std::cout << "i: " << i / 2 << " x: " << positions[i] << " y: " << positions[i + 1] << endl;
		}
		std::cout << endl;
	}
};
