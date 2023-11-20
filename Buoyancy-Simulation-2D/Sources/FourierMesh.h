#pragma once


#include "utilities.h"
#include "Polygons.h"//para barycentric, no me hace gracia, globalvariables?
#include <algorithm>
#include <map>



bool isPointBetween(float P, float A, float B) { //point is between but not equal
	float minVal = B, maxVal = A;
	if (A < B) {
		minVal = A;
		maxVal = B;
	}
	return (P > minVal && P < maxVal);
}



void printMatrix(const std::vector<float>& matrix, const std::string& matrixName, int rows, int cols) {
	if (matrix.size() != rows * cols) {
		std::cout << "Invalid matrix size. Should be " << rows * cols << " elements for a " << rows << "x" << cols << " matrix." << std::endl;
		return;
	}
	std::cout << matrixName << std::endl;

	for (int row = 0; row < rows; ++row) {
		for (int col = 0; col < cols; ++col) {
			std::cout << matrix[row * cols + col] << "\t";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

struct FourierMesh {

	unsigned int VBFourierMesh;
	unsigned int VAFourierMesh;

	vector<float> positions;
	vector <unsigned int> indices;


	int xn = 2;
	int segments = xn - 1;
	double endXPosition = 1000;

	float period = 20000;
	float frecuency = 2 * PI / period;
	float amplitude = 350;
	float offset = 500;
	float phase = 9;// 2 * PI + PI / 2;		//Esto no deja de crecer
	float phaseSpeed = 0.010;

	void createWavePositions() {
		double segmentsLength = endXPosition / segments;
		double interm = 0;
		positions.clear();
		positions.reserve(2 * xn);

		for (int i = 0; i < xn; ++i, interm += segmentsLength) {
			positions.emplace_back(interm);
			positions.emplace_back(amplitude * sin(frecuency * interm + phase) + offset);
		}
		//phase += phaseSpeed;
		//cout << phase << endl;
		//offset++;
	}









	void FourierMeshCreation() {

		createWavePositions();
		FourierMeshBuffer();
		FourierMeshIBO();
		glBindVertexArray(0);

	}
	void FourierMeshBuffer() {

		glGenVertexArrays(1, &VAFourierMesh);
		glBindVertexArray(VAFourierMesh);

		glGenBuffers(1, &VBFourierMesh);
		glBindBuffer(GL_ARRAY_BUFFER, VBFourierMesh);
		glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), /*positions.data()*/ nullptr, GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0); // Attribute index 0
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	}

	void FourierMeshIBO()
	{
		indices.clear();
		for (unsigned int i = 0; i < positions.size() / 2 - 1; i++) {
			indices.insert(indices.end(), { i,i + 1 });
		}
		unsigned int ibo;
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * 4, indices.data(), GL_DYNAMIC_DRAW);
	}


	void show() {
		//cout<<intersectionPoints.size() << endl;


		//cout << "Positions size: " << positions.size() / 2 << endl;
		//cout << "Positions:" << endl;
		//for (int i = 0; i < positions.size(); i += 2) {
		//	cout << positions[i] << " ";
		//	cout << positions[i + 1] << endl;
		//}
		//cout << endl;

		//cout << "Indices:" << endl;
		//for (int i = 0; i < indices.size(); i++) {
		//	cout << indices[i] << " ";
		//}
		//cout << endl;


		cout << endl;

	}


	void draw() {
		glBindVertexArray(0);
		glBindVertexArray(VAFourierMesh);
		glBindBuffer(GL_ARRAY_BUFFER, VBFourierMesh);

		glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * 4, positions.data());
		glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
	}









};