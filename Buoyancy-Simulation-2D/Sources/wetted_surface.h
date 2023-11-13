#pragma once


bool calculateIntersectionPoints(float Ax, float Ay, float Bx, float By, float Cx, float Cy, float Dx, float Dy, float& ix, float& iy) {	//AB the waves, CD the polygon
	//It assumes AB,CD are not 0

	float ABx = Bx - Ax;
	float ABy = By - Ay;
	float CDx = Dx - Cx;
	float CDy = Dy - Cy;

	float ACx = Cx - Ax;
	float ACy = Cy - Ay;

	/*
	In 2d geometry the cross product is a scalar value, not a vector it represents the area of their parallelogram
	t = 0: Intersection at point A.
	0 < t < 1: Intersection between A and B (on the line segment).
	t = 1: Intersection at point B.
	t < 0: Intersection lies before A
	t > 1: Intersection lies beyond B
	*/
	float precalculate = ABx * CDy - ABy * CDx;
	if (!precalculate) { return false; } //they are parallel

	float t = (ACx * CDy - ACy * CDx) / precalculate; //(AC×CD)/(AB×CD)
	float u = (ACx * ABy - ACy * ABx) / precalculate;//(AC×AB)/(AB×CD)



	if (t >= 0 && t <= 1 && u >= 0 && u < 1) {	//Return false for intersection in D
		ix = Ax + t * ABx;
		iy = Ay + t * ABy;
		return true;
	}

	return false;
}


float isRightOfLine(float& Ax, float& Ay, float& Bx, float& By, float& Px, float& Py) {		 //is P to the right of AB?
	float AB[2] = { Bx - Ax,  By - Ay };
	float AP[2] = { Px - Ax, Py - Ay };

	float crossProductZ = AB[0] * AP[1] - AB[1] * AP[0];

	return crossProductZ;	//if negative it is to its right, if 0, P is on the infinite line of AB
}

struct WettedSurface {

	//vector<float> positions = { 100,100,300,100,200,200,100,100 ,		500,100,700,100,600,200,500,100 };
	//vector<vector<float>> Allpositions;
	vector<float>positions;
	std::vector<std::pair<int, std::vector<float>>> mapIntersectionPoints;



	vector<float>& polygonPositions;
	vector<unsigned int>& polygonIndices;
	vector<float>& positionsFourier;

	WettedSurface(vector<float>& polygonPositions_, vector<unsigned int>& polygonIndices_, vector<float>& positionsFourier_)
		: polygonPositions(polygonPositions_), polygonIndices(polygonIndices_), positionsFourier(positionsFourier_) {
	}


	void createWettedPositions() {

		mapIntersectionPoints.clear();
		positions.clear();


		calculateIntersections();

		if (mapIntersectionPoints.size()) {
			mergeWettedPositions();
		}
		else {
			float biggestY = std::numeric_limits<float>::min();
			int biggestj = 0;

			for (int j = 0; j < polygonIndices.size(); j += 2) {

				if (polygonPositions[j + 1] > biggestY) {
					biggestY = polygonPositions[j + 1];
					biggestj = j;
				}
			}

			int biggesti = 0;

			for (int i = 0; i < positionsFourier.size(); i += 2) {

				if (positionsFourier[i] < polygonPositions[biggestj]) {
					biggesti = i;
				}
				else break;
			}
			if (isRightOfLine(positionsFourier[biggesti], positionsFourier[biggesti + 1], positionsFourier[biggesti + 2], positionsFourier[biggesti + 3], polygonPositions[polygonIndices[biggestj] * 2], polygonPositions[polygonIndices[biggestj] * 2 + 1]) <= 0) {
				positions = polygonPositions;

			}

		}

		cout << "mapIntersectionPoints:" << endl;
		for (const auto& entry : mapIntersectionPoints) {
			std::cout << "{ " << entry.first << ", { ";
			for (float value : entry.second) {
				std::cout << value << " ";
			}
			std::cout << "}}" << std::endl;
		}std::cout << std::endl;


	}




	void getWavePoints(float& wav1, float& wav2) {
		int minWav = std::min(wav1, wav2);
		int maxWav = std::max(wav1, wav2);

		for (int i = positionsFourier.size() - 2; i >= 0; i -= 2) {

			if (positionsFourier[i] > minWav && positionsFourier[i] < maxWav) {
				positions.insert(positions.end(), { positionsFourier[i],positionsFourier[i + 1] });
			}

		}

	}

	void calculateIntersections() {

		float Px, Py;
		bool intersectionFound = false;

		std::vector<std::pair<int, std::vector<float>>> possibleIntersections;
		mapIntersectionPoints.reserve(polygonIndices.size() / 2);

		for (int i = 0; i < positionsFourier.size() - 2; i += 2) {

			if (!intersectionFound) {

				for (int j = 0; j < polygonIndices.size(); j += 2) {	//j values will be twice as big because indices has repeated layout, convenient to avoid multiplying


					if (calculateIntersectionPoints(positionsFourier[i], positionsFourier[i + 1], positionsFourier[i + 2], positionsFourier[i + 3], polygonPositions[polygonIndices[j] * 2],
						polygonPositions[polygonIndices[j] * 2 + 1], polygonPositions[(polygonIndices[j] + 1) * 2], polygonPositions[(polygonIndices[j] + 1) * 2 + 1], Px, Py)) {

						intersectionFound = true;

						possibleIntersections.push_back({ j, {Px, Py, float(j)} });							//I think I don't need j for anythin

					}
				}



				if (intersectionFound) {

					intersectionFound = false;

					if (possibleIntersections.size() > 1) {

						std::sort(possibleIntersections.begin(), possibleIntersections.end(), [](const std::pair<int, std::vector<float>>& a, const std::pair<int, std::vector<float>>& b) {
							return a.second[0] < b.second[0];
							}
						);

					}

					for (int k = 0; k < possibleIntersections.size(); k++) {

						int firstKey = possibleIntersections[k].first;


						float crossProductWP = isRightOfLine(positionsFourier[i], positionsFourier[i + 1], positionsFourier[i + 2], positionsFourier[i + 3], polygonPositions[polygonIndices[firstKey] * 2], polygonPositions[polygonIndices[firstKey] * 2 + 1]);

						if (crossProductWP == 0) {	// we are checking here j+1 and j-1, if any of them is at the right they will be the inmediates, if both are or are not, none
							int indexMinus, indexPlus;
							if (firstKey == 0) {
								indexMinus = polygonIndices[polygonIndices.size() - 2];
								indexPlus = polygonIndices[1];
							}
							else if (firstKey == polygonIndices.size() - 2) {//the last index is [0], it should never check for j=back
								indexMinus = polygonIndices[firstKey] - 1;
								indexPlus = polygonIndices[0];
							}
							else {
								indexMinus = polygonIndices[firstKey] - 1;
								indexPlus = polygonIndices[firstKey] + 1;
							}


							float crossProductInmMinus = isRightOfLine(positionsFourier[i], positionsFourier[i + 1], positionsFourier[i + 2], positionsFourier[i + 3], polygonPositions[indexMinus * 2], polygonPositions[indexMinus * 2 + 1]);
							float crossProductInmPlus = isRightOfLine(positionsFourier[i], positionsFourier[i + 1], positionsFourier[i + 2], positionsFourier[i + 3], polygonPositions[indexPlus * 2], polygonPositions[indexPlus * 2 + 1]);		//y todo este x2 tú?


							if (crossProductInmPlus < 0 && crossProductInmMinus >= 0) {

								possibleIntersections[k].second[2] = indexPlus * 2;
								mapIntersectionPoints.emplace_back(std::move(possibleIntersections[k]));

							}


							else if (crossProductInmMinus < 0 && crossProductInmPlus >= 0) {

								possibleIntersections[k].second[2] = indexMinus * 2;
								mapIntersectionPoints.emplace_back(std::move(possibleIntersections[k]));
							}


							//if none it won't add anything to map

						}

						else if (crossProductWP > 0) {				//nesting else if?


							possibleIntersections[k].second[2] += 2;
							mapIntersectionPoints.emplace_back(std::move(possibleIntersections[k]));


						}
						else if (crossProductWP < 0) {

							mapIntersectionPoints.emplace_back(std::move(possibleIntersections[k]));


						}

						//if none it won't add anything to map

					}
				}

			}

		}

	}

	void getImmediates(int key1, int key2) {


		if (key1 < key2) {


			for (int i = key1; i <= key2; i += 2) {

				positions.insert(positions.end(), { polygonPositions[i], polygonPositions[i + 1] });
			}
		}
		if (key2 < key1) {
			for (int i = key1; i < polygonIndices.size(); i += 2) {
				positions.insert(positions.end(), { polygonPositions[i],polygonPositions[i + 1] });
			}
			for (int i = 0; i <= key2; i += 2) {
				positions.insert(positions.end(), { polygonPositions[i],polygonPositions[i + 1] });
			}
		}
		if (key1 == key2) {

			positions.insert(positions.end(), { polygonPositions[key1],polygonPositions[key1 + 1] });
		}
		//case multiple intersections in one line
	}


	void mergeWettedPositions() {
		/*cout << "mapIntersectionPoints" << endl;
		for (const auto& entry : mapIntersectionPoints) {
			std::cout << "Key: " << entry.first << ",   Values: ";
			for (float value : entry.second) {
				std::cout << value << " ";
			}
			std::cout << std::endl;
		}std::cout << std::endl;*/



		////////////////////////////////////////////////////////////////////////////
				//Hacer closedPolygon para cada iteración del while//
		////////////////////////////////////////////////////////////////////////////



		int countIndices = 0;

		auto it = mapIntersectionPoints.begin();

		while (it != mapIntersectionPoints.end()) {


			auto wav1 = it->second[0];
			auto imm1 = it->second[2];
			std::move(it->second.begin(), it->second.begin() + 2, std::back_inserter(positions));


			float firstInters[2];
			std::move(it->second.begin(), it->second.begin() + 2, firstInters);

			++it;
			auto wav2 = it->second[0];
			auto imm2 = it->second[2];
			getImmediates(imm1, imm2);
			std::move(it->second.begin(), it->second.begin() + 2, std::back_inserter(positions));
			getWavePoints(wav1, wav2);



			//cout << "positions.back(): " << positions[positions.size() - 4] << " " << positions[positions.size() - 3] << endl;
			positions.insert(positions.end(), std::begin(firstInters), std::end(firstInters));
			//cout << "imm1: " << imm1 << " imm2:" << imm2 << endl;

			createIndices();


			++it;
		}



		/*for (const auto& entry : mapIntersectionPoints) {
			std::move(entry.second.begin(), entry.second.begin() + 2, std::back_inserter(positions));


		}*/

		for (int i = 0; i < positions.size(); i += 2) {
			cout << positions[i] << " " << positions[i + 1] << endl;
		}



	}

	void createIndices() {
		indices.clear();
		/*for (unsigned int i = 0; i < positions.size() / 2 - 1; i++) {
			indices.insert(indices.end(), { i,i + 1 });
		}*/
		
		//indices.insert(indices.end(), { 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5 ,6 ,6, 7 ,7, 8, 8 ,9, 9, 10, 10, 11 });
		indices.insert(indices.end(), { 0, 1, 1, 2, 2, 3, 3, 4, 4, 5,/* 5 ,6 ,*/6, 7 ,7, 8, 8 ,9, 9, 10, 10, 11});//Tendría que poder cerrar esto, 

		cout << "indices" << endl;
		for (unsigned int i = 0; i < indices.size(); i++) {
			cout << indices[i] << " ";
		}cout << endl;
	}
	






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
	vector<float> centroid;
	float force;


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
		 indicesAll.clear(); indicesRemaining.clear();
		
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
		cout << "Positions size: " << positions.size() / 2 << endl;
		cout << "Positions:" << endl;
		for (int i = 0; i < positions.size(); i += 2) {
			cout << positions[i] << " ";
			cout << positions[i + 1] << endl;
		}
		cout << endl;

		cout << "Indices:" << endl;
		for (int i = 0; i < indices.size(); i++) {
			cout << indices[i] << " ";
		}
		cout << endl;

		cout << "indicesRemaining:" << endl;
		for (int i = 0; i < indicesRemaining.size(); i++) {
			cout << indicesRemaining[i] << " ";
		}
		cout << endl;

		cout << "indicesAll:" << endl;
		for (int i = 0; i < indicesAll.size(); i++) {
			cout << indicesAll[i] << " ";
		}
		cout << endl;

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
		glBindVertexArray(0);

		glBindVertexArray(VAPolygonClosed);
		glBindBuffer(GL_ARRAY_BUFFER, VBPolygonClosed);
		glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * 4, positions.data());
		glDrawElements(GL_TRIANGLES, triangleIndices.size(), GL_UNSIGNED_INT, nullptr);
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


};