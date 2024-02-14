#pragma once

#include "newWetted_Surface.hpp"


bool NewWettedSurface::calculateIntersectionPoints(const p& A, const p& B, const p& C, const p& D, p& i) {
	//It is meant to have AB as the waves, CD as the polygon
	//It assumes AB, CD are not 0	//what if they are?
	p AB = { B.x - A.x ,B.y - A.y };
	p CD = { D.x - C.x ,D.y - C.y };
	p AC = { C.x - A.x ,C.y - A.y };

	/*
	In 2d geometry the cross product is a scalar value, not a vector, it represents the area of their parallelogram
	when:
	t = 0: Intersection at point A.
	0 < t < 1: Intersection between A and B (on the line segment).
	t = 1: Intersection at point B.
	t < 0: Intersection lies before A
	t > 1: Intersection lies beyond B
	*/
	float precalculate = AB.x * CD.y - AB.y * CD.x;
	if (!precalculate) { // They are parallel
		// Check if C is on the line segment AB
		float scalarProjection = ((AC.x * AB.x) + (AC.y * AB.y)) / (AB.x * AB.x + AB.y * AB.y);
		float crossProduct = AC.x * AB.y - AC.y * AB.x;

		if (scalarProjection >= 0 && scalarProjection <= 1 && crossProduct == 0) {
			i.x = C.x;
			i.y = C.y;
			return true;
		}
		return false;
	}

	float t = (AC.x * CD.y - AC.y * CD.x) / precalculate; // (AC×CD)/(AB×CD)
	float u = (AC.x * AB.y - AC.y * AB.x) / precalculate; // (AC×AB)/(AB×CD)

	if (t >= 0 && t <= 1 && u >= 0 && u < 1) { // Return false for intersection in D
		i.x = A.x + t * AB.x;
		i.y = A.y + t * AB.y;
		return true;
	}

	return false;
}


//hay que evitar el nested loop solo mirando si positions está en las condiciones de tener intersección
//testear el nuevo indexMinus, cambiar el nombre a isJrightOfWater, quizás? y entender y comentar la lógica de los cross products
void NewWettedSurface::calculateIntersections() {
	p point;
	intersections.clear();
	intersections.reserve(30);

	for (int i = 0; i < fourierPositions.size() - 1; i++)
	{
		
		for (unsigned int j = 0; j < polygonPositions.size() - 1; j++)
		{

			if (calculateIntersectionPoints(fourierPositions[i], fourierPositions[i + 1]
				, polygonPositions[j], polygonPositions[(j + 1)]
				, point))
			{
				Intersections possibleIntersection = { point,j,j };

				
				float isJrightOfWater = newisRightOfLine(fourierPositions[i], fourierPositions[i + 1], polygonPositions[j]);



				if (isJrightOfWater > 0)
				{
					possibleIntersection.imm++;
					intersections.emplace_back(std::move(possibleIntersection));
				}
				else if (isJrightOfWater < 0)
				{
					intersections.emplace_back(std::move(possibleIntersection));
				}

				//when point is on a polygon vertex. It doesn't detect intersection if both immediates are over or under the point
				else if (isJrightOfWater == 0)
				{
					int indexMinus, indexPlus;

					//unsigned int size = polygonPositions.size() - 1;
					//indexMinus = (j - 1 + size) % size;
					//indexPlus = (j + 1) % size;

					if (j == 0)
					{
						indexMinus = polygonPositions.size() - 1;
						indexPlus = j + 1;
					}
					else if (j == polygonPositions.size() - 2)
					{
						indexMinus = j - 1;
						indexPlus = 0;
					}
					else
					{
						indexMinus = j - 1;
						indexPlus = j + 1;
					}

					float crossProductInmMinus = newisRightOfLine(fourierPositions[i], fourierPositions[i + 1], polygonPositions[indexMinus]);

					float crossProductInmPlus = newisRightOfLine(fourierPositions[i], fourierPositions[i + 1], polygonPositions[indexPlus]);


					if (crossProductInmPlus < 0 && crossProductInmMinus >= 0)
					{

						possibleIntersection.imm = indexPlus;
						intersections.emplace_back(std::move(possibleIntersection));

					}

					else if (crossProductInmMinus < 0 && crossProductInmPlus >= 0)
					{

						possibleIntersection.imm = indexMinus;
						intersections.emplace_back(std::move(possibleIntersection));
					}



				}
				//if none it won't add anything to intersections
			}


		}
	}

	std::sort(intersections.begin(), intersections.end()
		, [](const Intersections& a, const Intersections& b)
		{
			return a.point.x < b.point.x;
		});


	/*cout << "intersections" << endl;
	for (const auto& item : intersections)
	{
		cout << "{{" << item.point.x << "," << item.point.y << "},"
			<< item.segment << "," << item.imm << "}" << endl;
	}cout << endl;*/
}


//positions reserve

//For each surface of the wetted surface it jumps a first imm into a second one till it closes. 
void NewWettedSurface::calculatePositions() {

	calculateIntersections();

	positions.clear();



	//If there are intersections, it creates the positions of the wetted surface
	if (intersections.size())
	{




		//stores distances
		std::unordered_set<int> usedDistances;
		usedDistances.reserve(intersections.size());

		//vector<unsigned int> debugInfo; //to store the info for debug purposes

		std::vector<Intersections>::iterator initialIt, firstIt, secondIt;
		unsigned int initialImm, firstImm, secondImm;
		unsigned int distanceInitialImm, distanceFirstImm, distanceSecondImm;







		//map of to know what elements share a segment
		std::unordered_map<int, vector<int>> mapSharedSegments;
		mapSharedSegments.reserve(intersections.size());

		for (int i = 0; i < intersections.size(); ++i)
		{
			int num = intersections[i].segment;
			mapSharedSegments[num].push_back(i);
		}





		//outer loop, stays here until all surfaces are closed
		while (usedDistances.size() < intersections.size())
		{


			distanceInitialImm = 0;


			//looks for distanceInitialImm, being it the first non used distance of the intersections
			while (usedDistances.find(distanceInitialImm) != usedDistances.end())
			{
				++distanceInitialImm; //increments till you find one that is not in the set
			}


			initialIt = intersections.begin() + distanceInitialImm;
			initialImm = (*initialIt).imm;

			firstIt = initialIt;
			firstImm = initialImm;

			distanceFirstImm = distanceInitialImm;



			positions.emplace_back((*firstIt).point.x);
			positions.emplace_back((*firstIt).point.y);

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

}







