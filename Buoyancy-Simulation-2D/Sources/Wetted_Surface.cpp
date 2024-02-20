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
//Tío, el algoritmo no funciona para concave arrow
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

					unsigned int size = polygonPositions.size() - 1;
					indexMinus = (j - 1 + size) % size;
					indexPlus = (j + 1) % size;


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


//positions reserve. Estoy convencido que los iterators y distancias son redundantes

//For each surface of the wetted surface it jumps a first imm into a second one till it closes. 
void NewWettedSurface::calculatePositions() {

	calculateIntersections();

	positions.clear();



	//If there are intersections, it creates the positions of the wetted surface
	if (intersections.size())
	{




		//stores distances. Distances are used to move through the vector that is intersection
		///////Es literalmente el uso de [index], no me creo que haga falta el uso de iteradores
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



			positions.emplace_back((*firstIt).point);

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

					secondImm = intersections[distanceSecondImm].imm;
					secondIt = intersections.begin() + distanceSecondImm;

					distancefirstSecondImm = distanceSecondImm;

				}

				//firstSecondImm is already found and the premise is that we won't have a distance greater than its till the surface closes
				else
				{
					distanceSecondImm = calculateSecondIt_notInitial(areImmediates, distancefirstSecondImm
						, mapSharedSegments, distanceFirstImm, distanceInitialImm, firstImm, initialImm);


					secondImm = intersections[distanceSecondImm].imm;
					secondIt = intersections.begin() + distanceSecondImm;
				}






				if (areImmediates) {
					getImmediates(firstImm, secondImm);
				}
				positions.emplace_back((*secondIt).point);



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
				firstImm = (*firstIt).imm;


				getWavePoints((*firstIt).point.x, (*secondIt).point.x);


				positions.emplace_back((*firstIt).point);

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
		//float centroidX = inputPolygon.centroid[0] * 1000;
		//float centroidY = inputPolygon.centroid[1] * 1000;

		//int firstIndex = (centroidX - fourierPositions[0]) / fourier.interval * 2;


		//// Ensure even indices
		//firstIndex -= firstIndex % 2;

		//int secondIndex = firstIndex + 2;

		//if (newisRightOfLine(fourierPositions[firstIndex], fourierPositions[firstIndex + 1]
		//	, fourierPositions[secondIndex], fourierPositions[secondIndex + 1], centroidX, centroidY) < 0)
		//{
		//	positions.reserve(polygonPositions.size());
		//	positions = polygonPositions;

		//}

	}

}





unsigned int NewWettedSurface::calculateSecondIt_Initial(bool& areImmediates, unsigned int& distancefirstSecondImm
	, std::unordered_map<int, vector<int>>& mapSharedSegments, unsigned int& distanceInitialImm, unsigned int& firstImm, unsigned int& initialImm) {

	unsigned int distanceSecondImm;

	unsigned int segmentFirstImm = intersections[distanceInitialImm].segment;

	
	bool foundThirdCase = false;
	//third case. When first and second intersection share same segment
	//if there is a third case it will be from initial+1 to the end
	if (mapSharedSegments[segmentFirstImm].size() > 1)
	{
		for (unsigned int j : mapSharedSegments[segmentFirstImm])
		{
			if (j > distanceInitialImm)
			{
				//// UFF... NO SÉ PORQUE ESTO ES VERDAD
				if (polygonPositions[firstImm].x > polygonPositions[intersections[j].imm].x)
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

		////LO DEJO POR AHORA, PERO INDICES.BACK() ES POSITIONS.SIZE(), NO? //HE QUITADO UN X2 DESPUES DE .BACK()
		unsigned int summingDistance = polygonIndices.back() - initialImm;//for later

		//Checking from the firt valid distance till the end of the distances
		for (unsigned int j = distanceInitialImm + 1; j < intersections.size(); j++)
		{
			//We use a corrected "j" that is relative to initialImm instead of the original indices
			unsigned int correctedImmJ;
			if (intersections[j].imm < initialImm)
			{
				//if the original index of secondImm was between 0 and initialImm, now it will by larger by a factor
				//that is totalIndices-initialImm
				correctedImmJ = intersections[j].imm + summingDistance;
			}
			else
			{
				//if secondImm is greater than initial imm (therefor less than max index), and initialImm has 
				// become 0, its difference will be the relative distance to initialImm
				correctedImmJ = intersections[j].imm - initialImm;
			}


			if (correctedImmJ < currentMinImm)
			{
				//j can't be on the same segment as i unless it is dist+1 (otherwise it will take the one that is in the same
				// segment instead of the one that it should be because is correctedImmJ=0 although is farther)
				if ((intersections[j].segment != intersections[distanceInitialImm].segment) || j == distanceInitialImm + 1)
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



unsigned int NewWettedSurface::calculateSecondIt_notInitial(bool& areImmediates, unsigned int& distancefirstSecondImm
	, std::unordered_map<int, vector<int>>& mapSharedSegments, unsigned int& distanceFirstImm
	, unsigned int& distanceInitialImm, unsigned int& firstImm, unsigned int& initialImm)
{
	unsigned int distanceSecondImm;

	unsigned int segmentFirstImm = intersections[distanceFirstImm].segment;
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
				if (polygonPositions[firstImm].x < polygonPositions[intersections[j].imm].x)
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
		unsigned int summingDistance = polygonIndices.back() - initialImm;


		int currentMinImm = std::numeric_limits<int>::max();




		 
		for (size_t j = distanceFirstImm - 1; j > distanceInitialImm; j--)
		{

			//for initial case I was changing correctedImmJ to be relative to initialImm, not everything is relative to firstImm
			unsigned int correctedImmJ;
			if (intersections[j].imm < firstImm)
			{
				//if the original index of secondImm was between 0 and firstImm, now it will by larger by a factor
				//that is totalIndices-initialImm
				correctedImmJ = intersections[j].imm + summingDistance;
			}
			else
			{
				//if secondImm is greater than initial imm (therefor less than max index), and firstImm has 
				// become 0, its difference will be the relative distance to firstImm
				correctedImmJ = intersections[j].imm - firstImm;
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


void NewWettedSurface::getImmediates(int firstImm, int secondImm)
{
	if (firstImm <= secondImm)
	{
		for (int i = firstImm; i <= secondImm; i ++)
		{
			positions.emplace_back(polygonPositions[i]);
		}
	}
	else if (secondImm < firstImm)
	{
		for (int i = firstImm; i < polygonPositions.size()-1; i ++)
		{
			positions.emplace_back(polygonPositions[i]);
		}
		for (int i = 0; i <= secondImm; i ++)
		{
			positions.emplace_back(polygonPositions[i]);
		}
	}

}

void NewWettedSurface::getWavePoints(float firstX, float secondX) {//imputs are the positions of the intersections. Will go from secondX to firstX


	int firstIndex = (firstX - fourierPositions[0].x) * fourier.intervalInverse *2;
	int secondIndex = (secondX - fourierPositions[0].x) * fourier.intervalInverse *2;


	// Ensure even indices
	firstIndex -= firstIndex % 2;
	secondIndex -= secondIndex % 2;

	// Collect points between firstIndex and secondIndex
	for (int i = secondIndex/2; i > firstIndex/2; i --)
	{
		
		positions.emplace_back(fourierPositions[i]);
	}
}