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
void NewWettedSurface::calculateIntersections() {
	p point;
	intersections.clear();
	intersections.reserve(30);

	for (int i = 0; i < fourierPositions.size() - 1; i++)
	{
		// j = current polygon index
		for (unsigned int j = 0; j < polygonPositions.size() - 1; j++)
		{

			if (calculateIntersectionPoints(fourierPositions[i], fourierPositions[i + 1]
				, polygonPositions[j], polygonPositions[(j + 1)]
				, point))
			{
				//we do not add it directly into intersections because it may be an invalid point (when crossProduct == 0)
				Intersections possibleIntersection = { point,j,j };

				float crossProductJ = newisRightOfLine(fourierPositions[i], fourierPositions[i + 1], polygonPositions[j]);


				//If crossProductJ > 0, j is above the water, then the immediate is j++, else j is the immediate
				if (crossProductJ > 0)
				{
					possibleIntersection.imm++;
					intersections.emplace_back(std::move(possibleIntersection));
				}
				else if (crossProductJ < 0)
				{
					intersections.emplace_back(std::move(possibleIntersection));
				}


				//when the point is on a index, it takes the next and previous indices to decide if there is an intersection
				// It doesn't detect an intersection if both immediates are over or under the point. So one point must be under 
				// and the other above (or at 0) to be an intersection. This way we are forcing an even number of intersections
				else if (crossProductJ == 0)
				{
					int prevIndex, nextIndex;

					unsigned int size = polygonPositions.size() - 1;
					prevIndex = (j - 1 + size) % size;
					nextIndex = (j + 1) % size;


					float crossProductPrevIndex = newisRightOfLine(fourierPositions[i], fourierPositions[i + 1], polygonPositions[prevIndex]);

					float crossProductNextIndex = newisRightOfLine(fourierPositions[i], fourierPositions[i + 1], polygonPositions[nextIndex]);

					if (crossProductNextIndex < 0 && crossProductPrevIndex >= 0)
					{

						possibleIntersection.imm = nextIndex;
						intersections.emplace_back(std::move(possibleIntersection));

					}

					else if (crossProductNextIndex >= 0 && crossProductPrevIndex < 0)
					{

						possibleIntersection.imm = prevIndex;
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



	//very rarely the algorithm produces the same point twice,
	//  in that case they must be erased because we can't ensure that they are ordered properly
	for (int i = 0; i < static_cast<int>(intersections.size()) - 1; i++)
	{
		if (intersections[i].point.x == intersections[i + 1].point.x)
		{
			intersections.erase(intersections.begin() + i + 1);
			intersections.erase(intersections.begin() + i);
			i--;
		}
	}



	cout << "intersections" << endl;
	for (const auto& item : intersections)
	{
		cout << "{{" << item.point.x << "," << item.point.y << "},"
			<< item.segment << "," << item.imm << "}" << endl;
	}cout << endl;
}


//positions reserve.



//It links interesection points in pairs, jumping from a first one into a second one till all surfaces are closed
void NewWettedSurface::calculatePositions() {
	//cout << "inputPolygon.positions.size() " << inputPolygon.positions.size() << endl;
	calculateIntersections();


	positions.clear();



	//If there are intersections, it creates the positions of the wetted surface
	if (intersections.size())
	{
		//stores distances. Distances here are equivalent to the intersection indices 
		std::unordered_set<int> usedDistances;
		usedDistances.reserve(intersections.size());

		//vector<unsigned int> debugInfo; //to store the info for debug purposes

		//I'll be using iterator instead of direct indexing for no particular reason
		std::vector<Intersections>::iterator initialIt, firstIt, secondIt;
		unsigned int initialImm, firstImm, secondImm;
		unsigned int distanceInitialImm, distanceFirstImm, distanceSecondImm;

		//Tío pero si es distanceInitialIt? Que coño dice de imms. Los iterators tienen que morir





		//map to know what elements share a common segment
		// Each intersection point is assigned an entry in the map with the segment as the key, it will have multiple entries if shared segment
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


			//looks for distanceInitialImm, which it is the first non used distance of the intersections
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


			int distanceCheck = -1;// invalid distance to enter the while loop

			int whileCounter = 0;


			//Stays here till the surface closes. You get here with firstIt already known. Calculates secondIt, goes from first to second
			//  with getImmediates if applies,and second to the next first with getWavePoints
			while (distanceCheck != 0)
			{
				//cout << "whileCounter " << whileCounter << endl;
				if (whileCounter > 10)
				{
					continueRunning = false;
					cout << "aborting " << endl;
					return;
					std::exit(EXIT_FAILURE);
				}
				whileCounter++;

				bool areImmediates = 1; //activates getImmediates


				//defining distanceSecondImm and secondImm
				if (firstIt == initialIt)
					distanceSecondImm = calculateSecondIt_Initial(initialIt,distanceInitialImm);


				//firstSecondImm is already found and the premise is that we won't have a distance greater than it till the surface closes
				else
					distanceSecondImm = calculateSecondIt_notInitial(firstIt, distanceFirstImm, distanceInitialImm);

				//cout << "distanceFirstImm:" << distanceFirstImm << ", distanceSecondImm: " << distanceSecondImm << endl;

				secondImm = intersections[distanceSecondImm].imm;
				secondIt = intersections.begin() + distanceSecondImm;


				if ((*firstIt).segment == (*secondIt).segment)
					areImmediates = 0;



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




unsigned int NewWettedSurface::calculateSecondIt_Initial(std::vector<Intersections>::iterator& initialIt,unsigned int& distanceInitialImm) {

	//cout << "initial" << endl;

	unsigned int distanceSecondImm; //output

	unsigned int currentMinSegment = std::numeric_limits<unsigned int>::max();
	unsigned int correctedSegment; float savedX;


	//cout << "distanceFirstImm: " << distanceInitialImm << ", (*firstIt).segment: " << (*initialIt).segment << endl;
	for (unsigned int j = distanceInitialImm + 1; j < intersections.size(); j++)
	{
		//(*initialIt).segment and those bigger than it stay the same but those that are smaller are increased by positions.size()-1
		// so that everything is bigger than (*initialIt).segment). Those that were behind initialIt now are correctly the farther from it
		if (intersections[j].segment < (*initialIt).segment)
			correctedSegment = intersections[j].segment + polygonPositions.size() - 1;

		else
			correctedSegment = intersections[j].segment;

		bool continuing = true;

		//if j is in the same segment as initial it can't have less savedX than initial
		if ((*initialIt).segment == correctedSegment)
		{
			if ((abs(intersections[j].point.x - polygonPositions[intersections[j].segment].x)
				- abs((*initialIt).point.x - polygonPositions[intersections[j].segment].x))<0)
			{
				continuing = false;
				//cout << "se quita de la pull j = " << j << endl;
			}
		}
		if (continuing)
		{
			if (correctedSegment < currentMinSegment)
			{
				currentMinSegment = correctedSegment;

				savedX = abs(intersections[j].point.x - polygonPositions[intersections[j].segment].x); //for the third case

				distanceSecondImm = j;
			}
			else if (correctedSegment == currentMinSegment)
			{

				if (abs(intersections[j].point.x - polygonPositions[intersections[j].segment].x) < savedX)
				{
					currentMinSegment = correctedSegment;

					savedX = abs(intersections[j].point.x - polygonPositions[intersections[j].segment].x);

					distanceSecondImm = j;
				}
			}
			//cout << "j: " << j << ", intersections[j].segment: " << intersections[j].segment << ", correctedSegment: " << correctedSegment << endl;
		}
	}
	//cout << "distanceSecondImm: " << distanceSecondImm << endl << endl;
	return distanceSecondImm;
}

//Aquí ahora lo que estás buscando es el segment immediatamente más pequeño que firstIt
unsigned int NewWettedSurface::calculateSecondIt_notInitial(std::vector<Intersections>::iterator& firstIt
	, unsigned int& distanceFirstImm, unsigned int& distanceInitialImm)
{

	//cout << "not initial" << endl;

	unsigned int distanceSecondImm; //output

	unsigned int currentMinSegment = std::numeric_limits<unsigned int>::max();
	unsigned int correctedSegment; float savedX;

	//cout << "distanceFirstImm: " << distanceFirstImm << ", (*firstIt).segment: " << (*firstIt).segment << endl;

	for (int j = distanceFirstImm - 1; j > distanceInitialImm; j--)
	{
		//We still want the correctedSegment that is closer to it
		if (intersections[j].segment < (*firstIt).segment)
			correctedSegment = intersections[j].segment + polygonPositions.size() - 1;

		else
			correctedSegment = intersections[j].segment;

		if (correctedSegment < currentMinSegment)
		{

			currentMinSegment = correctedSegment;

			savedX = abs(intersections[j].point.x - polygonPositions[intersections[j].segment].x); //for the third case

			distanceSecondImm = j;
		}
		else if (correctedSegment == currentMinSegment)
		{
			if (abs(intersections[j].point.x - polygonPositions[intersections[j].segment].x) < savedX)
			{

				currentMinSegment = correctedSegment;

				savedX = abs(intersections[j].point.x - polygonPositions[intersections[j].segment].x);

				distanceSecondImm = j;
			}
		}
		//cout << "j: " << j << ", intersections[j].segment: " << intersections[j].segment << ", correctedSegment: " << correctedSegment << endl;
	}
	//cout << "distanceSecondImm: " << distanceSecondImm << endl << endl;


	return distanceSecondImm;
}


void NewWettedSurface::getImmediates(int firstImm, int secondImm)
{
	if (firstImm <= secondImm)
	{
		for (int i = firstImm; i <= secondImm; i++)
		{
			positions.emplace_back(polygonPositions[i]);
		}
	}
	else if (secondImm < firstImm)
	{
		for (int i = firstImm; i < polygonPositions.size() - 1; i++)
		{
			positions.emplace_back(polygonPositions[i]);
		}
		for (int i = 0; i <= secondImm; i++)
		{
			positions.emplace_back(polygonPositions[i]);
		}
	}

}

void NewWettedSurface::getWavePoints(float firstX, float secondX) {//imputs are the positions of the intersections. Will go from secondX to firstX


	int firstIndex = (firstX - fourierPositions[0].x) * fourier.intervalInverse * 2;
	int secondIndex = (secondX - fourierPositions[0].x) * fourier.intervalInverse * 2;


	// Ensure even indices
	firstIndex -= firstIndex % 2;
	secondIndex -= secondIndex % 2;

	// Collect points between firstIndex and secondIndex
	for (int i = secondIndex / 2; i > firstIndex / 2; i--)
	{

		positions.emplace_back(fourierPositions[i]);
	}
}