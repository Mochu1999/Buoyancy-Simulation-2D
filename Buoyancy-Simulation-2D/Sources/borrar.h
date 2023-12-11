//
//
//
//#pragma once
//
//
//#include <algorithm>
//
//
//
//struct Polygons {
//
//
//
//	
//	vector<float> positions;
//	vector <float> oldPositions;
//
//	Polygons(vector<float> positions_)
//		:positions(positions_) {
//
//		oldPositions = positions;
//	}
//
//
//	vector <unsigned int> indices;
//	vector <unsigned int> indicesAll;
//	vector <unsigned int> indicesRemaining;
//	vector<unsigned int> triangleIndices;
//	unsigned int ibo;
//
//	unsigned int VBPolygonLines;
//	unsigned int VAPolygonLines;
//	//ordenar, meter doubles, limPIar y poner en xournal funciones aux
//	unsigned int VBPolygonClosed;
//	unsigned int VAPolygonClosed;
//
//	float area; //m^2
//	float densityArea = 500; //kg/m^2
//	float mass; //kg
//
//	//vector<float> centroid;
//	float centroid[2] = { 0,0 };
//
//	float force[2] = { 0,0 }; //N
//
//	float velocity[2] = { 0,0 }; //m/s
//	float acceleration[2] = { 0,0 }; //m/s^2
//	float inertiaZ;
//
//	void transform(float translationX, float translationY) {
//		for (int i = 0; i < positions.size(); i += 2) {
//			positions[i] += translationX;
//			positions[i + 1] += translationY;
//		}
//	}
//	void getDownwardForce() { //inheritance?
//		mass = area * densityArea;
//		force[1] = -9.81 * mass;
//	}
//
//	void areaCalculation() {	//surveyor's formula
//		area = 0;
//		for (int i = 0; i < positions.size() - 2; i += 2) {
//			area -= (positions[i + 2] - positions[i]) * (positions[i + 3] + positions[i + 1]) * 1e-6; //1e-6 to pass to meters
//		}
//		area /= 2.0;
//
//	}
//	void centroidCalculation() {
//		std::fill(centroid, centroid + 2, 0);
//
//
//		for (int i = 0; i < positions.size() - 2; i += 2) {
//			float Ai = positions[i] * positions[i + 3] - positions[i + 2] * positions[i + 1];
//
//			centroid[0] += (positions[i] + positions[i + 2]) * Ai;
//			centroid[1] += (positions[i + 1] + positions[i + 3]) * Ai;
//		}
//
//		centroid[0] /= (6.0 * area);
//		centroid[1] /= (6.0 * area);
//
//		
//	}
//
//	int createPolygonsLines() { //int feature is not being used?
//
//		if (positions[0] == positions[positions.size() - 2] && positions.back() == positions[1] && positions.size() > 4) {	//if the polygon lines are closed (and not a point)
//			areaCalculation();
//			if (area < 0) {
//				for (int i = 0; i < positions.size() / 2; i += 2) {
//					std::swap(positions[i], positions[positions.size() - 2 - i]);
//					std::swap(positions[i + 1], positions[positions.size() - 1 - i]);
//				}
//				areaCalculation();
//			}
//			centroidCalculation();
//
//
//			PolygonsLinesBuffer();
//			CreatePolygonLinesIBO();
//
//			glBindVertexArray(0);
//			return 1;
//		}
//		else {
//			positions.insert(positions.end(), { 0,0 });
//			PolygonsLinesBuffer();
//			CreatePolygonLinesIBO();
//			glBindVertexArray(0);
//			return 0;
//		}
//	}
//
//	void PolygonsLinesBuffer() {
//
//		glGenVertexArrays(1, &VAPolygonLines);
//		glBindVertexArray(VAPolygonLines);
//
//		glGenBuffers(1, &VBPolygonLines);
//		glBindBuffer(GL_ARRAY_BUFFER, VBPolygonLines);
//
//		glEnableVertexAttribArray(0); // Attribute index 0
//		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
//	}
//
//	void CreatePolygonLinesIBO()
//	{
//		indices.clear(); indicesAll.clear(); indicesRemaining.clear();
//		for (unsigned int i = 0; i < positions.size() / 2 - 1; i++) {
//			indices.insert(indices.end(), { i,i + 1 });
//		}
//		for (unsigned int i = 0; i < positions.size() / 2 - 1; i++) {
//			indicesAll.insert(indicesAll.end(), { i });
//
//		}
//		indicesRemaining = indicesAll;
//
//
//		glGenBuffers(1, &ibo);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * 4, indices.data(), GL_DYNAMIC_DRAW);
//	}
//
//
//
//	void show() {
//
//		cout << endl << endl;
//		cout << "indices" << endl;
//		for (unsigned int i = 0; i < indices.size(); i++) {
//			cout << indices[i] << ", ";
//		}cout << endl;
//
//		cout << "positions: " << endl;
//		for (int i = 0; i < positions.size(); i += 2) {
//			cout << positions[i] << ", " << positions[i + 1] << "," << endl;
//		}cout << endl;
//
//
//
//
//		cout << "triangleIndices:" << endl;
//		for (int i = 0; i < triangleIndices.size(); i += 3) {
//			cout << triangleIndices[i] << " ";
//			cout << triangleIndices[i + 1] << " ";
//			cout << triangleIndices[i + 2] << endl;
//		}
//		cout << endl;
//
//
//	}
//	void clear() {
//		positions.clear();
//		indices.clear();
//		indicesAll.clear();
//		indicesRemaining.clear();
//		triangleIndices.clear();
//	}
//
//	
//
//	void linesDraw() {
//
//		glBindVertexArray(0);
//		glBindVertexArray(VAPolygonLines);
//		glBindBuffer(GL_ARRAY_BUFFER, VBPolygonLines);
//		glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * 4, positions.data());
//		glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
//	}
//
//
//
//
//
//
//	void generateTriangleIndices() {	//grouping indices
//		triangleIndices.clear();
//		while (!indicesRemaining.empty()) {
//
//			for (int i = 0; i < indicesRemaining.size(); i++) {
//
//				if (indicesRemaining.size() == 3) {		//ends while loop, closes polygon
//
//					triangleIndices.insert(triangleIndices.end(), { indicesRemaining[0],indicesRemaining[1],indicesRemaining[2] });
//					indicesRemaining.clear();
//					break;
//				}
//
//				else {
//					/*cout <<endl<< "triangleIndices:" << endl;
//					for (int i = 0; i < triangleIndices.size(); i += 3) {
//						cout << triangleIndices[i] << " ";
//						cout << triangleIndices[i + 1] << " ";
//						cout << triangleIndices[i + 2] << endl;
//					}
//					cout << endl;*/
//
//					unsigned int b = indicesRemaining[i];	//current index is b, nearests indices a and c
//					unsigned int a, c;
//					if (b == indicesRemaining[0])
//						a = indicesRemaining.back();
//					else
//						a = indicesRemaining[i - 1];
//
//					if (b == indicesRemaining.back())
//						c = indicesRemaining[0];
//					else
//						c = indicesRemaining[i + 1];
//					//b = 0; a = 1; c = 2;
//					//cout << "a: " << a << " b: " << b << " c: " << c << endl;
//
//					bool barycentricFlag = 0;
//					for (int k = 0; k < indicesAll.size(); k++) {	//are there points inside abc
//						if (k != a && k != b && k != c) {
//							if (checkBarycentric(positions[k * 2], positions[k * 2 + 1], positions[a * 2], positions[a * 2 + 1], positions[b * 2], positions[b * 2 + 1], positions[c * 2], positions[c * 2 + 1])) {
//								//cout << k << endl;
//								barycentricFlag = 1;
//								break;
//							}
//						}
//					}
//					/*cout << "barycentricFlag " << barycentricFlag << endl;
//					cout <<"isConcave: "<< isConcave(absoluteAngle(positions[b * 2] - positions[a * 2], positions[b * 2 + 1] - positions[a * 2 + 1]),
//						absoluteAngle(positions[c * 2] - positions[b * 2], positions[c * 2 + 1] - positions[b * 2 + 1])) << endl;*/
//					if (!barycentricFlag) {
//						if (isConcave(absoluteAngle(positions[b * 2] - positions[a * 2], positions[b * 2 + 1] - positions[a * 2 + 1]),
//							absoluteAngle(positions[c * 2] - positions[b * 2], positions[c * 2 + 1] - positions[b * 2 + 1]))) {				//is concave
//
//							indicesRemaining.erase(indicesRemaining.begin() + i);		//errasing b and adding triangle
//							triangleIndices.insert(triangleIndices.end(), { a,b,c });
//							break;
//						}
//					}
//
//
//				}
//			}
//		}
//	}
//
//	void createClosedPolygon() {
//		generateTriangleIndices();
//		PolygonsClosedBuffer();
//		glBindVertexArray(0);
//	}
//	void PolygonsClosedBuffer() {
//		glGenVertexArrays(1, &VAPolygonClosed);
//		glBindVertexArray(VAPolygonClosed);
//
//		glGenBuffers(1, &VBPolygonClosed);
//		glBindBuffer(GL_ARRAY_BUFFER, VBPolygonClosed);
//		glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), /*positions.data()*/ nullptr, GL_DYNAMIC_DRAW);
//
//		glEnableVertexAttribArray(0); // Attribute index 0
//		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
//
//		glGenBuffers(1, &ibo);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangleIndices.size() * 4, triangleIndices.data(), GL_DYNAMIC_DRAW);
//	}
//
//	void closedDraw() {
//		//show();
//		glBindVertexArray(0);
//		glBindVertexArray(VAPolygonClosed);
//		glBindBuffer(GL_ARRAY_BUFFER, VBPolygonClosed);
//		glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * 4, positions.data());
//		glDrawElements(GL_TRIANGLES, triangleIndices.size(), GL_UNSIGNED_INT, nullptr);
//	}
//
//
//
//
//};