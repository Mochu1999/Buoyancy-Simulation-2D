#pragma once


bool calculateIntersectionPoints(float Ax, float Ay, float Bx, float By, float Cx, float Cy, float Dx, float Dy, float& ix, float& iy) {	//AB the waves, CD the polygon
	//It assumes AB,CD are not 0	//what if they are?

	float ABx = Bx - Ax;
	float ABy = By - Ay;
	float CDx = Dx - Cx;
	float CDy = Dy - Cy;

	float ACx = Cx - Ax;
	float ACy = Cy - Ay;

	/*
	In 2d geometry the cross product is a scalar value, not a vector, it represents the area of their parallelogram
	t = 0: Intersection at point A.
	0 < t < 1: Intersection between A and B (on the line segment).
	t = 1: Intersection at point B.
	t < 0: Intersection lies before A
	t > 1: Intersection lies beyond B
	*/
	float precalculate = ABx * CDy - ABy * CDx;
	if (!precalculate) { // They are parallel
		// Check if C is on the line segment AB
		float scalarProjection = ((ACx * ABx) + (ACy * ABy)) / (ABx * ABx + ABy * ABy);
		float crossProduct = ACx * ABy - ACy * ABx;

		if (scalarProjection >= 0 && scalarProjection <= 1 && crossProduct == 0) {
			ix = Cx;
			iy = Cy;
			return true;
		}
		return false;
	}

	float t = (ACx * CDy - ACy * CDx) / precalculate; //(AC×CD)/(AB×CD)
	float u = (ACx * ABy - ACy * ABx) / precalculate;//(AC×AB)/(AB×CD)



	if (t >= 0 && t <= 1 && u >= 0 && u < 1) {	//Return false for intersection in D
		ix = Ax + t * ABx;
		iy = Ay + t * ABy;
		return true;
	}

	return false;
}


inline float isRightOfLine(float& Ax, float& Ay, float& Bx, float& By, float& Px, float& Py) {		 //is P to the right of AB?
	float AB[2] = { Bx - Ax,  By - Ay };
	float AP[2] = { Px - Ax, Py - Ay };

	float crossProductZ = AB[0] * AP[1] - AB[1] * AP[0];

	return crossProductZ;	//if negative it is to its right, if 0, P is on the infinite line of AB
}

struct WettedSurface {

	vector<float>positions;
	std::vector<std::pair<int, std::vector<float>>> mapIntersectionPoints;



	vector<float>& polygonPositions;
	vector<unsigned int>& polygonIndices;
	vector<float>& positionsFourier;

	WettedSurface(vector<float>& polygonPositions_, vector<unsigned int>& polygonIndices_, vector<float>& positionsFourier_)
		: polygonPositions(polygonPositions_), polygonIndices(polygonIndices_), positionsFourier(positionsFourier_) {
	}

	float area=0;
	float densityArea = 1000;
	float mass;
	float centroid[2] = { 0,0 };
	float force[2] = { 0,0 };

	void getUpwardForce() {
		mass = area* densityArea;//que hace aquí si solo o necesito una vez
		force[1] = 9.81 * mass;
	}

	void areaCalculation() {	//surveyor's formula
		area = 0; //must be here to avoid increasing area value if area is already created
		for (int i = 0; i < positions.size() - 2; i += 2) {
			area -= (positions[i + 2] - positions[i]) * (positions[i + 3] + positions[i + 1]) * 1e-6;
		}
		area /= 2.0;
	}

	void createWettedPositions(vector<unsigned int> triangleIndices_) {

		mapIntersectionPoints.clear();
		positions.clear();
		triangleIndices.clear();
		indices.clear();



		calculateIntersections();

		if (mapIntersectionPoints.size()) {
			mergeWettedPositions();
		}
		else {

			float biggestY = std::numeric_limits<float>::min();
			int biggestj = 0;
			for (int j = 0; j < polygonIndices.size(); j += 2) {	//finds uppermost polygon index

				if (polygonPositions[j + 1] > biggestY) {
					biggestY = polygonPositions[j + 1];
					biggestj = j;
				}
			}

			int biggesti = 0;
			for (int i = 0; i < positionsFourier.size(); i += 2) {	//finds the wave just previous to biggestj

				if (positionsFourier[i] < polygonPositions[biggestj]) {
					biggesti = i;
				}
				else break;	//it found it
			}
			if (isRightOfLine(positionsFourier[biggesti], positionsFourier[biggesti + 1], positionsFourier[biggesti + 2], positionsFourier[biggesti + 3],
				polygonPositions[polygonIndices[biggestj] * 2], polygonPositions[polygonIndices[biggestj] * 2 + 1]) <= 0) {
				positions = polygonPositions;
				createIndices(positions);
				triangleIndices = triangleIndices_;

			}

		}




	}




	void getWavePoints(vector<float>& intermPositions, float& wav1, float& wav2) {
		int minWav = std::min(wav1, wav2);
		int maxWav = std::max(wav1, wav2);

		for (int i = positionsFourier.size() - 2; i >= 0; i -= 2) {

			if (positionsFourier[i] > minWav && positionsFourier[i] < maxWav) {
				intermPositions.insert(intermPositions.end(), { positionsFourier[i],positionsFourier[i + 1] });
			}

		}

	}


	void calculateIntersections() {

		float Px, Py;
		bool intersectionFound = false;

		std::vector<std::pair<int, std::vector<float>>> possibleIntersections;
		mapIntersectionPoints.reserve(polygonIndices.size() / 2);

		for (int i = 0; i < positionsFourier.size() - 2; i += 2) {
			possibleIntersections.clear();


			if (!intersectionFound) {

				for (int j = 0; j < polygonIndices.size(); j += 2) {	//j values will be twice as big because indices has repeated layout, convenient to avoid multiplying
					//cout << "j: " << j << endl;
					//if (j == 4) {
					//	cout << j << " " << polygonPositions[polygonIndices[j] * 2] << " " << polygonPositions[polygonIndices[j] * 2 + 1] << endl;
					//}

					if (calculateIntersectionPoints(positionsFourier[i], positionsFourier[i + 1], positionsFourier[i + 2], positionsFourier[i + 3], polygonPositions[polygonIndices[j] * 2],
						polygonPositions[polygonIndices[j] * 2 + 1], polygonPositions[(polygonIndices[j] + 1) * 2], polygonPositions[(polygonIndices[j] + 1) * 2 + 1], Px, Py)) {

						intersectionFound = true;

						possibleIntersections.push_back({ j, {Px, Py, float(j)} });							//I think I don't need j for anythin
						//cout << "possibleIntersections: " << j << " " << Px << " " << Py << endl;
					}
				}


				if (intersectionFound) {

					intersectionFound = false;

					//cout << "possibleIntersections:" << endl;
					//for (const auto& entry : possibleIntersections) {
					//	std::cout << "{ " << entry.first << ", { ";
					//	for (float value : entry.second) {
					//		std::cout << value << " ";
					//	}
					//	std::cout << "}}" << std::endl;
					//}std::cout << std::endl;

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

							//cout <<"crossProductInmMinus: "<< indexMinus<<" " <<crossProductInmMinus <<endl<< 
							//	" crossProductInmPlus: " << indexPlus<<" "<< crossProductInmPlus << endl;

							if (crossProductInmPlus < 0 && crossProductInmMinus >= 0) {

								possibleIntersections[k].second[2] = indexPlus * 2;
								mapIntersectionPoints.emplace_back(std::move(possibleIntersections[k]));

							}

							else if (crossProductInmMinus < 0 && crossProductInmPlus >= 0) {

								possibleIntersections[k].second[2] = indexMinus * 2;
								mapIntersectionPoints.emplace_back(std::move(possibleIntersections[k]));
							}

							else if (crossProductInmMinus < 0 && crossProductInmPlus < 0) {
								possibleIntersections[k].second[2] = indexMinus * 2;
								mapIntersectionPoints.emplace_back(possibleIntersections[k]);
								possibleIntersections[k].second[2] = indexPlus * 2;
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

	vector<float> getImmediates(int key1, int key2) {
		vector<float> inmediatesVector;

		if (key1 < key2) {


			for (int i = key1; i <= key2; i += 2) {

				inmediatesVector.insert(inmediatesVector.end(), { polygonPositions[i], polygonPositions[i + 1] });
			}
		}
		if (key2 < key1) {
			for (int i = key1; i < polygonIndices.size(); i += 2) {
				inmediatesVector.insert(inmediatesVector.end(), { polygonPositions[i],polygonPositions[i + 1] });
			}
			for (int i = 0; i <= key2; i += 2) {
				inmediatesVector.insert(inmediatesVector.end(), { polygonPositions[i],polygonPositions[i + 1] });
			}
		}
		if (key1 == key2) {

			inmediatesVector.insert(inmediatesVector.end(), { polygonPositions[key1],polygonPositions[key1 + 1] });
		}
		return inmediatesVector;
	}

	int lastIndex;

	void mergeWettedPositions() {


		/*cout << "mapIntersectionPoints:" << endl;
		for (const auto& entry : mapIntersectionPoints) {
			std::cout << "{ " << entry.first << ", { ";
			for (float value : entry.second) {
				std::cout << value << " ";
			}
			std::cout << "}}" << std::endl;
		}std::cout << std::endl;*/


		lastIndex = 0;


		auto it = mapIntersectionPoints.begin();

		while (it != mapIntersectionPoints.end()) {
			vector<float> intermPositions;

			
			auto wav1 = it->second[0];
			auto imm1 = it->second[2];
			std::move(it->second.begin(), it->second.begin() + 2, std::back_inserter(intermPositions));

			float firstInters[2];
			std::move(it->second.begin(), it->second.begin() + 2, firstInters);


			++it;
			auto wav2 = it->second[0];
			auto imm2 = it->second[2];

			vector<float> inmediatesVector = getImmediates(imm1, imm2);
			
			intermPositions.insert(intermPositions.end(), inmediatesVector.begin(), inmediatesVector.end());
			std::move(it->second.begin(), it->second.begin() + 2, std::back_inserter(intermPositions));
			getWavePoints(intermPositions,wav1, wav2);



			intermPositions.insert(intermPositions.end(), std::begin(firstInters), std::end(firstInters));
			
			createIndices(intermPositions);
			
			positions.insert(positions.end(), std::begin(intermPositions), std::end(intermPositions));


			++it;

			
			/*cout << "intermPositions: " << endl;
			for (int i = 0; i < intermPositions.size(); i += 2) {
				cout << intermPositions[i] << " " << intermPositions[i + 1] << endl;
			}cout << endl;*/


		}
		

		/*if (checkBarycentric(300, 600, positions[10 * 2], positions[10 * 2 + 1], positions[0 * 2], positions[0 * 2 + 1], positions[1 * 2], positions[1 * 2 + 1])) {
			cout << " hay barycentric" << endl;
		}*/


		/*cout << "positions: " << endl;
		for (int i = 0; i < positions.size(); i += 2) {
			cout << positions[i] << ", " << positions[i + 1] << "," << endl;
		}cout << endl;*/




		/*cout << "triangleIndices:" << endl;
		for (int i = 0; i < triangleIndices.size(); i += 3) {
			cout << triangleIndices[i] << " ";
			cout << triangleIndices[i + 1] << " ";
			cout << triangleIndices[i + 2] << endl;
		}
		cout << endl;*/



	}

	void createIndices(vector<float>& intermPositions) {

		for (unsigned int i = 0; i < intermPositions.size() / 2 - 1; i++) {
			indices.insert(indices.end(), { i + lastIndex,i + 1 + lastIndex });
		}


		indicesAll.clear(); indicesRemaining.clear();

		for (unsigned int i = 0; i < intermPositions.size() / 2 - 1; i++) {
			indicesAll.insert(indicesAll.end(), { i });

		}
		indicesRemaining = indicesAll;

		/*cout << endl << endl;
		cout << "indices" << endl;
		for (unsigned int i = 0; i < indices.size(); i++) {
			cout << indices[i] << ", ";
		}cout << endl;*/
		generateTriangleIndices(intermPositions);



		lastIndex = indices.back() + 1;
	}







	vector <unsigned int> indices;
	vector <unsigned int> indicesAll;
	vector <unsigned int> indicesRemaining;
	vector<unsigned int> triangleIndices;

	unsigned int VBPolygonLines;
	unsigned int VAPolygonLines;

	unsigned int VBPolygonClosed;
	unsigned int VAPolygonClosed;

	
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

	void createPolygonsLines() {

		areaCalculation();
		/*if (area < 0) {
			for (int i = 0; i < positions.size() / 2; i += 2) {
				std::swap(positions[i], positions[positions.size() - 2 - i]);
				std::swap(positions[i + 1], positions[positions.size() - 1 - i]);
			}
			areaCalculation();
		}*/
		centroidCalculation();
		PolygonsLinesBuffer();
		CreatePolygonLinesIBO();

		glBindVertexArray(0);

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


		unsigned int ibo;
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * 4, indices.data(), GL_DYNAMIC_DRAW);
	}



	void show() {

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



	void linesDraw() {

		glBindVertexArray(0);
		glBindVertexArray(VAPolygonLines);
		glBindBuffer(GL_ARRAY_BUFFER, VBPolygonLines);
		glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * 4, positions.data());
		glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
	}






	void generateTriangleIndices(vector<float>& intermPositions) {	//grouping indices

		while (!indicesRemaining.empty()) {

			for (int i = 0; i < indicesRemaining.size(); i++) {

				if (indicesRemaining.size() == 3) {		//ends while loop, closes polygon

					triangleIndices.insert(triangleIndices.end(), { indicesRemaining[0] + lastIndex,indicesRemaining[1] + lastIndex,indicesRemaining[2] + lastIndex });
					indicesRemaining.clear(); //para que?
					break;
				}

				else {
					/*cout <<endl<< "triangleIndices:" << endl;
					for (int i = 0; i < triangleIndices.size(); i += 3) {
						cout << triangleIndices[i] << " ";
						cout << triangleIndices[i + 1] << " ";
						cout << triangleIndices[i + 2] << endl;
					}
					cout << endl;
					cout << "indicesRemaining:" << endl;
					for (int i = 0; i < indicesRemaining.size(); i++) {
						cout << indicesRemaining[i] << " ";
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

					//cout << "a: " << a << " b: " << b << " c: " << c << endl;

					bool barycentricFlag = 0;
					for (int k = 0; k < indicesAll.size(); k++) {	//are there points inside abc
						if (k != a && k != b && k != c) {
							if (checkBarycentric(intermPositions[k * 2], intermPositions[k * 2 + 1], intermPositions[a * 2], intermPositions[a * 2 + 1], intermPositions[b * 2], intermPositions[b * 2 + 1], intermPositions[c * 2], intermPositions[c * 2 + 1])) {
								//cout <<"k "<< k << endl;
								barycentricFlag = 1;
								break;
							}
						}
					}
					/*cout << "barycentricFlag " << barycentricFlag << endl;
					cout <<"isConcave: "<< isConcave(absoluteAngle(positions[b * 2] - positions[a * 2], positions[b * 2 + 1] - positions[a * 2 + 1]),
						absoluteAngle(positions[c * 2] - positions[b * 2], positions[c * 2 + 1] - positions[b * 2 + 1])) << endl;*/
					if (!barycentricFlag) {
						if (isConcave(absoluteAngle(intermPositions[b * 2] - intermPositions[a * 2], intermPositions[b * 2 + 1] - intermPositions[a * 2 + 1]),
							absoluteAngle(intermPositions[c * 2] - intermPositions[b * 2], intermPositions[c * 2 + 1] - intermPositions[b * 2 + 1]))) {				//is concave

							indicesRemaining.erase(indicesRemaining.begin() + i);		//errasing b and adding triangle
							triangleIndices.insert(triangleIndices.end(), { a + lastIndex,b + lastIndex,c + lastIndex });
							
							
							//return;
							break;
						}
					}


				}
			}
		}
	}

	void createClosedPolygon() {

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

	
	


};