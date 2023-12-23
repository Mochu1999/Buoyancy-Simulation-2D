#pragma once

#include <unordered_set>

//maybe breaking the surface in no concave surfaces is faster...

struct NewWettedSurface {

	vector<float>positions;		//final positions of the wetted surface
	//Cuando hagas un reserve tienes que estimar tambi�n el tama�o de wavespositons, quiz�s cogiendo el largo y multiplicandolo por la densidad en x de olas

	std::vector<std::pair<int, std::vector<float>>> mapIntersectionPoints;



	vector<float>& polygonPositions;
	vector<unsigned int>& polygonIndices;
	vector<float>& positionsFourier;



	Polygons polygon;


	//Deber�as meter la instance de polygon mejor? //Creo que es mejor porque el hecho de que la cojas no implica coger m�s memoria
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





		//the pipeline consists in start with one immediate, jump to the next one and repeat the process till getting where 
		// you started
		if (mapIntersectionPoints.size())
		{


			//vector that orders the immediates from smallest to largest
			vector<vector<float>> orderedImms;


			//adding the immediate, the distance to begin, the segment and a flag for areImmediates in the third case
			float distanceCounter = 0;
			for (auto it = mapIntersectionPoints.begin(); it != mapIntersectionPoints.end(); ++it)
			{

				vector<float> interm = { it->second[2],distanceCounter ,static_cast<float>(it->first),1 };

				orderedImms.push_back(interm);

				distanceCounter++;
			}




			//secondImm will be firstImm+1 unless case with same segments
			std::sort(orderedImms.begin(), orderedImms.end(),
				[](const std::vector<float>& a, const std::vector<float>& b)
				{
					if (a[0] == b[0])  //if same immediate
					{
						return a[1] < b[1]; //the one with less distance
					}


					return a[0] < b[0]; //normal condition, ordered by immediates
				}
			);





			orderedImms.push_back(orderedImms[0]); //normal case where the first intersection is the last index, wraps around







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
			unsigned int distanceInitialImm, distanceFirstImm, distanceSecondImm;




			int count = 0;

			//this while loop defines initialIt and the first firstIt
			//returns here at the start of each surface
			while (usedImms.size() < mapIntersectionPoints.size() * 2)
			{


				distanceInitialImm = 0;
				//with sets, elements are located faster using a hash table (efficient af look for it and try it in other places)
				//Each element is unique, it automatically prevents duplicates
				std::unordered_set<int> distancesSet;
				for (size_t i = 1; i < usedImms.size(); i += 2)
				{
					distancesSet.insert(usedImms[i]);
				}

				while (distancesSet.find(distanceInitialImm) != distancesSet.end())
				{
					++distanceInitialImm; //increments till you find one that is not in the set
				}




				distanceFirstImm = distanceInitialImm;
				initialIt = mapIntersectionPoints.begin() + distanceInitialImm;


				firstIt = initialIt;
				firstImm = firstIt->second[2];
				positions.insert(positions.end(), { firstIt->second[0], firstIt->second[1] });



				std::size_t distanceCheck = -1;// invalid distance to enter the while loop



				//cout << endl << "count " << count; count++;
				int countInnerLoop = 0;



				//Premise, no distance in a surface will be higher than that of the first secondImm
				unsigned int distancefirstSecondImm = std::numeric_limits<unsigned int>::max();





				//You get here with firstIt already known. Calculates secondIt with orderedImms
				// Goes from first to second with getImmediates if applies, and second to the next first with getWavePoints
				//stays here till the surface closes
				while (distanceCheck != 0  /*countInnerLoop < 3*/) {

					//cout << " .......countInnerLoop " << countInnerLoop << endl << endl; 
					countInnerLoop++;



					bool areImmediates; //takes the flag of same segments
					//looks for secondImm and secondIt with orderedImms




					areImmediates = 1;
					cout << "firstImm " << firstImm << " dist " << distanceFirstImm << endl;



					for (size_t i = 0; i < orderedImms.size(); i++)
					{
						//finds in ordered imms where you are
						if (firstImm == orderedImms[i][0] && distanceFirstImm == orderedImms[i][1])
						{
							//makes your id unusable for the future //es necesario?
							orderedImms[i][3] = 0;
							if (i == 0)
							{
								cout << "ocurre i==0" << endl;
								orderedImms[i][3] = 0; 
								orderedImms[orderedImms.size() - 1][3] = 0;
							}





							if (firstIt == initialIt)
							{


								cout << "*firstIt == initialIt" << endl;

								//third case
								
								size_t savedJ;
								bool foundThirdCase = false;
								float currentDistance = std::numeric_limits<float>::max();
								//looking for lines with the same segment
								for (size_t j = 1; j < orderedImms.size(); j++) //no estoy cogiendo el primer elemento repetido //no tendr�a que hacer falta mirar todo orderedImms, los que compartan segmento van a estar o en el immediato de arriba o del segmento
								{
									if (j != i)
									{
										if (orderedImms[j][2] == orderedImms[i][2] /*&& orderedImms[j][3] == 1*/) //if same segment
										{
											//equivalent of saying that j isRightOf i
											if (polygonPositions[firstImm] > polygonPositions[orderedImms[j][0]])
											{
												if (orderedImms[j][1] > orderedImms[i][1] && orderedImms[j][1] < currentDistance)
												{
													foundThirdCase = true;

													currentDistance = orderedImms[j][1];
													savedJ = j;

												}
											}

										}
									}
								}
								
								if (foundThirdCase)
								{
									cout << " third case" << endl;
									areImmediates = 0;

									secondImm = orderedImms[savedJ][0];
									orderedImms[savedJ][3] = 0;


									distanceSecondImm = orderedImms[savedJ][1];

									secondIt = mapIntersectionPoints.begin() + distanceSecondImm;


									break;
								}

								//if third case fails, enters first case
								float c = 1;
								while (true)
								{
									if (orderedImms[i + c][3] == 1)
									{
										if (orderedImms[i][0] != orderedImms[i + c][0]
											|| orderedImms[i][2] != orderedImms[i + c][2])
										{
											cout << " first case" << endl;
											secondImm = orderedImms[i + c][0];
											orderedImms[i + c][3] = 0;


											distanceSecondImm = orderedImms[i + c][1];

											secondIt = mapIntersectionPoints.begin() + distanceSecondImm;


											break;
										}



									}
									c++;
								}




							}



							else

							{
								/*for (size_t j = 1; j < orderedImms.size(); j++)
								{
									if (j != i)
									{
										if (orderedImms[j][1] == orderedImms[i][1] - 1)
										{
											secondImm = orderedImms[j][0];
											orderedImms[j][3] = 0;
											distanceSecondImm = orderedImms[j][1];

											secondIt = mapIntersectionPoints.begin() + distanceSecondImm;

											if (orderedImms[i][2] == orderedImms[j][2])
												areImmediates = 0;

											break;
										}
									}
								}*/




								//caso 3 invertido, busca si alguno tiene segments igual a i
								//, coge el que tenga menor dist que i pero mayor que distInitial
								std::vector<std::pair<int, std::vector<float>>> possibleSecondImm;
								size_t savedJ;
								bool foundThirdCase = false;
								float currentDistance = std::numeric_limits<float>::min();
								//looking for lines with the same segment
								for (size_t j = 1; j < orderedImms.size(); j++) //no estoy cogiendo el primer elemento repetido //no tendr�a que hacer falta mirar todo orderedImms, los que compartan segmento van a estar o en el immediato de arriba o del segmento
								{
									if (j != i)
									{
										if (orderedImms[j][2] == orderedImms[i][2]) //if same segment
										{
											//equivalent of saying that j isRightOf i
											if (polygonPositions[firstImm] < polygonPositions[orderedImms[j][0]])
											{
												if (orderedImms[j][1] < orderedImms[i][1] && orderedImms[j][1] > currentDistance)
												{
													foundThirdCase = true;

													currentDistance = orderedImms[j][1];
													savedJ = j;

												}
											}

										}
									}
								}
								float possibleSecondIndex;
								if (foundThirdCase)
								{
									cout << "inverse third case" << endl;
									areImmediates = 0;

									secondImm = orderedImms[savedJ][0];
									orderedImms[savedJ][3] = 0;


									distanceSecondImm = orderedImms[savedJ][1];

									secondIt = mapIntersectionPoints.begin() + distanceSecondImm;


									break;

								}




















								/*
								Que hay, soy yo, t�. No te agobies, supongo que tendr�s pocos recuerdos de que pas� ayer porque
								hasta yo los tengo y lo acabo de hacer. La pipeline consiste en, primero es initialIt y puede no
								haber o haber caso tres o caso normal. Ese deber�a de ir bien siempre por depender de orderedImms

								Para cuando no es initialIt, aqu�, puede haber un tercer caso pero en sentido contrario o
								est� el caso normal inverso que se hace sin depender de orderedImms (menos por el i)

								Yo te menciono varias de tus opciones y luego ya haces lo que te salga de la polla, sin juzgar.
								Tienes para hacer o arreglar normal inverse, que depende de una comparaci�n entre segments y
								si el indice 0 no est� a la izquierda de la figura va a crashear, ese es el mayor reto que puedes
								pelear en verdad. Puedes ir a ese la cosa es que si empiezas ah� no vas a limpiar, lo cual
								no es ni malo ni bueno. Tambi�n tienes la opci�n de empezar a quitar orderedImms y creo
								que [3] tiene que estar a punto de ser inutil y poder ser eliminado. Anyways, esto m�s bien
								es peque�o recap, s� que vas a ir a arreglar normal inverse porque te conozco

								Pasatelo bien y alegrate que estamos terminando esto, quien sabe, lo mismo arreglas eso y el
								programa ya es full funcional en todos los casos, no veo porque no iba a ser, complejidad lleg�
								a funcionar
								*/









								int maxeameEsta = std::numeric_limits<int>::max();
								int guardameEsta;


								for (size_t j = distanceInitialImm + 1; j < distancefirstSecondImm; j++)
								{
									cout << "if" << mapIntersectionPoints[j].first << " " << j << endl;

									cout << " " << mapIntersectionPoints[j].first << " "
										<< orderedImms[i][0] << endl
										<< " " << mapIntersectionPoints[j].first << " "
										<< maxeameEsta << endl;


									if (mapIntersectionPoints[j].first >= orderedImms[i][0]
										&& mapIntersectionPoints[j].first <= maxeameEsta)
									{
										cout << " ** " << j << endl;
										maxeameEsta = mapIntersectionPoints[j].second[2];
										guardameEsta = j;
									}
									cout << endl;

								}
								cout << "normal inverso" << endl;

								secondImm = mapIntersectionPoints[guardameEsta].second[2];


								distanceSecondImm = guardameEsta /*+ distanceInitialImm*/;


								secondIt = mapIntersectionPoints.begin() + distanceSecondImm;

								if (orderedImms[i][2] == mapIntersectionPoints[guardameEsta].first)
									areImmediates = 0;

								//cout << "secondImm " << secondImm << " distanceSecondImm " << distanceSecondImm << endl;;


								break;


								//for (size_t j = 1; j < orderedImms.size(); j++) //no estoy cogiendo el primer elemento repetido //no tendr�a que hacer falta mirar todo orderedImms, los que compartan segmento van a estar o en el immediato de arriba o del segmento
								//{
								//	
								//		/*cout << "j " << j << endl;
								//		cout << " "<<(orderedImms[j][0] > orderedImms[i][0]
								//			&& orderedImms[j][1] < orderedImms[i][1]) << endl;*/
								//		if (orderedImms[j][0] > orderedImms[i][0]
								//			&& orderedImms[j][1] < orderedImms[i][1] 
								//			&& orderedImms[j][1] < orderedImms[distanceInitialImm][1])
								//		{
								//			
								//			if(orderedImms[j][0]- orderedImms[i][0]< maxeameEsta)
								//			{
								//				maxeameEsta = orderedImms[j][0] - orderedImms[i][0];
								//				guardameEsta = j;
								//			}
								//		}
								//	
								//}
								//secondImm = orderedImms[guardameEsta][0];
								//orderedImms[guardameEsta][3] = 0;
								//distanceSecondImm = orderedImms[guardameEsta][1];

								//secondIt = mapIntersectionPoints.begin() + distanceSecondImm;

								//if (orderedImms[i][2] == orderedImms[guardameEsta][2])
								//	areImmediates = 0;

								//break;

								/*
								//El retorno del m�tico caso 2
								//Esto se da porque en caso 2 volviendo, este se queda detr�s de i en vez de en +1
								if (orderedImms[i - 1][3] == 1 && orderedImms[i - 1][0] == orderedImms[i][0]
									&& orderedImms[i - 1][1] == orderedImms[i][1] - 1)
								{
									cout << "renacido caso 2" << endl;
									secondImm = orderedImms[i - 1][0];
									orderedImms[i - 1][3] = 0;
									distanceSecondImm = orderedImms[i - 1][1];

									secondIt = mapIntersectionPoints.begin() + distanceSecondImm;

									break;
								}
								//El razonamiento de esto es que buscas si puedes ir a i+1 y si no es valido va para atr�s


								//Esto estoy viendo que se da en casos donde hay caso 2 pero en vez de mirar a su compa�ero de imm
								// mira por encima a donde no es, [3] es un indicador de casos 2 cuando va de vuelta a initialIt
								// (porque si no es indistinguible de casos 1)
								if (orderedImms[i + 1][3] == 1 && orderedImms[i + 1][1] < distancefirstSecondImm)
								{
									if (orderedImms[i + 1][1] < orderedImms[i][1])
									{
										cout << "*first case" << endl;
										secondImm = orderedImms[i + 1][0];
										orderedImms[i + 1][3] = 0;
										distanceSecondImm = orderedImms[i + 1][1];

										secondIt = mapIntersectionPoints.begin() + distanceSecondImm;

										break;
									}
								}


								//second case, se diferencia del primero solo cuando comparten imms pero est� detr�s del que est�s
								//analizando porque el que est�s analizando es un firstImm despu�s de firstSecondImm (intentas ir
								// para atr�s)
								//Lo de immediatamente arriba es mentira, tambi�n puedes necesitar volver para atr�s en otros
								//casos, y es esos casos si no se comparte immediate no hay immediates

								float c = 1;
								while (true) {
									if (orderedImms[i - c][1] < orderedImms[i][1] && orderedImms[i - c][3] == 1)
									{
										break;
									}

									c++;
								}

								//bool statement = false;
								//float c = 1;
								////baja hasta que tenga menos distancia y sea valido
								//while (!statement) {
								//	if (orderedImms[i - c][1] > orderedImms[i][1] || orderedImms[i - c][3] == 0)
								//	{
								//		c++;
								//	}
								//	else
								//	{
								//		statement = false;
								//	}
								//}


								cout << " *behind case" << endl;
								secondImm = orderedImms[i - c][0];
								orderedImms[i - c][3] = 0;
								distanceSecondImm = orderedImms[i - c][1];

								secondIt = mapIntersectionPoints.begin() + distanceSecondImm;

								if (firstImm != secondImm)
									areImmediates = 0;

								break;

								*/
							}








						}



					}
					//cout << "-areImmediates? " << areImmediates << endl;

					if (initialIt == firstIt) {
						distancefirstSecondImm = distanceSecondImm;
					}








					if (areImmediates) {
						getImmediates(firstImm, secondImm);
					}
					positions.insert(positions.end(), { secondIt->second[0], secondIt->second[1] });





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
			/*cout << "positions: " << endl;
			for (int i = 0; i < positions.size(); i += 2) {
				cout << positions[i] << " " << positions[i + 1] << endl;
			}cout << endl;*/
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