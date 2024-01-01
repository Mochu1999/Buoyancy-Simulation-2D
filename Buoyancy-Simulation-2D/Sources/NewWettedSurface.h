#pragma once




struct NewWettedSurface {

	vector<float>positions;
	//Cuando hagas un reserve tienes que estimar también el tamaño de wavespositons, quizás cogiendo el largo y multiplicandolo por la densidad en x de olas

	std::vector<std::pair<int, std::vector<float>>> mapIntersectionPoints;



	vector<float>& polygonPositions;
	vector<unsigned int>& polygonIndices;
	vector<float>& positionsFourier;



	Polygons polygon;


	//Deberías meter la instance de polygon mejor? //Creo que es mejor porque el hecho de que la cojas no implica coger más memoria
	NewWettedSurface(vector<float>& polygonPositions_, vector<unsigned int>& polygonIndices_, vector<float>& positionsFourier_)
		: polygonPositions(polygonPositions_), polygonIndices(polygonIndices_), positionsFourier(positionsFourier_)
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

		mapIntersectionPoints.clear();
		positions.clear();




		calculateIntersections();


		//////////////////////////////////////
		/*
		Que hay, solo falta organización y cambiarle el formato a mapIntersectionPoints, vas a tener que estudiarte si es siquiera necesario.
		Tu tarea es complicada macho porque vas a necesitar una perspectiva global de todo el proceso para ver que es redundante y que simplificar
		Dicho eso, falta tu tarea, lo de las intersecciones para el que vaya después de ti y esto lo damos por terminado hasta que volvamos 
		 aquí con un nuevo algoritmo para triangular, hacia las estrellas

		*/



		/*cout << "mapIntersectionPoints:" << endl;
		for (const auto& entry : mapIntersectionPoints) {
			std::cout << "{ " << entry.first << ", { ";
			for (float value : entry.second) {
				std::cout << value << " ";
			}
			std::cout << "}}" << std::endl;
		}std::cout << std::endl;*/


		/////reservar para todos los vectors


		//Ponte aquí alguna explicación crack
		if (mapIntersectionPoints.size())
		{

			

			//stores distances
			std::unordered_set<int> usedDistances;
			usedDistances.reserve(mapIntersectionPoints.size());

			vector<unsigned int> debugInfo; //to store the info for debug purposes

			std::vector<std::pair<int, std::vector<float>>>::iterator initialIt, firstIt, secondIt;
			unsigned int initialImm, firstImm, secondImm;
			unsigned int distanceInitialImm, distanceFirstImm, distanceSecondImm;

			unsigned int totalPolygonIndices = polygonIndices.back() * 2;






			std::unordered_map<int, vector<int>> mapSharedSegments;
			for (int i = 0; i < mapIntersectionPoints.size(); ++i) {
				int num = mapIntersectionPoints[i].first;
				mapSharedSegments[num].push_back(i);
			}





			//outer loop that defines initialIt and the first firstIt
			//returns here at the start of each surface
			while (usedDistances.size() < mapIntersectionPoints.size() )
			{


				distanceInitialImm = 0;


				//with sets, elements are located faster using a hash table (efficient af look for it and try it in other places)
				//Each element is unique, it automatically prevents duplicates
				while (usedDistances.find(distanceInitialImm) != usedDistances.end())
				{
					++distanceInitialImm; //increments till you find one that is not in the set
				}




				distanceFirstImm = distanceInitialImm;


				initialIt = mapIntersectionPoints.begin() + distanceInitialImm;

				firstIt = initialIt;


				initialImm = initialIt->second[2];
				firstImm = initialImm;

				positions.insert(positions.end(), { firstIt->second[0], firstIt->second[1] });


				//Premise: no distance in a surface will be higher than that of the first secondImm
				unsigned int distancefirstSecondImm = std::numeric_limits<unsigned int>::max();



				std::size_t distanceCheck = -1;// invalid distance to enter the while loop


				//You get here with firstIt already known. Calculates secondIt, goes from first to second with getImmediates if applies
				// ,and second to the next first with getWavePoints. Stays here till the surface closes
				while (distanceCheck != 0)
				{
					unsigned int segmentFirstImm = mapIntersectionPoints[distanceFirstImm].first;

					bool areImmediates = 1; //activates getImmediates


					//defining secondImm and distanceSecondImm
					//Different cases for when is initialIt and when it is not
					{
						if (firstIt == initialIt)
						{
							//////esto le quita mucha claridad, lo mismo hacer dos functiones para secondIt solo por readability

							//third case. When first and second intersection share same segment



							bool foundThirdCase = false;
							//if there is a third case it will be from initial+1 to the end
							if (mapSharedSegments[segmentFirstImm].size() > 1)
							{
								for (size_t j : mapSharedSegments[segmentFirstImm])
								{
									if (j > distanceFirstImm)
									{
										if (polygonPositions[firstImm] > polygonPositions[mapIntersectionPoints[j].second[2]])
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

								secondImm = mapIntersectionPoints[distanceSecondImm].second[2];
								secondIt = mapIntersectionPoints.begin() + distanceSecondImm;

								distancefirstSecondImm = distanceSecondImm;


							}

							else
							{


								//First case, looks for the imm closest to initial imm
								//If multiple share the same imms it takes the nearest in distance												

								//checks all valid distances and saves here the current one for the next check
								int currentMinImm = std::numeric_limits<int>::max();

								unsigned int summingDistance = totalPolygonIndices - initialImm;//for later

								//Checking from the firt valid distance till the end of the distances
								for (size_t j = distanceInitialImm + 1; j < mapIntersectionPoints.size(); j++)
								{
									//We use a corrected "j" that is relative to initialImm instead of the original indices
									unsigned int correctedImmJ;
									if (mapIntersectionPoints[j].second[2] < initialImm)
									{
										//if the original index of secondImm was between 0 and initialImm, now it will by larger by a factor
										//that is totalIndices-initialImm
										correctedImmJ = mapIntersectionPoints[j].second[2] + summingDistance;
									}
									else
									{
										//if secondImm is greater than initial imm (therefor less than max index), and initialImm has 
										// become 0, its difference will be the relative distance to initialImm
										correctedImmJ = mapIntersectionPoints[j].second[2] - initialImm;
									}


									if (correctedImmJ < currentMinImm)
									{
										//j can't be on the same segment as i unless it is dist+1 (otherwise it will take the one that is in the same
										// segment instead of the one that it should be because is correctedImmJ=0 although is farther)
										if ((mapIntersectionPoints[j].first != mapIntersectionPoints[distanceFirstImm].first) || j == distanceInitialImm + 1)
										{
											currentMinImm = correctedImmJ;
											distanceSecondImm = j; //iterative process
										}///can this be removed when it exists a case for only 2 intersection.size()==2?
									}
								}
							}

							//we end with the correct distanceSecondImm

							//Y esta advertencia tío? No entiendo

							secondImm = mapIntersectionPoints[distanceSecondImm].second[2];
							secondIt = mapIntersectionPoints.begin() + distanceSecondImm;

							distancefirstSecondImm = distanceSecondImm;



						}

						//firstSecondImm is already found and the premise is that we won't have a distance greater than its till the surface closes
						else
						{

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
										if (polygonPositions[firstImm] < polygonPositions[mapIntersectionPoints[j].second[2]])
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

								secondImm = mapIntersectionPoints[distanceSecondImm].second[2];
								secondIt = mapIntersectionPoints.begin() + distanceSecondImm;


							}




							else
							{

								//reverse first case 
								unsigned int summingDistance = totalPolygonIndices - initialImm;


								int currentMinImm = std::numeric_limits<int>::max();





								for (size_t j = distanceFirstImm - 1; j > distanceInitialImm; j--)
								{

									//for initial case I was changing correctedImmJ to be relative to initialImm, not everything is relative to firstImm
									unsigned int correctedImmJ;
									if (mapIntersectionPoints[j].second[2] < firstImm)
									{
										//if the original index of secondImm was between 0 and firstImm, now it will by larger by a factor
										//that is totalIndices-initialImm
										correctedImmJ = mapIntersectionPoints[j].second[2] + summingDistance;
									}
									else
									{
										//if secondImm is greater than initial imm (therefor less than max index), and firstImm has 
										// become 0, its difference will be the relative distance to firstImm
										correctedImmJ = mapIntersectionPoints[j].second[2] - firstImm;
									}




									if (correctedImmJ <= currentMinImm)
									{
										currentMinImm = correctedImmJ;
										distanceSecondImm = j;
									}


								}


								secondImm = mapIntersectionPoints[distanceSecondImm].second[2];
								secondIt = mapIntersectionPoints.begin() + distanceSecondImm;


							}

						}
					}




					if (areImmediates) {
						getImmediates(firstImm, secondImm);
					}
					positions.insert(positions.end(), { secondIt->second[0], secondIt->second[1] });









					usedDistances.emplace(distanceFirstImm);
					usedDistances.emplace(distanceSecondImm);

					debugInfo.insert(debugInfo.end(), { firstImm, distanceFirstImm, secondImm, distanceSecondImm });


					//The key in all of this. secondIt is linked to its pair, to be used in the next iteration
					if (distanceSecondImm % 2 == 0) {
						distanceFirstImm = distanceSecondImm + 1;
					}
					else {
						distanceFirstImm = distanceSecondImm - 1;
					}



					//for the next iteration
					firstIt = mapIntersectionPoints.begin() + distanceFirstImm;
					firstImm = firstIt->second[2];


					getWavePoints(firstIt->second[0], secondIt->second[0]);


					positions.insert(positions.end(), { firstIt->second[0], firstIt->second[1] });



					distanceCheck = distanceInitialImm - distanceFirstImm; //if 0 you have arrived to initialIt again and the loop ends

				}



				/*cout << "debugInfo: " << endl;
				for (int i = 0; i < debugInfo.size(); i += 4) {
					cout << debugInfo[i] << " " << debugInfo[i + 1] << ", " << debugInfo[i + 2] << " " << debugInfo[i + 3] << endl;
				}cout << endl;*/
			}

		}


		/*cout << "positions: " << endl;
		for (int i = 0; i < positions.size(); i += 2) {
			cout << positions[i] << " " << positions[i + 1] << endl;
		}cout << endl;*/

		//if (mapIntersectionPoints.size()) {
		//	mergeWettedPositions();
		//}
		//else {
		//	float biggestY = std::numeric_limits<float>::min();
		//	int biggestj = 0;
		//	for (int j = 0; j < polygonIndices.size(); j += 2) {	//finds uppermost polygon index
		//		if (polygonPositions[j + 1] > biggestY) {
		//			biggestY = polygonPositions[j + 1];
		//			biggestj = j;
		//		}
		//	}
		//	int biggesti = 0;
		//	for (int i = 0; i < positionsFourier.size(); i += 2) {	//finds the wave just previous to biggestj
		//		if (positionsFourier[i] < polygonPositions[biggestj]) {
		//			biggesti = i;
		//		}
		//		else break;	//it found it
		//	}
		//	if (isRightOfLine(positionsFourier[biggesti], positionsFourier[biggesti + 1], positionsFourier[biggesti + 2], positionsFourier[biggesti + 3],
		//		polygonPositions[polygonIndices[biggestj] * 2], polygonPositions[polygonIndices[biggestj] * 2 + 1]) <= 0) {
		//		positions = polygonPositions;
		//		//createIndices(positions);
		//		//triangleIndices = triangleIndices_;
		//	}
		//}




	}

	void getWavePoints(float firstX, float secondX) {//x values of the intersections. Will go from secondX to firstX

		float interval = positionsFourier[2] - positionsFourier[0]; // Take this out from here

		int firstIndex = (firstX - positionsFourier[0]) / interval * 2;
		int secondIndex = (secondX - positionsFourier[0]) / interval * 2;


		// Ensure even indices
		firstIndex -= firstIndex % 2;
		secondIndex -= secondIndex % 2;


		// Collect points between firstIndex and secondIndex
		for (int i = secondIndex; i > firstIndex; i -= 2) {
			positions.insert(positions.end(), { positionsFourier[i], positionsFourier[i + 1] });
		}
	}

	


	void calculateIntersections() { //fills mapIntersectionPoints with intersections and immediates indices

		float Px, Py;
		bool intersectionFound = false;

		std::vector<std::pair<int, std::vector<float>>> possibleIntersections;
		mapIntersectionPoints.reserve(polygonIndices.size() / 2);

		for (int i = 0; i < positionsFourier.size() - 2; i += 2) {
			possibleIntersections.clear();


			if (!intersectionFound) {

				for (int j = 0; j < polygonIndices.size(); j += 2) {	//j values will be twice as big because indices has repeated layout, convenient to avoid multiplying


					if (calculateIntersectionPoints(positionsFourier[i], positionsFourier[i + 1], positionsFourier[i + 2], positionsFourier[i + 3], polygonPositions[polygonIndices[j] * 2],
						polygonPositions[polygonIndices[j] * 2 + 1], polygonPositions[(polygonIndices[j] + 1) * 2], polygonPositions[(polygonIndices[j] + 1) * 2 + 1], Px, Py)) {

						intersectionFound = true;

						possibleIntersections.push_back({ j, {Px, Py, float(j)} });							//I think I don't need j for anythin
						//cout << "possibleIntersections: " << j << " " << Px << " " << Py << endl;
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
							float crossProductInmPlus = isRightOfLine(positionsFourier[i], positionsFourier[i + 1], positionsFourier[i + 2], positionsFourier[i + 3], polygonPositions[indexPlus * 2], polygonPositions[indexPlus * 2 + 1]);

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
								mapIntersectionPoints.emplace_back(possibleIntersections[k]);//Que es esto? 
								possibleIntersections[k].second[2] = indexPlus * 2;
								mapIntersectionPoints.emplace_back(std::move(possibleIntersections[k]));
							}

							//if none it won't add anything to map
						}



						else if (crossProductWP > 0) {


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

	void getImmediates(int firstImm, int secondImm) { //It adds the immediates and those in-between to the positions

		if (firstImm < secondImm) {


			for (int i = firstImm; i <= secondImm; i += 2) {

				positions.insert(positions.end(), { polygonPositions[i], polygonPositions[i + 1] });
			}
		}
		else if (secondImm < firstImm) {
			for (int i = firstImm; i < polygonIndices.size(); i += 2) {
				positions.insert(positions.end(), { polygonPositions[i],polygonPositions[i + 1] });
			}
			for (int i = 0; i <= secondImm; i += 2) {
				positions.insert(positions.end(), { polygonPositions[i],polygonPositions[i + 1] });
			}
		}
		else if (firstImm == secondImm) {

			positions.insert(positions.end(), { polygonPositions[firstImm],polygonPositions[firstImm + 1] });
		}

	}

};