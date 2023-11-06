#pragma once


#include "utilities.h"
#include "Polygons.h"//para barycentric, no me hace gracia, globalvariables?
#include <algorithm>
#include <map>

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

bool isPointBetween(float P, float A, float B) { //point is between but not equal
	float minVal = B, maxVal = A;
	if (A < B) {
		minVal = A;
		maxVal = B;
	}
	return (P > minVal && P < maxVal);
}

float isRightOfLine(float& Ax, float& Ay, float& Bx, float& By, float& Px, float& Py) {		 //is P to the right of AB?
	float AB[2] = { Bx - Ax,  By - Ay };
	float AP[2] = { Px - Ax, Py - Ay };

	float crossProductZ = AB[0] * AP[1] - AB[1] * AP[0];

	return crossProductZ;	//if negative it is to its right, if 0, P is on the infinite line of AB
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

	float period = 2000;
	float frecuency = 2 * PI / period;
	float amplitude = 0;
	float offset = 500;
	float phase = 2 * PI + PI / 2;		//Esto no deja de crecer
	float phaseSpeed = 0.020;

	void createWavePositions() {
		double segmentsLength = endXPosition / segments;
		double interm = 0;
		positions.clear();
		positions.reserve(2 * xn);

		for (int i = 0; i < xn; ++i, interm += segmentsLength) {
			positions.emplace_back(interm);
			positions.emplace_back(amplitude * sin(frecuency * interm + phase) + offset);
		}
		phase += phaseSpeed;
		//offset++;
	}




	vector<vector<float>> allInsidePoints; vector<float>insidePoints;
	vector<unsigned int> polygonInmediatePositions;
	vector<float> intersectionPoints;
	vector<float> insideWavePoints;

	std::map<int, std::vector<float>> mapIntersectionPoints;

	vector <int>intersectionsJ;

	vector<float>& polygonPositions;
	vector<unsigned int>& polygonIndices;
	vector<unsigned int>& triangleIndices;

	FourierMesh(vector<float>& polygonPositions_, vector<unsigned int>& polygonIndices_, vector<unsigned int>& triangleIndices_)
		: polygonPositions(polygonPositions_), polygonIndices(polygonIndices_), triangleIndices(triangleIndices_) {
	}


	void createWettedPositions() {


		polygonInmediatePositions.clear(); allInsidePoints.clear(); insideWavePoints.clear();
		intersectionsJ.clear();
		intersectionPoints.clear();
		mapIntersectionPoints.clear();

		if (calculateFirstIntersection()) {
			calculateIntersections();
			mergeWettedPositions();
		}









		for (const auto& entry : mapIntersectionPoints) {
			std::cout << "Key: " << entry.first << ",   Values: ";
			for (float value : entry.second) {
				std::cout << value << " ";
			}
			std::cout << std::endl;
		}std::cout << std::endl;


		for (const auto& entry : mapIntersectionPoints) {
			std::move(entry.second.begin(), entry.second.begin() + 2, std::back_inserter(intersectionPoints));
			intersectionPoints.insert(intersectionPoints.end(), { polygonPositions[entry.second[2]], polygonPositions[entry.second[2] + 1] });

		}

		for (int i = 0; i < intersectionPoints.size(); i += 2) {
			cout << intersectionPoints[i] << " " << intersectionPoints[i + 1] << endl;
		}

		/*for (const auto& entry : possibleFirstIntersection) {
		std::cout << "Key: " << entry.first << ",   Values: ";
		for (float value : entry.second) {
			std::cout << value << " ";
		}
		std::cout << std::endl;
	}std::cout << std::endl;*/





	}

	void ValidatePoint(int& i, int& j) {
		float Px, Py;

		std::map<int, std::vector<float>> possibleIntersection;
		bool intersectionCandidate = 0;

		if (calculateIntersectionPoints(positions[i - 2], positions[i - 1], positions[i], positions[i + 1], polygonPositions[polygonIndices[j] * 2],
			polygonPositions[polygonIndices[j] * 2 + 1], polygonPositions[(polygonIndices[j] + 1) * 2], polygonPositions[(polygonIndices[j] + 1) * 2 + 1], Px, Py)) {

			possibleIntersection.clear();

			possibleIntersection.emplace(j, vector<float>{Px, Py, float(j)});

			intersectionCandidate = true;
			cout << "aquí" << endl;
		}

		if (intersectionCandidate) {
			intersectionCandidate = false;
			float crossProductWP = isRightOfLine(positions[i - 2], positions[i - 1], positions[i], positions[i + 1], polygonPositions[polygonIndices[j] * 2], polygonPositions[polygonIndices[j] * 2 + 1]);
			cout << "j" << " " << j << ", crosProdructWP " << crossProductWP << endl;
			if (crossProductWP == 0) {	//if it is in the point we are checking j+1 and j-1, if any of them is at the right they will be the inmediates, if both are or are not, none
				int indexMinus, indexPlus;
				if (j == 0) {
					indexMinus = polygonIndices[polygonIndices.size() - 2];
					indexPlus = polygonIndices[1];
				}
				else if (j == polygonIndices.size() - 2) {//the last index is [0], it should never check for j=back
					indexMinus = polygonIndices[j] - 1;
					indexPlus = polygonIndices[0];
				}
				else {
					indexMinus = polygonIndices[j] - 1;
					indexPlus = polygonIndices[j] + 1;
				}


				float crossProductInmMinus = isRightOfLine(positions[i - 2], positions[i - 1], positions[i], positions[i + 1], polygonPositions[indexMinus * 2], polygonPositions[indexMinus * 2 + 1]);
				float crossProductInmPlus = isRightOfLine(positions[i - 2], positions[i - 1], positions[i], positions[i + 1], polygonPositions[indexPlus * 2], polygonPositions[indexPlus * 2 + 1]);


				if (crossProductInmPlus < 0 && crossProductInmMinus >= 0) {
					mapIntersectionPoints.emplace(j, std::move(possibleIntersection[j]));
					mapIntersectionPoints[j][2] += 2;

				}


				if (0 < 0 && crossProductInmPlus >= 0) {
					mapIntersectionPoints.emplace(j, std::move(possibleIntersection[j]));	//Comprobar que este caso se pueda dar siquiera e inmediatos
					mapIntersectionPoints[j][2] -= 2;

				}


				//if none it continue to the next for entry?

			}

			else if (crossProductWP > 0) {
				mapIntersectionPoints.emplace(j, std::move(possibleIntersection[j]));
				mapIntersectionPoints[j][2] += 2;

			}
			else if (crossProductWP < 0) {		//este caso nunca va a darse para el punto inicial?
				mapIntersectionPoints.emplace(j, std::move(possibleIntersection[j]));


			}


			//	possibleIntersection.erase(j);//if it gets to here, it is not a valid intersection
		}
	}

	void calculateIntersections() {


		std::map<int, std::vector<float>> possibleIntersection;
		bool intersectionCandidate = 0;


		for (int j = mapIntersectionPoints.begin()->first + 2; j < polygonIndices.size(); j += 2) {		//this does not work for multiple intersections in one j
			for (int i = 2; i < positions.size(); i += 2) {

				ValidatePoint(i, j);
			}
		}
		for (int j = 0; j < mapIntersectionPoints.begin()->first; j += 2) {
			for (int i = 2; i < positions.size(); i += 2) {

				ValidatePoint(i, j);
			}
		}
	}

	bool calculateFirstIntersection() {
		float Px, Py;

		bool firstIntersectionFound = false;

		std::map<int, std::vector<float>> possibleFirstIntersection;



		for (int i = 2; i < positions.size(); i += 2) {

			if (!firstIntersectionFound) {
				for (int j = 0; j < polygonIndices.size(); j += 2) {	//detects all the intersections in an only wave		//j values will be twice as big because indices has repeated layout

					if (calculateIntersectionPoints(positions[i - 2], positions[i - 1], positions[i], positions[i + 1], polygonPositions[polygonIndices[j] * 2],
						polygonPositions[polygonIndices[j] * 2 + 1], polygonPositions[(polygonIndices[j] + 1) * 2], polygonPositions[(polygonIndices[j] + 1) * 2 + 1], Px, Py)) {

						firstIntersectionFound = true;

						possibleFirstIntersection.emplace(j, vector<float>{Px, Py, float(j)});	//it should take an array, not a vector

					}
				}



				if (firstIntersectionFound) {
					while (mapIntersectionPoints.empty()) {	//you are going out of here once a valid intersection is found or all the invalid ones discarded

						int firstKey = -1;	//-1 won't be a entry in this case
						float smallestPx = std::numeric_limits<float>::max();	//standard way of calling the maximum value of a float


						for (const auto& entry : possibleFirstIntersection) {
							if (!entry.second.empty() && entry.second[0] < smallestPx) {

								smallestPx = entry.second[0];
								firstKey = entry.first;
							}
						}


						if (firstKey == -1) { return false; }




						float crossProductWP = isRightOfLine(positions[i - 2], positions[i - 1], positions[i], positions[i + 1], polygonPositions[polygonIndices[firstKey] * 2], polygonPositions[polygonIndices[firstKey] * 2 + 1]);

						if (crossProductWP == 0) {	//if it is in the point we are checking j+1 and j-1, if any of them is at the right they will be the inmediates, if both are or are not, none
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


							float crossProductInmMinus = isRightOfLine(positions[i - 2], positions[i - 1], positions[i], positions[i + 1], polygonPositions[indexMinus * 2], polygonPositions[indexMinus * 2 + 1]);
							float crossProductInmPlus = isRightOfLine(positions[i - 2], positions[i - 1], positions[i], positions[i + 1], polygonPositions[indexPlus * 2], polygonPositions[indexPlus * 2 + 1]);


							if (crossProductInmPlus < 0 && crossProductInmMinus >= 0) {
								mapIntersectionPoints.emplace(firstKey, std::move(possibleFirstIntersection[firstKey]));
								mapIntersectionPoints[firstKey][2] += 2;
								return true;
							}


							if (0 < 0 && crossProductInmPlus >= 0) {
								mapIntersectionPoints.emplace(firstKey, std::move(possibleFirstIntersection[firstKey]));	//Comprobar que este caso se pueda dar siquiera e inmediatos
								mapIntersectionPoints[firstKey][2] -= 2;
								return true;
							}


							//if none it will end in erase

						}
						else if (crossProductWP > 0) {
							mapIntersectionPoints.emplace(firstKey, std::move(possibleFirstIntersection[firstKey]));
							mapIntersectionPoints[firstKey][2] += 2;
							return true;
						}
						else if (crossProductWP < 0) {		//este caso nunca va a darse para el punto inicial?
							mapIntersectionPoints.emplace(firstKey, std::move(possibleFirstIntersection[firstKey]));

							return true;
						}


						possibleFirstIntersection.erase(firstKey);//if it gets to here, it is not a valid intersection

					}

				}
				else return false;

			}

		}

	}

	void getInsideWavePoints(vector<float> polygonPositions, vector<unsigned int> polygonIndices, vector<unsigned int> triangleIndices) {//no unsa indices
		int a, b, c;
		vector<int> intermediate;
		for (int i = 0; i < positions.size(); i += 2) { //iterating through water points
			for (int j = 0; j < triangleIndices.size(); j += 3) { //checking through each polygon line
				a = triangleIndices[j], b = triangleIndices[j + 1], c = triangleIndices[j + 2];
				if (checkBarycentric(positions[i], positions[i + 1], polygonPositions[a * 2], polygonPositions[a * 2 + 1], polygonPositions[b * 2], polygonPositions[b * 2 + 1], polygonPositions[c * 2], polygonPositions[c * 2 + 1])) {
					insideWavePoints.insert(insideWavePoints.end(), { positions[i], positions[i + 1] });
					intermediate.push_back(i / 2);
					break;
				}

			}
		}



	}


	void NOTgetInmediates(vector<float> polygonPositions, vector<unsigned int> polygonIndices, int i) {
		if (polygonInmediatePositions[i] == polygonInmediatePositions[i + 1]) {
			insidePoints.insert(insidePoints.end(), { polygonPositions[polygonInmediatePositions[i] * 2],polygonPositions[polygonInmediatePositions[i] * 2 + 1] });
		}
		else if (polygonInmediatePositions[i + 1] > polygonInmediatePositions[i]) {
			for (int j = polygonInmediatePositions[i]; j <= polygonInmediatePositions[i + 1]; j++) {
				insidePoints.insert(insidePoints.end(), { polygonPositions[j * 2],polygonPositions[j * 2 + 1] });
			}
		}
		else {
			for (int j = polygonInmediatePositions[i]; j < polygonIndices.size() / 2; j++) {
				insidePoints.insert(insidePoints.end(), { polygonPositions[j * 2],polygonPositions[j * 2 + 1] });
			}

			for (int j = 0; j <= polygonInmediatePositions[i + 1]; j++) {
				insidePoints.insert(insidePoints.end(), { polygonPositions[j * 2],polygonPositions[j * 2 + 1] });
			}
		}
		polygonInmediatePositions.erase(polygonInmediatePositions.begin() + i, polygonInmediatePositions.begin() + i + 2);
	}

	void getImmediates(int& key1,int& key2) {
		if (polygonPositions[key1] == polygonPositions[key2]) {
			//insidePoints.insert(insidePoints.end(), { polygonPositions[polygonInmediatePositions[i] * 2],polygonPositions[polygonInmediatePositions[i] * 2 + 1] });
		}
	}

	void mergeWettedPositions() {
		

		auto it = mapIntersectionPoints.begin();

		while (it != mapIntersectionPoints.end()) {
			auto key1 = it->first;
			auto key2 = (++it)->first;
			getImmediates(key1, key2);
			++it; 
		}




	}

	//void MALmergeWettedPositions(vector<float> polygonPositions, vector<unsigned int> polygonIndices) {	//anti clock-wise
	//	float min, max;

	//	vector <float>remainingIntersectionPoints = intersectionPoints;
	//	//vector <float> remainingInmediatePositions = polygonInmediatePositions


	//	//cout << "intersectionPoints.size(): "<< intersectionPoints.size() << endl;

	//	/*for (int i = 0; i < intersectionPoints.size(); i += 2) {
	//		insidePoints.insert(insidePoints.end(), { intersectionPoints[i],intersectionPoints[i + 1] });

	//	}
	//	allInsidePoints.insert(allInsidePoints.end(), insidePoints);*/



	//	for (int i = 0; i < 12; i += 4) {//tienes que hacer aquí un predict de cuantos loops hay
	//		insidePoints.clear();

	//		//{
	//		//	cout << endl << "start: " << endl;
	//		//	cout << "polygonInmediatePositions" << endl;
	//		//	for (int i = 0; i < polygonInmediatePositions.size(); i++) {
	//		//		cout << polygonInmediatePositions[i] << " ";
	//		//	}
	//		//	cout << endl;

	//		//	cout << "remainingIntersectionPoints" << endl;
	//		//	for (int i = 0; i < remainingIntersectionPoints.size(); i++) {
	//		//		cout << remainingIntersectionPoints[i] << " ";
	//		//	}
	//		//	cout << endl;
	//		//	cout << endl;
	//		//}


	//		vector<float> startPoint = { remainingIntersectionPoints[0],remainingIntersectionPoints[1] };
	//		vector<float> endPoint = { remainingIntersectionPoints[2],remainingIntersectionPoints[3] };



	//		insidePoints.insert(insidePoints.end(), startPoint.begin(), startPoint.end());
	//		getInmediates(polygonPositions, polygonIndices, 0);
	//		insidePoints.insert(insidePoints.end(), endPoint.begin(), endPoint.end());
	//		remainingIntersectionPoints.erase(remainingIntersectionPoints.begin(), remainingIntersectionPoints.begin() + 4);//deque para más eficiencia


	//		//if (startPoint[0] < endPoint[0]) {
	//		//	bool checkMoreIntersections = true;
	//		//	float lastPoint = endPoint[0];


	//		//	//{
	//		//	//	cout << endl << "iteration: " << i / 4 << endl;
	//		//	//	cout << "polygonInmediatePositions" << endl;
	//		//	//	for (int i = 0; i < polygonInmediatePositions.size(); i++) {
	//		//	//		cout << polygonInmediatePositions[i] << " ";
	//		//	//	}
	//		//	//	cout << endl;
	//		//	//	cout << "remainingIntersectionPoints" << endl;
	//		//	//	for (int i = 0; i < remainingIntersectionPoints.size(); i++) {
	//		//	//		cout << remainingIntersectionPoints[i] << " ";
	//		//	//	}
	//		//	//	cout << endl;
	//		//	//	cout << endl;
	//		//	//}

	//		//	while (checkMoreIntersections) {

	//		//		//cout << endl<< "hola" << endl;
	//		//		int higherInt = -1;
	//		//		for (int j = 0; j < remainingIntersectionPoints.size(); j += 2) {

	//		//			if (remainingIntersectionPoints[j]< lastPoint && remainingIntersectionPoints[j] >startPoint[0]) {

	//		//				if (higherInt == -1) {
	//		//					higherInt = j;
	//		//				}

	//		//				if (remainingIntersectionPoints[j] > remainingIntersectionPoints[higherInt]) {
	//		//					//cout << "remainingIntersectionPoints[j]: " << remainingIntersectionPoints[j] <<" "<< remainingIntersectionPoints[higherInt]<< endl;
	//		//					higherInt = j;

	//		//				}
	//		//			}
	//		//		}


	//		//		if (higherInt != -1) {
	//		//			//cout << "higherInt: "<< higherInt<< endl;
	//		//			//cout << remainingIntersectionPoints[higherInt] << endl;
	//		//			//cout << "higherInt: " << higherInt << endl;
	//		//			//cout << "points que se quitan: " << remainingIntersectionPoints[higherInt] << " " << remainingIntersectionPoints[higherInt + 1] << " " << remainingIntersectionPoints[higherInt + 2] << " " << remainingIntersectionPoints[higherInt + 3] << endl << endl;
	//		//			//cout << "j que se quitan: " << higherInt / 2 << " " << higherInt / 2 + 1 << endl << endl;



	//		//			//cout << "lastpoint: "<< lastPoint << " polygonInmediatePositions[i]: " << " " << polygonPositions[polygonInmediatePositions[i]*2] << " polygonInmediatePositions[i+1]:" << " " << polygonPositions[polygonInmediatePositions[i + 1] *2]<< endl;
	//		//			for (int j = insideWavePoints.size() - 2; j >= 0; j -= 2) {//wave points
	//		//				if (insideWavePoints[j] > polygonPositions[polygonInmediatePositions[higherInt / 2] * 2] && insideWavePoints[j] < lastPoint) {
	//		//					insidePoints.insert(insidePoints.end(), { insideWavePoints[j],insideWavePoints[j + 1] });
	//		//				}
	//		//			}
	//		//			insidePoints.insert(insidePoints.end(), { remainingIntersectionPoints[higherInt],remainingIntersectionPoints[higherInt + 1] });

	//		//			getInmediates(polygonPositions, polygonIndices, higherInt / 2);

	//		//			insidePoints.insert(insidePoints.end(), { remainingIntersectionPoints[higherInt + 2],remainingIntersectionPoints[higherInt + 3] });
	//		//			lastPoint = remainingIntersectionPoints[higherInt + 2];
	//		//			remainingIntersectionPoints.erase(remainingIntersectionPoints.begin() + higherInt, remainingIntersectionPoints.begin() + higherInt + 4);

	//		//		}
	//		//		else {
	//		//			checkMoreIntersections = false;
	//		//			for (int j = insideWavePoints.size() - 2; j >= 0; j -= 2) {//wave points
	//		//				if (insideWavePoints[j] > startPoint[0] && insideWavePoints[j] < lastPoint) {
	//		//					insidePoints.insert(insidePoints.end(), { insideWavePoints[j],insideWavePoints[j + 1] });
	//		//				}
	//		//			}

	//		//		}
	//		//	}
	//		//	//cout << "adios" << endl;
	//		//	insidePoints.insert(insidePoints.end(), startPoint.begin(), startPoint.end());
	//		//}



	//		//if (startPoint[0] > endPoint[0]) {
	//		//	bool checkMoreIntersections = true;
	//		//	float lastPoint = endPoint[0];


	//		//	//{
	//		//	//	//cout << endl << "iteration: " << i / 4 << endl;
	//		//	//	cout << "polygonInmediatePositions" << endl;
	//		//	//	for (int i = 0; i < polygonInmediatePositions.size(); i++) {
	//		//	//		cout << polygonInmediatePositions[i] << " ";
	//		//	//	}
	//		//	//	cout << endl;
	//		//	//	cout << "remainingIntersectionPoints" << endl;
	//		//	//	for (int i = 0; i < remainingIntersectionPoints.size(); i++) {
	//		//	//		cout << remainingIntersectionPoints[i] << " ";
	//		//	//	}
	//		//	//	cout << endl;
	//		//	//	cout << endl;
	//		//	//}


	//		//	while (checkMoreIntersections) {

	//		//		//cout << endl<< "hola" << endl;
	//		//		int higherInt = -1;
	//		//		//cout << "remainingIntersectionPoints.size(): " << remainingIntersectionPoints.size() << endl;
	//		//		for (int j = 0; j < remainingIntersectionPoints.size(); j += 2) {

	//		//			if (remainingIntersectionPoints[j]< lastPoint) {
	//		//				//cout << "si " << endl;

	//		//				if (higherInt == -1) {
	//		//					higherInt = j;
	//		//				}

	//		//				if (remainingIntersectionPoints[j] > remainingIntersectionPoints[higherInt]) {
	//		//					higherInt = j;

	//		//				}
	//		//			}
	//		//		}
	//		//		//cout << "salgo " << endl;

	//		//		if (higherInt != -1) {
	//		//			//cout << "higherInt: "<< higherInt<< endl;
	//		//			//cout << remainingIntersectionPoints[higherInt] << endl;
	//		//			//cout << "higherInt: " << higherInt << endl;
	//		//			//cout << "points que se quitan: " << remainingIntersectionPoints[higherInt] << " " << remainingIntersectionPoints[higherInt + 1] << " " << remainingIntersectionPoints[higherInt + 2] << " " << remainingIntersectionPoints[higherInt + 3] << endl << endl;
	//		//			//cout << "j que se quitan: " << higherInt / 2 << " " << higherInt / 2 + 1 << endl << endl;

	//		//			//cout << "if " << endl;

	//		//			//cout << "lastpoint: "<< lastPoint << " polygonInmediatePositions[i]: " << " " << polygonPositions[polygonInmediatePositions[i]*2] << " polygonInmediatePositions[i+1]:" << " " << polygonPositions[polygonInmediatePositions[i + 1] *2]<< endl;
	//		//			//for (int j = insideWavePoints.size() - 2; j >= 0; j -= 2) {//wave points
	//		//			//	if (insideWavePoints[j] > polygonPositions[polygonInmediatePositions[higherInt / 2] * 2] && insideWavePoints[j] < lastPoint) {
	//		//			//		insidePoints.insert(insidePoints.end(), { insideWavePoints[j],insideWavePoints[j + 1] });
	//		//			//	}
	//		//			//}
	//		//			insidePoints.insert(insidePoints.end(), { remainingIntersectionPoints[higherInt],remainingIntersectionPoints[higherInt + 1] });

	//		//			getInmediates(polygonPositions, polygonIndices, higherInt / 2);

	//		//			insidePoints.insert(insidePoints.end(), { remainingIntersectionPoints[higherInt + 2],remainingIntersectionPoints[higherInt + 3] });
	//		//			lastPoint = remainingIntersectionPoints[higherInt + 2];
	//		//			remainingIntersectionPoints.erase(remainingIntersectionPoints.begin() + higherInt, remainingIntersectionPoints.begin() + higherInt + 4);

	//		//		}
	//		//		else {
	//		//			//cout << "else " << endl;

	//		//			checkMoreIntersections = false;
	//		//			//for (int j = insideWavePoints.size() - 2; j >= 0; j -= 2) {//wave points
	//		//			//	if (insideWavePoints[j] > startPoint[0] && insideWavePoints[j] < lastPoint) {
	//		//			//		insidePoints.insert(insidePoints.end(), { insideWavePoints[j],insideWavePoints[j + 1] });
	//		//			//	}
	//		//			//}

	//		//		}
	//		//	}
	//		//	//cout << "adios" << endl;
	//		//	insidePoints.insert(insidePoints.end(), startPoint.begin(), startPoint.end());
	//		//}


	//		allInsidePoints.insert(allInsidePoints.end(), insidePoints);
	//	}



	//	//{
	//	//	//cout << "sizes: " << remainingIntersectionPoints.size() << " " << polygonInmediatePositions.size() << endl;
	//	//	cout << "remainingIntersectionPoints final" << endl;
	//	//	for (int i = 0; i < remainingIntersectionPoints.size(); i++) {
	//	//		cout << remainingIntersectionPoints[i] << " ";
	//	//	}
	//	//	cout << endl;

	//	//	cout << "polygonInmediatePositions final" << endl;
	//	//	for (int i = 0; i < polygonInmediatePositions.size(); i++) {
	//	//		cout << polygonInmediatePositions[i] << " ";
	//	//	}
	//	//	cout << endl;

	//	//}
	//}


	//cout << "insidePoints" << endl;
//for (int i = 0; i < insidePoints.size(); i++) {
//	cout << insidePoints[i] << " ";
//}
//cout << "" << endl;

	/*cout << " allInsidePoints.size(): " << allInsidePoints.size() << "  allInsidePoints[0].size(): " << "allInsidePoints[0].size()" << endl;
		for (int i = 0; i < allInsidePoints.size(); i++) {
			for (int j = 0; j < allInsidePoints[i].size(); j++) {
				std::cout << allInsidePoints[i][j] << " ";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;*/



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
		printMatrix(intersectionPoints, "intersectionPoints", intersectionPoints.size() / 2, 2);

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