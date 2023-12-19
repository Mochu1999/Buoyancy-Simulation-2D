#pragma once

#include <unordered_set>

//maybe breaking the surface in no concave surfaces is faster...

struct NewWettedSurface {

	vector<float>positions;		//final positions of the wetted surface
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






		cout << "mapIntersectionPoints:" << endl;
		for (const auto& entry : mapIntersectionPoints) {
			std::cout << "{ " << entry.first << ", { ";
			for (float value : entry.second) {
				std::cout << value << " ";
			}
			std::cout << "}}" << std::endl;
		}std::cout << std::endl;






		if (mapIntersectionPoints.size()) {


			//vector that orders imms depending on the 3 intersection cases, normal, same immediate and same segment
			vector<vector<float>> orderedImms;


			//adding the immediate, the distance to begin, the segment and a flag for areImmediates in the third case
			float distanceCounter = 0;
			for (auto it = mapIntersectionPoints.begin(); it != mapIntersectionPoints.end(); ++it) {

				vector<float> interm = { it->second[2],distanceCounter ,static_cast<float>(it->first),1 };

				orderedImms.push_back(interm);

				distanceCounter++;
			}




			//secondImm will be firstImm+1 unless second or third cases where they will be ordered by distance
			std::sort(orderedImms.begin(), orderedImms.end(),
				[](const std::vector<float>& a, const std::vector<float>& b)
				{
					//same segment or same immediate condition
					//if (a[2] == b[2] )  //checking by segments and immediates
					//{
					//	return a[1] < b[1]; //the one with less distance
					//}
					if (a[0] == b[0])  //checking by segments and immediates
					{
						return a[1] < b[1]; //the one with less distance
					}


					return a[0] < b[0]; //normal condition, ordered by immediates
				}
			);



			size_t i = 1;
			//if the second or third case happens, you don't know if the order will be first to second or second to first,
			// so you wrap around the first one after the second one just in case  //I'm sure there is a complex way to know it beforehand but it may be less efficient overall
			//while (i < orderedImms.size())
			//{
			//	if (orderedImms[i][0] == orderedImms[i - 1][0])
			//	{
			//		vector<vector<float>> interm;
			//		interm.reserve(orderedImms.size() + 1);

			//		//It occurs to me a way that only adds the first insert and push_back so you don't have to do orderedImms = interm;
			//		interm.insert(interm.end(), orderedImms.begin(), orderedImms.begin() + i + 1);
			//		interm.push_back(orderedImms[i - 1]);
			//		interm.insert(interm.end(), orderedImms.begin() + i + 1, orderedImms.end());

			//		orderedImms = interm;
			//		i++;
			//	}

			//	i++;

			//}

			//float maxVal = orderedImms[0][0];
			//int lastIndex = 0;
			//for (int i = 0; i < orderedImms.size(); ++i) {
			//	if (orderedImms[i][0] >= maxVal) {
			//		maxVal = orderedImms[i][0];
			//		lastIndex = i;
			//	}
			//}
			//orderedImms.insert(orderedImms.begin() + lastIndex + 1, orderedImms[0]);
			orderedImms.push_back(orderedImms[0]); //normal case where the first intersection is the last index, wraps around
			/*
			A ver tío, si te has despertado no tendrías que estar aquí, tendrías que quitarte lo de fatiga de encima para estar
			full con esto por la tarde




			





			/////////////////////// No sé si esto es correcto
			//Changes the flags for the segment case once the immediate starts to decrease
			for (size_t i = 0; i < orderedImms.size() - 2; i++)
			{
				if (orderedImms[i + 1][0] < orderedImms[i][0])
				{
					orderedImms[i][3] = 0;
				}
			}




			/*cout << "orderedImms: " << endl;
			for (const auto& item : orderedImms) {
				cout << "{";
				for (const auto& val : item) {
					std::cout << val << ", ";
				}
				std::cout << "}," << std::endl;
			}cout << endl;*/



			//stores imms and distances
			vector<unsigned int> usedImms;

			std::vector<std::pair<int, std::vector<float>>>::iterator initialIt, firstIt, secondIt;
			unsigned int firstImm, secondImm;





			int count = 0;

			//this while loop defines initialIt and the first firstIt
			//returns here at the start of each surface
			while (usedImms.size() < mapIntersectionPoints.size() * 2) {

				unsigned int distanceInitialImm = 0;
				//with sets elements are located faster using a hash table (efficient af look for it and try it in other places)
				//Each element is unique, it automatically prevents duplicates
				std::unordered_set<int> secondElements;

				for (size_t i = 1; i < usedImms.size(); i += 2) {
					secondElements.insert(usedImms[i]);
				}
				while (secondElements.find(distanceInitialImm) != secondElements.end()) {
					++distanceInitialImm; //increments till you find one that is not in the set
				}




				unsigned int distanceFirstImm = distanceInitialImm;
				initialIt = mapIntersectionPoints.begin() + distanceInitialImm;


				firstIt = initialIt;
				firstImm = firstIt->second[2];

				positions.insert(positions.end(), { firstIt->second[0], firstIt->second[1] });



				std::size_t distanceCheck = -1;// invalid distance to enter the while loop



				//cout << endl << "count " << count; count++;
				int countInnerLoop = 0;






				//You get here with firstIt already known. Calculates secondIt with orderedImms
				// Goes from first to second with getImmediates if applies, and second to the next first with getWavePoints
				//stays here till the surface closes
				while (distanceCheck != 0) {

					/*cout << " .......countInnerLoop " << countInnerLoop << endl << endl; */
					countInnerLoop++;



					bool areImmediates; //takes the flag of same segments
					bool isSecondItFound = false;
					//looks for secondImm and secondIt with orderedImms

					





					//Vamos a ver, para que se produzca el tercer caso los immediates tienen que contradecirse, si no son
					// dos "casos 2" distintos
					//Como sabes si se contradicen? En el caso de la izq para 1 imm>segm y para 2 imm=seg
					//para el de la derecha también (así que eso no son parámetros a analizar)
					
					//idea loca, el caso 3 solo puede ocurrir para initialIt, prove me wrong
					areImmediates = 1;
					cout << "firstImm " << firstImm << " dist " << distanceFirstImm << endl;
					for (size_t i = 0; i < orderedImms.size(); i++)
					{
						if (firstImm == orderedImms[i][0] && distanceFirstImm == orderedImms[i][1])
						{
							orderedImms[i][3] = 0;

							if (i == 0)
							{
								orderedImms[orderedImms.size() - 1][3] = 0;
							}

							//la premisa que el tercer caso solo puede ocurrir para comienzo de superficie, prove me wrong
							if(firstIt==initialIt)
							{
								bool foundThirdCase = false;
								float currentDistance = std::numeric_limits<float>::max();
								size_t savedJ;
								for (size_t j = 1; j < orderedImms.size(); j++) //no estoy cogiendo el primer elemento repetido
								{
									if (j != i)
									{
										if (orderedImms[j][2] == orderedImms[i][2] && orderedImms[j][3] == 1)
										{
											foundThirdCase = true;

											if (orderedImms[j][1] > orderedImms[i][1] && orderedImms[j][1] < currentDistance)
											{
												currentDistance = orderedImms[j][1];
												savedJ = j;

											}

										}
									}
								}
								if (foundThirdCase)
								{
									areImmediates = 0;

									isSecondItFound = true;//coño es esto?
									secondImm = orderedImms[savedJ][0];

									secondIt = mapIntersectionPoints.begin() + orderedImms[savedJ][1];

									orderedImms[savedJ][3] = 0;

									break;
								}
							}
							

							if (orderedImms[i + 1][3] == 1)
							{
								isSecondItFound = true;
								secondImm = orderedImms[i + 1][0];
								orderedImms[i + 1][3] = 0;

								secondIt = mapIntersectionPoints.begin() + orderedImms[i + 1][1];

								break;
							}

							//if orderedImms[i + 1][3] !=1 and there is no third case
							isSecondItFound = true;
							secondImm = orderedImms[i - 1][0];
							orderedImms[i - 1][3] = 0;

							secondIt = mapIntersectionPoints.begin() + orderedImms[i - 1][1];

							break;



						}

					}








					if (areImmediates) {
						getImmediates(firstImm, secondImm);
					}
					positions.insert(positions.end(), { secondIt->second[0], secondIt->second[1] });



					//tío pero si secondImm ya lo estás sacando en el for de arriba, que haces
					unsigned int distanceSecondImm = std::distance(mapIntersectionPoints.begin(), secondIt);


					cout << "secondImm " << secondImm << " dist " << distanceSecondImm << endl;

					//cout << "firstImm " << firstImm << ", distanceFirstImm " << distanceSecondImm << endl;
					//cout << " secondImm " << secondImm << ", distanceSecondImm " << distanceSecondImm << endl;
					usedImms.insert(usedImms.end(), { firstImm, distanceFirstImm, secondImm, distanceSecondImm });



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

					/*if (countInnerLoop == 10) {
						cout << endl << "usedImms: " << endl;
						for (int i = 0; i < usedImms.size(); i += 4) {
							cout << usedImms[i] << " " << usedImms[i + 1] << ", " << usedImms[i + 2] << " " << usedImms[i + 3] << endl;
						}cout << endl;
					}*/
					cout << "orderedImms: " << endl;
					for (const auto& item : orderedImms) {
						cout << "{";
						for (const auto& val : item) {
							std::cout << val << ", ";
						}
						std::cout << "}," << std::endl;
					}cout << endl;
				}


				cout << endl << "usedImms: " << endl;
				for (int i = 0; i < usedImms.size(); i += 4) {
					cout << usedImms[i] << " " << usedImms[i + 1] << ", " << usedImms[i + 2] << " " << usedImms[i + 3] << endl;
				}cout << endl;
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

	//change this to a binary search where you find with this method the upper and lower bounds 
	void getWavePoints(float firstItXInters, float secondItXInters) {


		for (int i = positionsFourier.size() - 2; i >= 0; i -= 2) {

			if (positionsFourier[i] > firstItXInters && positionsFourier[i] < secondItXInters) {
				positions.insert(positions.end(), { positionsFourier[i],positionsFourier[i + 1] });
			}

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