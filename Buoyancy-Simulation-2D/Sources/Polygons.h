


#pragma once


#include <algorithm>

float absoluteAngle(float x, float y) {
	/*if (x == 0 && y == 0) {
		std::cout << "Error triggered: 0/0 is undefined" << std::endl;
		std::exit(EXIT_FAILURE);
	}*/

	float angleInRadians = atan2(y, x);
	float angleInDegrees = angleInRadians * 180 / PI;


	if (angleInDegrees < 0) {
		angleInDegrees += 360;
	}

	return angleInDegrees;
}

bool isConcave(float angle1, float angle2) {
	float angleDifference = angle2 - angle1;

	while (angleDifference < 0) {
		angleDifference += 360;
	}
	while (angleDifference >= 360) {
		angleDifference -= 360;
	}

	return angleDifference <= 180;
}

bool checkBarycentric(float x, float y, float x1, float y1, float x2, float y2, float x3, float y3) {

	float alpha = ((y2 - y3) * (x - x3) + (x3 - x2) * (y - y3)) / ((y2 - y3) * (x1 - x3) + (x3 - x2) * (y1 - y3)); //meassures the influence of x1,k1, if it were 1 and the rest  0 x,y would be on x1,y1
	float beta = ((y3 - y1) * (x - x3) + (x1 - x3) * (y - y3)) / ((y2 - y3) * (x1 - x3) + (x3 - x2) * (y1 - y3));
	float gamma = 1 - alpha - beta;
	if (alpha >= 0 && alpha <= 1 && beta >= 0 && beta <= 1 && gamma >= 0 && gamma <= 1) //if any of them is greater than 1 or less than 1 they won't be inside the triangle
		return true;	//it is inside
	else
		return false;

}
//bool checkBarycentric(float x, float y, float x1, float y1, float x2, float y2, float x3, float y3) {
//	float epsilon = 1e-2; // Tolerance value, can be adjusted based on your precision requirements
//
//	float denominator = (y2 - y3) * (x1 - x3) + (x3 - x2) * (y1 - y3);
//	if (std::abs(denominator) < epsilon) {
//		return false; // The triangle is degenerate (points are collinear or too close)
//	}
//
//	float alpha = ((y2 - y3) * (x - x3) + (x3 - x2) * (y - y3)) / denominator;
//	float beta = ((y3 - y1) * (x - x3) + (x1 - x3) * (y - y3)) / denominator;
//	float gamma = 1 - alpha - beta;
//
//	return (alpha >= -epsilon && alpha <= 1 + epsilon) &&
//		(beta >= -epsilon && beta <= 1 + epsilon) &&
//		(gamma >= -epsilon && gamma <= 1 + epsilon);
//}

//bool checkBarycentric(float x, float y, float x1, float y1, float x2, float y2, float x3, float y3) {		//suposed to be faster but not checked
//	// Inline cross product calculation and check for each edge
//	// Edge 1 (v0: x1->x2, y1->y2)
//	float c1 = (x2 - x1) * (y - y1) - (y2 - y1) * (x - x1);
//	// Edge 2 (v1: x1->x3, y1->y3)
//	float c2 = (x3 - x1) * (y - y1) - (y3 - y1) * (x - x1);
//	// If c1 and c2 are not on the same side, return false early
//	if ((c1 < 0 || c2 < 0) && (c1 > 0 || c2 > 0)) return false;
//	// Edge 3 (v2: x3->x2, y3->y2)
//	float c3 = (x2 - x3) * (y - y3) - (y2 - y3) * (x - x3);
//	// Final check
//	return (c1 >= 0 && c2 >= 0 && c3 >= 0) || (c1 <= 0 && c2 <= 0 && c3 <= 0);
//}

struct Polygons {



	//vector<float> positions /*= { 400,600,500,400,600,600,400,600 };*/= { 100,100,900,100,950,350,820,460,700,350,800,200,600,400,800,600,500,800, 150,600, 400,400,500,200,100,100 };//{ 400,400,500,200,600,400,700,600,150,600,400,400 };
	//vector<float> positions{ 400, 600, 350, 600, 350, 350, 650, 350, 650, 750, 250, 750, 250, 250, 750, 250, 750, 600, 700, 600, 700, 300, 300, 300, 300, 700, 600, 700, 600, 400, 400, 400, 400, 600 };
	//vector<float> positions{ 400, 600, 350, 600, 350, 350, 650, 350, 650, 750, 600, 700, 600, 400, 400, 400, 400, 600 };
	//vector<float> positions{ 100,700,100,400,300,400,350,350,400,400,400,600,600,600,600,400,650,350,700,400,700,700,360,700,360,450,340,450,340,700,330,700,330,440,370,440,370,520,380,520,380,430,320,430,320,700,300,700,300,450,200,450,200,700,175,700,175,480,125,480,125,700,100,700 };

	vector<float> positions = { 600,400,800,400 ,600,600 ,400,600,200,400 ,400,400,600,400 };
	//vector<float> positions = { 200,200,300,400,300,600		,700,600,700,400,800,200,900,400,900,600,800,800		,200,800,100,600,100,400,200,200 };
	//vector<float> positions = { 100,700,100,400,300,500,700,400,700,700,100,700 };


	

	vector <unsigned int> indices;
	vector <unsigned int> indicesAll;
	vector <unsigned int> indicesRemaining;
	vector<unsigned int> triangleIndices;

	unsigned int VBPolygonLines;
	unsigned int VAPolygonLines;
	//ordenar, meter doubles, limPIar y poner en xournal funciones aux
	unsigned int VBPolygonClosed;
	unsigned int VAPolygonClosed;

	float area;
	float densityArea=0.2;
	float mass;
	vector<float> centroid;
	float force;
	float pos = 0;
	float vel=0;
	float acceleration=0;

	void transform(float translationX, float translationY) {
		for (int i = 0; i < positions.size(); i += 2) {
			positions[i] += translationX;
			positions[i + 1] += translationY;
		}
	}
	void getDownwardForce() {
		mass = area * densityArea;//que hace aquí si solo o necesito una vez
		force = 9.81 * mass * 0.01;
	}
	
	void areaCalculation() {	//surveyor's formula
		area = 0; //must be here to avoid increasing area value if area is already created
		for (int i = 0; i < positions.size() - 2; i += 2) {
			area -= (positions[i + 2] - positions[i]) * (positions[i + 3] + positions[i + 1]);
		}
		area /= 2.0;
		//cout << "area: " << area << endl;
	}
	void centroidCalculation() {
		centroid = { 0,0 };//array


		for (int i = 0; i < positions.size() - 2; i += 2) {
			float Ai = positions[i] * positions[i + 3] - positions[i + 2] * positions[i + 1];

			centroid[0] += (positions[i] + positions[i + 2]) * Ai;
			centroid[1] += (positions[i + 1] + positions[i + 3]) * Ai;
		}

		centroid[0] /= (6.0 * area);
		centroid[1] /= (6.0 * area);

		//cout << "Centroid: (" << Cx << ", " << Cy << ")" << endl;	//hacer assert si area=0?
	}

	
	int createPolygonsLines() { //int feature is not being used?

		if (positions[0] == positions[positions.size() - 2] && positions.back() == positions[1] && positions.size() > 4) {	//if the polygon lines are closed (and not a point)
			areaCalculation();
			if (area < 0) {
				for (int i = 0; i < positions.size() / 2; i += 2) {
					std::swap(positions[i], positions[positions.size() - 2 - i]);
					std::swap(positions[i + 1], positions[positions.size() - 1 - i]);
				}
				areaCalculation();
			}
			centroidCalculation();
			PolygonsLinesBuffer();
			CreatePolygonLinesIBO();
			
			glBindVertexArray(0);
			return 1;
		}
		else {
			positions.insert(positions.end(), { 0,0 });
			PolygonsLinesBuffer();
			CreatePolygonLinesIBO();
			glBindVertexArray(0);
			return 0;
		}
	}

	void PolygonsLinesBuffer() {

		glGenVertexArrays(1, &VAPolygonLines);
		glBindVertexArray(VAPolygonLines);

		glGenBuffers(1, &VBPolygonLines);
		glBindBuffer(GL_ARRAY_BUFFER, VBPolygonLines);
		glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), /*positions.data()*/ nullptr, GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0); // Attribute index 0
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	}

	void CreatePolygonLinesIBO()
	{
		indices.clear(); indicesAll.clear(); indicesRemaining.clear();
		for (unsigned int i = 0; i < positions.size() / 2 - 1; i++) {
			indices.insert(indices.end(), { i,i + 1 });
		}
		for (unsigned int i = 0; i < positions.size() / 2 - 1; i++) {
			indicesAll.insert(indicesAll.end(), { i });

		}
		indicesRemaining = indicesAll;

		unsigned int ibo;
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * 4, indices.data(), GL_DYNAMIC_DRAW);
	}



	void show() {

		cout << endl << endl;
		cout << "indices" << endl;
		for (unsigned int i = 0; i < indices.size(); i++) {
			cout << indices[i] << ", ";
		}cout << endl;

		cout << "positions: " << endl;
		for (int i = 0; i < positions.size(); i += 2) {
			cout << positions[i] << ", " << positions[i + 1] << "," << endl;
		}cout << endl;




		cout << "triangleIndices:" << endl;
		for (int i = 0; i < triangleIndices.size(); i += 3) {
			cout << triangleIndices[i] << " ";
			cout << triangleIndices[i + 1] << " ";
			cout << triangleIndices[i + 2] << endl;
		}
		cout << endl;


	}
	void clear() {
		positions.clear();
		indices.clear();
		indicesAll.clear();
		indicesRemaining.clear();
		triangleIndices.clear();
	}

	void hop(float& xpos, float& ypos) {

		if (abs(xpos - positions[0]) <= 20 && abs(ypos - positions[1]) <= 20) {
			xpos = positions[0];
			ypos = positions[1];
		}
		positions[positions.size() - 2] = xpos;
		positions.back() = ypos;

	}

	void linesDraw() {

		glBindVertexArray(0);
		glBindVertexArray(VAPolygonLines);
		glBindBuffer(GL_ARRAY_BUFFER, VBPolygonLines);
		glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * 4, positions.data());
		glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
	}






	void generateTriangleIndices() {	//grouping indices
		triangleIndices.clear();
		while (!indicesRemaining.empty()) {

			for (int i = 0; i < indicesRemaining.size(); i++) {

				if (indicesRemaining.size() == 3) {		//ends while loop, closes polygon

					triangleIndices.insert(triangleIndices.end(), { indicesRemaining[0],indicesRemaining[1],indicesRemaining[2] });
					indicesRemaining.clear();
					break;
				}

				else {
					/*cout <<endl<< "triangleIndices:" << endl;
					for (int i = 0; i < triangleIndices.size(); i += 3) {
						cout << triangleIndices[i] << " ";
						cout << triangleIndices[i + 1] << " ";
						cout << triangleIndices[i + 2] << endl;
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
							triangleIndices.insert(triangleIndices.end(), { a,b,c });
							break;
						}
					}


				}
			}
		}
	}

	void createClosedPolygon() {
		generateTriangleIndices();
		PolygonsClosedBuffer();
		CreatePolygonClosedIBO();
		glBindVertexArray(0);
	}
	void PolygonsClosedBuffer() {
		glGenVertexArrays(1, &VAPolygonClosed);
		glBindVertexArray(VAPolygonClosed);

		glGenBuffers(1, &VBPolygonClosed);
		glBindBuffer(GL_ARRAY_BUFFER, VBPolygonClosed);
		glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), /*positions.data()*/ nullptr, GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0); // Attribute index 0
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	}
	void CreatePolygonClosedIBO() {
		unsigned int ibo;

		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangleIndices.size() * 4, triangleIndices.data(), GL_DYNAMIC_DRAW);
	}
	void closedDraw() {
		//show();
		glBindVertexArray(0);
		glBindVertexArray(VAPolygonClosed);
		glBindBuffer(GL_ARRAY_BUFFER, VBPolygonClosed);
		glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * 4, positions.data());
		glDrawElements(GL_TRIANGLES, triangleIndices.size(), GL_UNSIGNED_INT, nullptr);
	}




};