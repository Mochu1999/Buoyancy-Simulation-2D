#pragma once
#include "FourierMesh.h"
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

//the advantage of not returning a bool is that you can compare if it is 0 (right on the line)
inline float isRightOfLine(float& Ax, float& Ay, float& Bx, float& By, float& Px, float& Py) {		 //is P to the right of AB?
	float AB[2] = { Bx - Ax,  By - Ay };
	float AP[2] = { Px - Ax, Py - Ay };

	float crossProductZ = AB[0] * AP[1] - AB[1] * AP[0];

	return crossProductZ;	//if negative it is to its right, if 0, P is on the infinite line of AB
}

struct NewWettedSurface {

	vector<float>positions;

	std::vector<std::array<float, 4>> intersections;



	Polygons& inputPolygon;

	vector<float>& polygonPositions = inputPolygon.positions;
	vector<unsigned int>& polygonIndices = inputPolygon.dlines.indices;


	Polygons outputPolygon;



	FourierMesh& fourier;
	vector<float>& fourierPositions = fourier.dlines.positions; //for reducing the overhead of dereferencing the member each time

	NewWettedSurface(Polygons& inputPolygon_, FourierMesh& fourier_)
		: inputPolygon(inputPolygon_), fourier(fourier_)
	{}


	void draw() {
		createWettedPositions();


		/*if (positions.size()) {
			polygon.clear();
			polygon.addSet(positions);
			polygon.draw();
		}*/
	}




	void createWettedPositions() {

		intersections.clear();
		intersections.reserve(30);

		calculateIntersections();

		positions.clear();




		//If there are intersections, it creates the positions of the wetted surface
		if (intersections.size())
		{
			/////Check this is true with the Cherno video
			int aproximatingFourierRange =
				(intersections[intersections.size() - 1][0] - intersections[0][0]) * fourier.intervalInverse + 2;
			positions.reserve(polygonPositions.size() + aproximatingFourierRange);

			//cout << "intersections:" << endl;
			//for (const auto& entry : intersections) {
			//	std::cout << "{ " << entry[2] << ", { ";
			//	for (float value : entry.second) {
			//		std::cout << value << " ";
			//	}
			//	std::cout << "}}" << std::endl;
			//}std::cout << std::endl;



			//stores distances
			std::unordered_set<int> usedDistances;
			usedDistances.reserve(intersections.size());

			//vector<unsigned int> debugInfo; //to store the info for debug purposes

			std::vector<std::array<float, 4>>::iterator initialIt, firstIt, secondIt;
			unsigned int initialImm, firstImm, secondImm;
			unsigned int distanceInitialImm, distanceFirstImm, distanceSecondImm;







			//map of to know what elements share a segment
			std::unordered_map<int, vector<int>> mapSharedSegments;
			mapSharedSegments.reserve(intersections.size());

			for (int i = 0; i < intersections.size(); ++i)
			{
				int num = intersections[i][2];
				mapSharedSegments[num].push_back(i);
			}





			//outer loop, stays here untill all surfaces are closed
			while (usedDistances.size() < intersections.size())
			{


				distanceInitialImm = 0;


				//looks for distanceInitialImm, being it the first non used distance of the intersections
				while (usedDistances.find(distanceInitialImm) != usedDistances.end())
				{
					++distanceInitialImm; //increments till you find one that is not in the set
				}


				initialIt = intersections.begin() + distanceInitialImm;
				initialImm = (*initialIt)[3];

				firstIt = initialIt;
				firstImm = initialImm;

				distanceFirstImm = distanceInitialImm;



				positions.emplace_back((*firstIt)[0]);
				positions.emplace_back((*firstIt)[1]);

				//Premise: no distance in a surface will be higher than that of the first secondImm
				unsigned int distancefirstSecondImm = std::numeric_limits<unsigned int>::max();



				int distanceCheck = -1;// invalid distance to enter the while loop




				//Stays here till the surface closes. You get here with firstIt already known. Calculates secondIt, goes from first to second
				//  with getImmediates if applies,and second to the next first with getWavePoints
				while (distanceCheck != 0)
				{


					bool areImmediates = 1; //activates getImmediates


					//defining secondImm and distanceSecondImm

					if (firstIt == initialIt)
					{
						distanceSecondImm = calculateSecondIt_Initial(areImmediates, distancefirstSecondImm
							, mapSharedSegments, distanceInitialImm, firstImm, initialImm);

						secondImm = intersections[distanceSecondImm][3];
						secondIt = intersections.begin() + distanceSecondImm;

						distancefirstSecondImm = distanceSecondImm;

					}

					//firstSecondImm is already found and the premise is that we won't have a distance greater than its till the surface closes
					else
					{
						distanceSecondImm = calculateSecondIt_notInitial(areImmediates, distancefirstSecondImm
							, mapSharedSegments, distanceFirstImm, distanceInitialImm, firstImm, initialImm);


						secondImm = intersections[distanceSecondImm][3];
						secondIt = intersections.begin() + distanceSecondImm;
					}






					if (areImmediates) {
						getImmediates(firstImm, secondImm);
					}
					positions.emplace_back((*secondIt)[0]);
					positions.emplace_back((*secondIt)[1]);



					usedDistances.emplace(distanceFirstImm);
					usedDistances.emplace(distanceSecondImm);

					//debugInfo.insert(debugInfo.end(), { firstImm, distanceFirstImm, secondImm, distanceSecondImm });


					//The key in all of this. secondIt is linked to its pair, to be used in the next iteration
					if (distanceSecondImm % 2 == 0) {
						distanceFirstImm = distanceSecondImm + 1;
					}
					else {
						distanceFirstImm = distanceSecondImm - 1;
					}



					//for the next iteration
					firstIt = intersections.begin() + distanceFirstImm;
					firstImm = (*firstIt)[3];


					getWavePoints((*firstIt)[0], (*secondIt)[0]);


					positions.emplace_back((*firstIt)[0]);
					positions.emplace_back((*firstIt)[1]);

					distanceCheck = distanceInitialImm - distanceFirstImm; //if 0 you have arrived to initialIt again and the loop ends

				}




				/*cout << "debugInfo: " << endl;
				for (int i = 0; i < debugInfo.size(); i += 4) {
					cout << debugInfo[i] << " " << debugInfo[i + 1] << ", " << debugInfo[i + 2] << " " << debugInfo[i + 3] << endl;
				}cout << endl;*/
			}

		}

		else //no intersections, we will check if a random index is under or over the water
		{
			float centroidX = inputPolygon.centroid[0] * 1000;
			float centroidY = inputPolygon.centroid[1] * 1000;

			int firstIndex = (centroidX - fourierPositions[0]) / fourier.interval * 2;


			// Ensure even indices
			firstIndex -= firstIndex % 2;

			int secondIndex = firstIndex + 2;

			if (isRightOfLine(fourierPositions[firstIndex], fourierPositions[firstIndex + 1]
				, fourierPositions[secondIndex], fourierPositions[secondIndex + 1], centroidX, centroidY) < 0)
			{
				positions.reserve(polygonPositions.size());
				positions = polygonPositions;

			}

		}


		/*cout << "positions: " << endl;
			for (int i = 0; i < positions.size(); i += 2) {
				cout << positions[i] << " " << positions[i + 1] << endl;
			}cout << endl;*/

	}

	unsigned int calculateSecondIt_Initial(bool& areImmediates, unsigned int& distancefirstSecondImm
		, std::unordered_map<int, vector<int>>& mapSharedSegments, unsigned int& distanceInitialImm, unsigned int& firstImm, unsigned int& initialImm) {

		unsigned int distanceSecondImm;

		unsigned int segmentFirstImm = intersections[distanceInitialImm][2];
		//third case. When first and second intersection share same segment
		bool foundThirdCase = false;
		//if there is a third case it will be from initial+1 to the end
		if (mapSharedSegments[segmentFirstImm].size() > 1)
		{
			for (size_t j : mapSharedSegments[segmentFirstImm])
			{
				if (j > distanceInitialImm)
				{
					if (polygonPositions[firstImm] > polygonPositions[intersections[j][3]])
					{
						foundThirdCase = true;

						distanceSecondImm = j;

						break;
					}
				}
			}
		}
		if (foundThirdCase)
		{
			areImmediates = 0;
		}

		else
		{


			//First case, looks for the imm closest to initial imm
			//If multiple share the same imms it takes the nearest in distance												

			//checks all valid distances and saves here the current one for the next check
			int currentMinImm = std::numeric_limits<int>::max();

			unsigned int summingDistance = polygonIndices.back() * 2 - initialImm;//for later

			//Checking from the firt valid distance till the end of the distances
			for (unsigned int j = distanceInitialImm + 1; j < intersections.size(); j++)
			{
				//We use a corrected "j" that is relative to initialImm instead of the original indices
				unsigned int correctedImmJ;
				if (intersections[j][3] < initialImm)
				{
					//if the original index of secondImm was between 0 and initialImm, now it will by larger by a factor
					//that is totalIndices-initialImm
					correctedImmJ = intersections[j][3] + summingDistance;
				}
				else
				{
					//if secondImm is greater than initial imm (therefor less than max index), and initialImm has 
					// become 0, its difference will be the relative distance to initialImm
					correctedImmJ = intersections[j][3] - initialImm;
				}


				if (correctedImmJ < currentMinImm)
				{
					//j can't be on the same segment as i unless it is dist+1 (otherwise it will take the one that is in the same
					// segment instead of the one that it should be because is correctedImmJ=0 although is farther)
					if ((intersections[j][2] != intersections[distanceInitialImm][2]) || j == distanceInitialImm + 1)
					{
						currentMinImm = correctedImmJ;
						distanceSecondImm = j; //iterative process
					}///can this be removed when it exists a case for only 2 intersection.size()==2?
				}
			}
		}


		//we end with the correct distanceSecondImm

		return distanceSecondImm;


	}


	unsigned int calculateSecondIt_notInitial(bool& areImmediates, unsigned int& distancefirstSecondImm
		, std::unordered_map<int, vector<int>>& mapSharedSegments, unsigned int& distanceFirstImm
		, unsigned int& distanceInitialImm, unsigned int& firstImm, unsigned int& initialImm)
	{
		unsigned int distanceSecondImm;

		unsigned int segmentFirstImm = intersections[distanceFirstImm][2];
		//reverse third case
		bool foundThirdCase = false;
		if (mapSharedSegments[segmentFirstImm].size() > 1)
		{
			for (auto it = mapSharedSegments[segmentFirstImm].end(); it != mapSharedSegments[segmentFirstImm].begin();)
			{
				it--;//this is here to cover .begin() (and not specifying end()+1
				size_t j = *it;
				if (j < distanceFirstImm)
				{
					if (polygonPositions[firstImm] < polygonPositions[intersections[j][3]])
					{
						foundThirdCase = true;

						distanceSecondImm = j;

						break;
					}
				}

			}
		}
		if (foundThirdCase)
		{
			areImmediates = 0;
		}


		else
		{

			//reverse first case 
			unsigned int summingDistance = polygonIndices.back() * 2 - initialImm;


			int currentMinImm = std::numeric_limits<int>::max();





			for (size_t j = distanceFirstImm - 1; j > distanceInitialImm; j--)
			{

				//for initial case I was changing correctedImmJ to be relative to initialImm, not everything is relative to firstImm
				unsigned int correctedImmJ;
				if (intersections[j][3] < firstImm)
				{
					//if the original index of secondImm was between 0 and firstImm, now it will by larger by a factor
					//that is totalIndices-initialImm
					correctedImmJ = intersections[j][3] + summingDistance;
				}
				else
				{
					//if secondImm is greater than initial imm (therefor less than max index), and firstImm has 
					// become 0, its difference will be the relative distance to firstImm
					correctedImmJ = intersections[j][3] - firstImm;
				}




				if (correctedImmJ <= currentMinImm)
				{
					currentMinImm = correctedImmJ;
					distanceSecondImm = j;
				}


			}
		}
		return distanceSecondImm;
	}


	void getWavePoints(float firstX, float secondX) {//imputs are the positions of the intersections. Will go from secondX to firstX


		int firstIndex = (firstX - fourierPositions[0]) * fourier.intervalInverse * 2;
		int secondIndex = (secondX - fourierPositions[0]) * fourier.intervalInverse * 2;


		// Ensure even indices
		firstIndex -= firstIndex % 2;
		secondIndex -= secondIndex % 2;


		// Collect points between firstIndex and secondIndex
		for (int i = secondIndex; i > firstIndex; i -= 2) 
		{
			positions.emplace_back(fourierPositions[i]);
			positions.emplace_back(fourierPositions[i + 1]);
		}
	}




	void calculateIntersections() { //fills intersections with intersections, segments and immediates 

		float Px, Py;

		for (int i = 0; i < fourierPositions.size() - 2; i += 2)
		{

			//polygonIndices j values will be twice as big because indices has repeated layout, convenient to avoid multiplying
			for (int j = 0; j < polygonIndices.size(); j += 2)
			{

				if (calculateIntersectionPoints(fourierPositions[i], fourierPositions[i + 1], fourierPositions[i + 2]
					, fourierPositions[i + 3], polygonPositions[polygonIndices[j] * 2], polygonPositions[polygonIndices[j] * 2 + 1]
					, polygonPositions[(polygonIndices[j] + 1) * 2], polygonPositions[(polygonIndices[j] + 1) * 2 + 1], Px, Py))
				{

					std::array<float, 4 > possibleIntersections = { Px, Py ,j ,j };



					int firstKey = possibleIntersections[2];


					float crossProductWP = isRightOfLine(fourierPositions[i], fourierPositions[i + 1], fourierPositions[i + 2], fourierPositions[i + 3]
						, polygonPositions[polygonIndices[firstKey] * 2], polygonPositions[polygonIndices[firstKey] * 2 + 1]);



					if (crossProductWP > 0)
					{
						possibleIntersections[3] += 2;
						intersections.emplace_back(std::move(possibleIntersections));
					}
					else if (crossProductWP < 0)
					{
						intersections.emplace_back(std::move(possibleIntersections));
					}
					//when is on a point. It doesn't detect intersection if both immediates are over or under the point
					else if (crossProductWP == 0)
					{
						int indexMinus, indexPlus;

						if (firstKey == 0)
						{
							indexMinus = polygonIndices[polygonIndices.size() - 2];
							indexPlus = polygonIndices[1];
						}
						else if (firstKey == polygonIndices.size() - 2)
						{//the last index is [0], it should never check for j=back
							indexMinus = polygonIndices[firstKey] - 1;
							indexPlus = polygonIndices[0];
						}
						else
						{
							indexMinus = polygonIndices[firstKey] - 1;
							indexPlus = polygonIndices[firstKey] + 1;
						}


						float crossProductInmMinus = isRightOfLine(fourierPositions[i], fourierPositions[i + 1], fourierPositions[i + 2]
							, fourierPositions[i + 3], polygonPositions[indexMinus * 2], polygonPositions[indexMinus * 2 + 1]);

						float crossProductInmPlus = isRightOfLine(fourierPositions[i], fourierPositions[i + 1], fourierPositions[i + 2]
							, fourierPositions[i + 3], polygonPositions[indexPlus * 2], polygonPositions[indexPlus * 2 + 1]);



						if (crossProductInmPlus < 0 && crossProductInmMinus >= 0)
						{

							possibleIntersections[3] = indexPlus * 2;
							intersections.emplace_back(std::move(possibleIntersections));

						}

						else if (crossProductInmMinus < 0 && crossProductInmPlus >= 0)
						{

							possibleIntersections[3] = indexMinus * 2;
							intersections.emplace_back(std::move(possibleIntersections));
						}



					}
					//if none it won't add anything to intersections
				}

			}


		}

		std::sort(intersections.begin(), intersections.end()
			, [](const std::array<float, 4>& a, const std::array<float, 4>& b)
			{
				return a[0] < b[0];
			});

	}

	//It adds the immediates and those in-between to the positions
	void getImmediates(int firstImm, int secondImm)
	{

		if (firstImm <= secondImm)
		{
			for (int i = firstImm; i <= secondImm; i += 2)
			{
				positions.emplace_back(polygonPositions[i]);
				positions.emplace_back(polygonPositions[i + 1]);
			}
		}
		else if (secondImm < firstImm)
		{
			for (int i = firstImm; i < polygonIndices.size(); i += 2)
			{
				positions.emplace_back(polygonPositions[i]);
				positions.emplace_back(polygonPositions[i + 1]);
			}
			for (int i = 0; i <= secondImm; i += 2)
			{
				positions.emplace_back(polygonPositions[i]);
				positions.emplace_back(polygonPositions[i + 1]);
			}
		}

	}

};