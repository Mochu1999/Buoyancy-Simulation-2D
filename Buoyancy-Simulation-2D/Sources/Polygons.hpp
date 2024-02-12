#pragma once

#include "Lines.hpp"
#include "utilities.cpp"

struct Polygons {

	DLines dlines;//El único motivo por el que hay dlines es porque necesito sus indices, y lo mismo esa premisa es mentira

	vector<float>& positions = dlines.positions;
	vector <unsigned int> indices; //indices of each triangle, do not mistake with the indices
	vector <unsigned int> indicesAll;
	vector <unsigned int> indicesRemaining;


	unsigned int vertexBuffer;
	unsigned int vertexArray;
	unsigned int indexBuffer;



	size_t currentBufferSize = 100000 * sizeof(float);
	/////////////////////////////////////////////////////////////////////////////////////////////
	// Si en circles no funciona aquí tampoco lo hará
	/////////////////////////////////////////////////////////////////////////////////////////////

	std::vector<std::array<float, 4>> Points;
	std::vector<std::array<float, 4>> sortedPoints;

	Polygons() {
		genBuffers();


		/*oldPositions = positions_;*/





	}












	////////////////////////////////////////
	// El orden después es. Formato de puntos, terminar la camara, terminar el mar, crear un cubo, exportar superficies mojadas
	// a 3d, arreglar bug de físicas y exportar físicas a 3d. Hacia las estrellas
	//


	//mete esto dentro cuando acabes
	std::vector<float> edges; //mejor un vector de pairs?
	std::vector<std::deque<unsigned int>> chain;
	std::vector<unsigned int> rm; //rightmost
	std::unordered_set<int> elementsToAvoid;

	///It can produce triangles of 0 area (collinear points), it may be important in the future
	void sweepTriangulation(/*int i*/) {
		for (size_t i = 0; i < sortedPoints.size(); ++i)
		{
		

			if (elementsToAvoid.find(sortedPoints[i][0]) == elementsToAvoid.end())  //element is not inside elementsToAvoid
			{


				int size = sortedPoints.size();

				//b is the current point. a is the one that is before b and c the one that is after in cc
				std::array<float, 4> b = sortedPoints[i];
				std::deque<float> bContainer;

				std::array<float, 4> a;
				std::array<float, 4> c;





				int bcounter = 1;
				while (true)
				{
					//a = Points[(static_cast<int>(b[0]) - bcounter) % size];    //for i=0, a=size-1 (.back), size-1 % size = size-1
					a = Points[(static_cast<int>(b[0]) - bcounter + size) % size]; //It had to be modified bc C++ doesn't deal with negative modulus

					if (a[1] == b[1])
					{
						elementsToAvoid.emplace(a[0]);
						bContainer.push_front(a[0]);
						bcounter++;
					}
					else
					{
						break;
					}
				}

				bContainer.push_back(b[0]);
				bcounter = 1;
				while (true)
				{
					c = Points[(static_cast<int>(b[0]) + bcounter) % size];        //for i=size-1 (.back), size % size = 0
					if (c[1] == b[1])
					{
						elementsToAvoid.emplace(c[0]);
						bContainer.push_back(c[0]);
						bcounter++;
					}
					else
					{
						break;
					}
				}

				//Se ha testeado con: proper start, end, bend


				int rmContainer; //picks the bottom one if shared x coordinates

				/*cout << endl << "**** b[0] = " << b[0] << endl;

				cout << "a[0]: " << a[0] << " c[0]: " << c[0] << endl;

				cout << "bContainer: " << endl;
				for (int i = 0; i < bContainer.size(); i++) {
					cout << bContainer[i] << " ";
				}cout << endl << endl;;*/


				size_t currentChain = 0;


				//START
				if (a[1] > b[1] && c[1] > b[1])
				{
					bool isProper = true;

					int currentEdge = 0;

					//looking if it is proper or improper (depending on where do you insert the new edges)
					//It passes through all edges and sees if b[0] is above, in between or below its current edges
					for (int e = 0; e < edges.size(); e += 2)
					{

						//front of chain goes with the top edge, the back of the chain goes with the bottom edge
						float topCheck = isRightOfLine(Points[chain[e / 2].front()][1], Points[chain[e / 2].front()][2],
							Points[edges[e]][1], Points[edges[e]][2],
							Points[b[0]][1], Points[b[0]][2]);

						float bottomCheck = isRightOfLine(Points[chain[e / 2].back()][1], Points[chain[e / 2].back()][2],
							Points[edges[e + 1]][1], Points[edges[e + 1]][2],
							Points[b[0]][1], Points[b[0]][2]);


						//above, proper in e
						if (topCheck > 0 && bottomCheck > 0)
						{
							currentEdge = e;

							break;
						}
						//between, improper
						else if (topCheck < 0 && bottomCheck > 0)
						{
							isProper = false;

							currentEdge = e;

							break;

						}
						//below, keeps looping and if there are no more edges, the new ones are created at the bottom
						else if (topCheck < 0 && bottomCheck < 0)
						{
							currentEdge = e + 2; //only used if it's the end of the for loop
						}
					}
					int currentChain = currentEdge / 2;
					int nextChain = currentChain + 1;

					//Proper start
					if (isProper)
					{
						chain.resize(chain.size() + 1);
						rm.resize(rm.size() + 1);

						// moving the elemments greater than currentEdge its position + 1
						for (size_t k = chain.size() - 1; k > currentChain; --k)
						{
							chain[k] = std::move(chain[k - 1]);
							rm[k] = std::move(rm[k - 1]);
						}
						chain[currentChain].insert(chain[currentChain].end(), bContainer.begin(), bContainer.end());
						//			/*chain[currentChain].insert(chain[currentChain].end(), { static_cast<unsigned int>(b[0]) });*/
						rm[currentChain] = b[0];

						edges.insert(edges.begin() + currentEdge, { a[0],c[0] }); //in cc, a will be always above c for proper starts

					}
					//Improper start, it breaks the chain. The first one is currentChain+bContainer 
					//  and the second one is a new one formed with bContainer.front and currentChain.back()

					else
					{
						chain.resize(chain.size() + 1);
						rm.resize(rm.size() + 1);

						for (size_t j = chain.size() - 1; j > nextChain; --j)
						{
							chain[j] = std::move(chain[j - 1]);
							rm[j] = std::move(rm[j - 1]);
						}

						int breakingPoint;
						for (int j = 0; j < chain[currentChain].size(); j++)
						{
							if (chain[currentChain][j] == rm[currentChain])
							{
								breakingPoint = j;
							}
						}
						chain[nextChain].insert(chain[nextChain].end(), chain[currentChain].begin() + breakingPoint, chain[currentChain].end());
						chain[currentChain].erase(chain[currentChain].begin() + breakingPoint + 1, chain[currentChain].end());

						chain[currentChain].insert(chain[currentChain].end(), bContainer.begin(), bContainer.end());

						chain[nextChain].emplace_front(bContainer.front());


						rm[currentChain] = b[0];
						rm[nextChain] = b[0];



						edges.insert(edges.begin() + currentEdge + 1, { c[0],a[0] }); //in cc, c will be always above a for improper starts

						vector<int> elementsToErase; //separating them to remove them from in a backwards motion
						elementsToErase.reserve(5);

						//triangulation, one per each new chain
						// It could be avoided if we were certain next element was a bend, but not knowing it could be an end

						//top chain
						if (chain[currentChain].size() > 2)
						{
							//from the nearests of b[0] till the start of chain
							for (size_t k = chain[currentChain].size() - 2; k >= 1; k--)
							{
								if (crossProduct(Points[chain[currentChain].back()][1], Points[chain[currentChain].back()][2]
									, Points[chain[currentChain][k]][1], Points[chain[currentChain][k]][2]
									, Points[chain[currentChain][k - 1]][1], Points[chain[currentChain][k - 1]][2]) <= 0)
									//<0 because order is clockwise (result still cc), =0 bc the area can be 0 in collinear and need to erase
								{
									indices.insert(indices.end()
										, { chain[currentChain][k - 1],chain[currentChain][k],chain[currentChain].back() });

									chain[currentChain].erase(chain[currentChain].begin() + k);
									//I don't need elementsToErase if the order is from end to start in the for loop
								}

							}

						}
						//bottom chain
						if (chain[nextChain].size() > 2)
						{
							//from the nearests of b[0] till the end of chain
							for (size_t k = 1; k < chain[nextChain].size() - 1; k++)
							{
								if (crossProduct(Points[chain[nextChain].front()][1], Points[chain[nextChain].front()][2]
									, Points[chain[nextChain][k]][1], Points[chain[nextChain][k]][2]
									, Points[chain[nextChain][k + 1]][1], Points[chain[nextChain][k + 1]][2]) > 0)
								{

									indices.insert(indices.end()
										, { chain[nextChain].front(),chain[nextChain][k],chain[nextChain][k + 1] });

									elementsToErase.emplace_back(k);
								}

							}

							for (int k = elementsToErase.size() - 1; k >= 0; k--)
							{
								chain[nextChain].erase(chain[nextChain].begin() + elementsToErase[k]);
							}
						}
					}
				}

				//al final del circulo creo que no sería proper end
				// , sería otra condición distinta donde se iría edge pero seguiría habiendo una chain

				//END
				else if (b[1] > a[1] && b[1] > c[1])
				{
					//theorem, it is a proper end if the index belongs in both elements of a pair of edges. Improper 
					// if they belong to two pairs of edges

					int currentEdge;
					for (int e = 0; e < edges.size(); e++)
					{
						if (edges[e] == b[0])
						{
							currentEdge = e;
							break;
						}
					}
					currentChain = currentEdge / 2;
					int possibleNextChain = (currentEdge + 1) / 2; //it can be the same as current chain or not


					// Proper end, closes a chain
					if (currentChain == possibleNextChain)
					{
						//if there is collinear first we need to "bend" the front of bContainer. And only then we apply convex triangulation
						if (bContainer.size() > 1)
						{
							chain[currentChain].emplace_back(bContainer.front());

							if (chain[currentChain].size() > 2)
							{
								for (size_t k = chain[currentChain].size() - 2; k >= 1; k--)
								{
									if (crossProduct(Points[chain[currentChain].back()][1], Points[chain[currentChain].back()][2]
										, Points[chain[currentChain][k]][1], Points[chain[currentChain][k]][2]
										, Points[chain[currentChain][k - 1]][1], Points[chain[currentChain][k - 1]][2]) < 0)
									{ //<0 because order is clockwise (result still cc)
										indices.insert(indices.end()
											, { chain[currentChain][k - 1],chain[currentChain][k],chain[currentChain].back() });

										chain[currentChain].erase(chain[currentChain].begin() + k);
									}

								}
							}


						}
						//yes, I am deliverately ignoring possible points between bContainer.front and back; as I've must done for everything
						chain[currentChain].emplace_back(bContainer.back());
						//chain[currentChain].insert(chain[currentChain].end(), bContainer.begin(), bContainer.end());


						//triangulating b[0] with all the other elements of the cain as if in a convex polygon
						for (size_t k = 0; k < chain[currentChain].size() - 2; k++)
						{
							indices.insert(indices.end(), { static_cast<unsigned int>(b[0]),chain[currentChain][k],chain[currentChain][k + 1] });
						}
						chain.erase(chain.begin() + currentChain); //erase the entire chain

						edges.erase(edges.begin() + currentEdge, edges.begin() + currentEdge + 2); //erase the edges


						rm.erase(rm.begin() + currentChain);
					}

					// Improper end. Acts as a bend. Merges two chains into one and triangulates them. possibleNextChain here is equal to nextChain
					else
					{ //la formula para top edge aquí es la equivalente a bottom edge en bend, por que no coinciden?

						//top chain

						rm[currentChain] = b[0];
						rm.erase(rm.begin() + possibleNextChain);


						chain[currentChain].emplace_back(b[0]);
						if (chain[currentChain].size() > 2)
						{
							for (size_t k = chain[currentChain].size() - 2; k >= 1; k--)
							{
								if (crossProduct(Points[chain[currentChain].back()][1], Points[chain[currentChain].back()][2]
									, Points[chain[currentChain][k]][1], Points[chain[currentChain][k]][2]
									, Points[chain[currentChain][k - 1]][1], Points[chain[currentChain][k - 1]][2]) < 0)
									//<0 because order is clockwise (result still cc)
								{
									indices.insert(indices.end()
										, { chain[currentChain][k - 1],chain[currentChain][k],chain[currentChain].back() });

									chain[currentChain].erase(chain[currentChain].begin() + k);
								}

							}
						}

						//bottom chain

						chain[possibleNextChain].insert(chain[possibleNextChain].begin(), bContainer.begin(), bContainer.end());

						if (chain[possibleNextChain].size() > 2)
						{
							vector<int> elementsToErase; //separating them to remove them from in a backwards motion
							elementsToErase.reserve(5);

							for (size_t k = bContainer.size(); k < chain[possibleNextChain].size() - 1; k++)
							{
								if (crossProduct(Points[bContainer.back()][1], Points[bContainer.back()][2]
									, Points[chain[possibleNextChain][k]][1], Points[chain[possibleNextChain][k]][2]
									, Points[chain[possibleNextChain][k + 1]][1], Points[chain[possibleNextChain][k + 1]][2]) > 0)
								{


									indices.insert(indices.end()
										, { static_cast<unsigned int>(bContainer.back())
										,chain[possibleNextChain][k],chain[possibleNextChain][k + 1] });

									elementsToErase.emplace_back(k);

								}

							}

							for (int k = elementsToErase.size() - 1; k >= 0; k--)
							{
								chain[possibleNextChain].erase(chain[possibleNextChain].begin() + elementsToErase[k]);
							}
						}


						//merging
						chain[possibleNextChain].pop_front();

						chain[currentChain].insert(chain[currentChain].end(),
							std::make_move_iterator(chain[possibleNextChain].begin()),
							std::make_move_iterator(chain[possibleNextChain].end()));

						chain.erase(chain.begin() + possibleNextChain); //erase the chain

						edges.erase(edges.begin() + currentEdge, edges.begin() + currentEdge + 2);
					}




				}


				//BEND
				else
				{
					int currentEdge;

					bool foundE = false;

					//normal case
					for (int e = 0; e < edges.size(); ++e)
					{
						if (bContainer.back() == edges[e] || bContainer.front() == edges[e])
						{
							currentEdge = e;
							break;
						}
					}


					int currentChain = currentEdge / 2;


					// changing edges to the immediate of bigger x
					if (a[1] > c[1])
						edges[currentEdge] = a[0];
					else
						edges[currentEdge] = c[0];





					//bend at a top edge
					if (currentEdge % 2 == 0)
					{
						//case where the bend goes downwards. First triangulates .back if applies, then .front
						if (bContainer.size() > 1 && Points[bContainer.front()][2] < Points[bContainer.back()][2])
						{

							chain[currentChain].emplace_front(bContainer.back());

							if (chain[currentChain].size() > 2)
							{
								vector<int> elementsToErase; //separating them to remove them from in a backwards motion
								elementsToErase.reserve(5);

								//from the lowest point of bContainer to the end of chain
								for (size_t k = 1; k < chain[currentChain].size() - 1; k++)
								{
									if (crossProduct(Points[chain[currentChain].front()][1], Points[chain[currentChain].front()][2]
										, Points[chain[currentChain][k]][1], Points[chain[currentChain][k]][2]
										, Points[chain[currentChain][k + 1]][1], Points[chain[currentChain][k + 1]][2]) > 0)
									{
										indices.insert(indices.end()
											, { chain[currentChain].front(),chain[currentChain][k],chain[currentChain][k + 1] });

										elementsToErase.emplace_back(k);
									}

								}
								for (int k = elementsToErase.size() - 1; k >= 0; k--)
								{
									chain[currentChain].erase(chain[currentChain].begin() + elementsToErase[k]);
								}
							}

							chain[currentChain].emplace_front(bContainer.front());

							if (chain[currentChain].size() > 2)
							{
								vector<int> elementsToErase; //separating them to remove them from in a backwards motion
								elementsToErase.reserve(5);

								//from the lowest point of bContainer to the end of chain
								for (size_t k = 1; k < chain[currentChain].size() - 1; k++)
								{
									if (crossProduct(Points[chain[currentChain].front()][1], Points[chain[currentChain].front()][2]
										, Points[chain[currentChain][k]][1], Points[chain[currentChain][k]][2]
										, Points[chain[currentChain][k + 1]][1], Points[chain[currentChain][k + 1]][2]) > 0)
									{
										indices.insert(indices.end()
											, { chain[currentChain].front(),chain[currentChain][k],chain[currentChain][k + 1] });

										elementsToErase.emplace_back(k);
									}

								}
								for (int k = elementsToErase.size() - 1; k >= 0; k--)
								{
									chain[currentChain].erase(chain[currentChain].begin() + elementsToErase[k]);
								}
							}

							rm[currentChain] = bContainer.front();

							/*
















							chain[currentChain].insert(chain[currentChain].begin(), bContainer.begin(), bContainer.end());

							if (chain[currentChain].size() > 2)
							{
								vector<int> elementsToErase; //separating them to remove them from in a backwards motion
								elementsToErase.reserve(5);

								//from the lowest point of bContainer to the end of chain
								for (size_t k = 1; k < chain[currentChain].size() - 1; k++)
								{
									if (crossProduct(Points[chain[currentChain].front()][1], Points[chain[currentChain].front()][2]
										, Points[chain[currentChain][k]][1], Points[chain[currentChain][k]][2]
										, Points[chain[currentChain][k + 1]][1], Points[chain[currentChain][k + 1]][2]) > 0)
									{
										indices.insert(indices.end()
											, { chain[currentChain].front(),chain[currentChain][k],chain[currentChain][k + 1] });

										elementsToErase.emplace_back(k);
									}

								}
								for (int k = elementsToErase.size() - 1; k >= 0; k--)
								{
									chain[currentChain].erase(chain[currentChain].begin() + elementsToErase[k]);
								}
							}
							rm[currentChain] = bContainer.front();
							*/
						}
						//case where there's no bend or it goes upwards
						else
						{
							chain[currentChain].emplace_front(bContainer.back());


							if (chain[currentChain].size() > 2)
							{
								vector<int> elementsToErase; //separating them to remove them from in a backwards motion
								elementsToErase.reserve(5);

								//from the b[0] to the end of chain
								for (size_t k = 1; k < chain[currentChain].size() - 1; k++)
								{

									if (crossProduct(Points[chain[currentChain].front()][1], Points[chain[currentChain].front()][2]
										, Points[chain[currentChain][k]][1], Points[chain[currentChain][k]][2]
										, Points[chain[currentChain][k + 1]][1], Points[chain[currentChain][k + 1]][2]) > 0)
									{
										indices.insert(indices.end()
											, { chain[currentChain].front(),chain[currentChain][k],chain[currentChain][k + 1] });

										elementsToErase.emplace_back(k);
									}

								}
								for (int k = elementsToErase.size() - 1; k >= 0; k--)
								{
									chain[currentChain].erase(chain[currentChain].begin() + elementsToErase[k]);
								}
							}

							//adding the rest of the container if applies
							if (bContainer.size() > 1)
							{
								chain[currentChain].insert(chain[currentChain].begin(), bContainer.begin(), bContainer.end() - 1);
							}
							rm[currentChain] = bContainer.back();
						}





					}
					//bend at a bottom edge
					else //Aquí claramente todas las triangulaciones se pueden despejar en una funcion
					{
						//case where the bend goes upwards. First triangulates .front, then .back bContainer is >1 always here
						if (bContainer.size() > 1 && Points[bContainer.back()][2] > Points[bContainer.front()][2])
						{

							chain[currentChain].emplace_back(bContainer.front());

							for (size_t k = chain[currentChain].size() - 2; k >= 1; k--)
							{
								if (crossProduct(Points[chain[currentChain].back()][1], Points[chain[currentChain].back()][2]
									, Points[chain[currentChain][k]][1], Points[chain[currentChain][k]][2]
									, Points[chain[currentChain][k - 1]][1], Points[chain[currentChain][k - 1]][2]) < 0)
								{ //<0 because order is clockwise (result still cc)
									indices.insert(indices.end()
										, { chain[currentChain][k - 1],chain[currentChain][k],chain[currentChain].back() });

									chain[currentChain].erase(chain[currentChain].begin() + k);
								}
							}


							chain[currentChain].emplace_back(bContainer.back());

							//from the top point of bContainer to the end of chain
							for (size_t k = chain[currentChain].size() - 2; k >= 1; k--)
							{
								if (crossProduct(Points[chain[currentChain].back()][1], Points[chain[currentChain].back()][2]
									, Points[chain[currentChain][k]][1], Points[chain[currentChain][k]][2]
									, Points[chain[currentChain][k - 1]][1], Points[chain[currentChain][k - 1]][2]) < 0)
								{ //<0 because order is clockwise (result still cc)
									indices.insert(indices.end()
										, { chain[currentChain][k - 1],chain[currentChain][k],chain[currentChain].back() });

									chain[currentChain].erase(chain[currentChain].begin() + k);
								}

							}

						}
						//case where there's no bend or it goes downwards. Only b[0] is to triangulate
						else
						{
							chain[currentChain].emplace_back(bContainer.front());


							//from the nearests of b[0] till the start of chain
							for (size_t k = chain[currentChain].size() - 2; k >= 1; k--)
							{
								if (crossProduct(Points[chain[currentChain].back()][1], Points[chain[currentChain].back()][2]
									, Points[chain[currentChain][k]][1], Points[chain[currentChain][k]][2]
									, Points[chain[currentChain][k - 1]][1], Points[chain[currentChain][k - 1]][2]) < 0)
								{ //<0 because order is clockwise (result still cc)
									indices.insert(indices.end()
										, { chain[currentChain][k - 1],chain[currentChain][k],chain[currentChain].back() });

									chain[currentChain].erase(chain[currentChain].begin() + k);
								}

							}


							if (bContainer.size() > 1)
							{
								chain[currentChain].insert(chain[currentChain].end(), bContainer.begin() + 1, bContainer.end());
							}
						}
						rm[currentChain] = bContainer.back();
					}



				}

				//cout << "chain:" << endl;
				//for (const auto& row : chain) {
				//	for (const auto& value : row) {
				//		std::cout << value << " ";
				//	}cout << endl;
				//}cout << endl;

				//cout << "rm:" << endl;
				//for (const auto& value : rm) {
				//	std::cout << value << " ";
				//}cout << endl << endl;

				//cout << "edges:" << endl;
				//for (const auto& value : edges) {
				//	std::cout << value << " ";
				//}cout << endl << endl;

				/*cout << "elementsToAvoid:" << endl;
				for (int elem : elementsToAvoid) {
					std::cout << elem << " ";
				}
				std::cout << std::endl << endl;*/

				/*cout << "indices:" << endl;
				for (int i = 0; i < indices.size(); i += 3) {
					cout << indices[i] << " ";
					cout << indices[i + 1] << " ";
					cout << indices[i + 2] << endl;
				}
				cout << endl;*/

			}
		}
	}










	/*vector <float> oldPositions;*/
	float area; //m^2
	float densityArea = 500; //kg/m^2
	float mass; //kg
	float centroid[2] = { 0,0 };
	float force[2] = { 0,0 }; //N
	float velocity[2] = { 0,0 }; //m/s
	float acceleration[2] = { 0,0 }; //m/s^2
	float totalPolarInertia; //centroids

	float angle = 0;
	float oldAngle = 0;

	void getDownwardForce() { //inheritance?
		mass = area * densityArea;
		force[1] = -9.81 * mass;
	}

	void transform(float translationX, float translationY) {
		for (int i = 0; i < positions.size(); i += 2) {
			positions[i] += translationX;
			positions[i + 1] += translationY;
		}
	}

	void rotate(float centerX, float centerY, float angle) {

		float cosAngle = cos(angle);
		float sinAngle = sin(angle);

		for (int i = 0; i < positions.size(); i += 2) {
			float x = positions[i] - centerX;
			float y = positions[i + 1] - centerY;

			positions[i] = x * cosAngle - y * sinAngle + centerX;
			positions[i + 1] = x * sinAngle + y * cosAngle + centerY;
		}
	}

	void areaCalculation() {	//surveyor's formula
		area = 0;
		for (int i = 0; i < positions.size() - 2; i += 2) {
			area -= (positions[i + 2] - positions[i]) * (positions[i + 3] + positions[i + 1]) * 1e-6; //1e-6 to pass to meters
		}
		area /= 2.0;

	}

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


	void polarAreaMomentOfInertia() {

		totalPolarInertia = 0;
		float inv36 = 1.0f / 36; //sacar a utilities

		float centroidSubTriangles[2];
		for (int i = 0; i < indices.size(); i += 3) {

			centroidSubTriangles[0] = (positions[indices[i] * 2] + positions[indices[i + 1] * 2] + positions[indices[i + 2] * 2]) / 3 * 1e-3;
			centroidSubTriangles[1] = (positions[indices[i] * 2 + 1] + positions[indices[i + 1] * 2 + 1] + positions[indices[i + 2] * 2 + 1]) / 3 * 1e-3;



			float x01 = positions[indices[i + 1] * 2] - positions[indices[i] * 2];
			float y01 = positions[indices[i + 1] * 2 + 1] - positions[indices[i] * 2 + 1];

			float x02 = positions[indices[i + 2] * 2] - positions[indices[i] * 2];
			float y02 = positions[indices[i + 2] * 2 + 1] - positions[indices[i] * 2 + 1];

			//b 01
			float b = sqrt(x01 * x01 + y01 * y01); //modulus

			//h (cross product 01,02)/01
			float h = std::abs(-x01 * y02 + x02 * y01) / b;

			//a projection of 02 into 01
			float a = (x01 * x02 + y01 * y02) / b;

			b *= 1e-3; h *= 1e-3; a *= 1e-3;

			float bh = b * h;

			float areaSubTriangle = bh * 0.5f;

			/*float xInertia = bh * h * h * inv36;
			float yInertia = bh * (b * b + a * a - a * b) * inv36;
			float polarInertia = xInertia + yInertia;*/




			float d2 = ((centroid[0] - centroidSubTriangles[0]) * (centroid[0] - centroidSubTriangles[0])
				+ (centroid[1] - centroidSubTriangles[1]) * (centroid[1] - centroidSubTriangles[1]));

			float polarInertia = bh * (b * b + a * a - a * b + h * h) * inv36 + areaSubTriangle * d2;

			totalPolarInertia += polarInertia;

		}
	}

	void clear() {
		edges.clear();
		chain.clear();
		rm.clear();
		elementsToAvoid.clear();
		dlines.clear();

		indices.clear();
		Points.clear();
		sortedPoints.clear();

	}


	void addSet(vector<float> items) {

		edges.clear();
		chain.clear();
		rm.clear();
		elementsToAvoid.clear();
		//dlines.clear();

		indices.clear();
		Points.clear();
		sortedPoints.clear();

		dlines.addSet(items);

		

		//esta lógica no permite varios addSet, pero bueno,cuando puedas hacer sweeptriangulation de una cambia esto a ahí
		//Y cambiale el nombre a points
		float counter = 0;

		//Esta lógica te está quitando el punto repetido, pero tendrás que cambiar el metodo cuando empieces a trabajar con nested poligonos
		//	Lo que tendrías que hacer es quitar lo de tener el punto front en el back e implementar logíca de que al final meta indice 0 siempre
		for (int i = 0; i < positions.size() - 2; i += 2)
		{
			Points.insert(Points.end(), { counter,positions[i],positions[i + 1],0 });
			counter++;
		}


		sortedPoints = Points;


		std::sort(sortedPoints.begin(), sortedPoints.end()
			, [](const std::array<float, 4 >& a, const std::array<float, 4 >& b)
			{
				if (a[1] == b[1])
					return a[2] > b[2];
				return a[1] < b[1];
			});

		for (int i = 0; i < sortedPoints.size(); ++i) {
			Points[sortedPoints[i][0]][3] = i;
		}



		sweepTriangulation();


		/*cout << "Points:" << endl;
		for (const auto& entry : Points) {
			for (unsigned int value : entry) {
				std::cout << value << " ";
			}
			std::cout << std::endl;
		}std::cout << std::endl;

		cout << "sortedPoints:" << endl;
		for (const auto& entry : sortedPoints) {
			for (unsigned int value : entry) {
				std::cout << value << " ";
			}
			std::cout << std::endl;
		}std::cout << std::endl;*/




		/*cout << "dlines.indices" << endl;
		for (unsigned int i = 0; i < dlines.indices.size(); i++) {
			cout << dlines.indices[i] << ", ";
		}cout << endl;*/




		/////////////////////////////////////////////
		//Esto no va a funcionar para multiples polígonos, los indices serían iguales
		//////// area calculation y el resto tendrían que depender de items

		//createIndices();

		//areaCalculation();


		/*if (area < 0) {
			for (int i = 0; i < positions.size() / 2; i += 2) {
				std::swap(positions[i], positions[positions.size() - 2 - i]);
				std::swap(positions[i + 1], positions[positions.size() - 1 - i]);
			}
			areaCalculation();
		}*/

		//centroidCalculation();
		//polarAreaMomentOfInertia();
	}

	//locks cursor in place
	/*void hop() {

		if (abs(cursorX - positions[0]) <= 20 && abs(cursorY - positions[1]) <= 20) {
			cursorX = positions[0];
			cursorY = positions[1];
		}
		positions[positions.size() - 2] = cursorX;
		positions.back() = cursorY;

	}*/
	void draw() {

		//indices.clear();
		
		
		dlines.draw();

		//newcreateIndices();

		glBindVertexArray(vertexArray);

		if (positions.size() * sizeof(float) > currentBufferSize) { //aren't triangle indices bigger? Do they have anything to do with this?

			currentBufferSize += positions.size() * sizeof(float);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, currentBufferSize, positions.data(), GL_DYNAMIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, currentBufferSize, nullptr, GL_DYNAMIC_DRAW);
		}





		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * sizeof(float), positions.data());

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(unsigned int), indices.data());


		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
	}

	void newcreateIndices() {
		indices.clear();

		for (unsigned int i = 1; i < dlines.indices.size() / 2 - 1; i++)
		{
			indices.insert(indices.end(), { 0,i,i + 1 });
		}
	}

	//renombrar a earclipping
	void createIndices() {	//grouping indices
		indices.clear(); indicesAll.clear(); indicesRemaining.clear();

		indices.reserve(positions.size() / 2);

		indicesAll.reserve(positions.size() / 2);
		for (unsigned int i = 0; i < positions.size() / 2 - 1; i++)
		{
			indicesAll.emplace_back(i);
		}
		indicesRemaining = indicesAll;


		//when there are no more indices remaining, the surface has closed
		while (!indicesRemaining.empty())
		{

			//will try to triangulate indicesRemaining[i] with its nearest indices from indicesRemaining
			for (int i = 0; i < indicesRemaining.size(); i++)
			{
				//ends while loop, closes polygon
				if (indicesRemaining.size() == 3)
				{

					indices.insert(indices.end(), { indicesRemaining[0],indicesRemaining[1],indicesRemaining[2] });
					indicesRemaining.clear();
					break;
				}

				else
				{
					//current index is b, nearests indices a and c
					unsigned int b = indicesRemaining[i];
					unsigned int a, c;

					//contemplating the cases where b is at the start or at the end
					if (b == indicesRemaining[0])
						a = indicesRemaining.back();
					else
						a = indicesRemaining[i - 1];

					if (b == indicesRemaining.back())
						c = indicesRemaining[0];
					else
						c = indicesRemaining[i + 1];


					bool barycentricFlag = 0;

					for (int k = 0; k < indicesAll.size(); k++)
					{
						//are there points inside abc
						if (k != a && k != b && k != c) {
							if (checkBarycentric(positions[k * 2], positions[k * 2 + 1], positions[a * 2], positions[a * 2 + 1]
								, positions[b * 2], positions[b * 2 + 1], positions[c * 2], positions[c * 2 + 1]))
							{
								barycentricFlag = 1;
								break;
							}
						}
					}

					if (!barycentricFlag)
					{
						//must be concave to be valid
						if (isConcave(absoluteAngle(positions[b * 2] - positions[a * 2], positions[b * 2 + 1] - positions[a * 2 + 1]),
							absoluteAngle(positions[c * 2] - positions[b * 2], positions[c * 2 + 1] - positions[b * 2 + 1])))
						{
							//errasing b and adding the indices
							indicesRemaining.erase(indicesRemaining.begin() + i);
							indices.insert(indices.end(), { a,b,c });
							break;
						}
					}


				}
			}
		}
	}

	void genBuffers() {
		glGenVertexArrays(1, &vertexArray);
		glBindVertexArray(vertexArray);

		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, currentBufferSize, /*positions.data()*/ nullptr, GL_DYNAMIC_DRAW);

		glGenBuffers(1, &indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, currentBufferSize, nullptr, GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindVertexArray(0);
	}

	~Polygons() {
		glDeleteVertexArrays(1, &vertexArray);
		glDeleteBuffers(1, &vertexBuffer);
		glDeleteBuffers(1, &indexBuffer);
	}
};