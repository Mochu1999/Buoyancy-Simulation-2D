#pragma once

#include "Lines.hpp"
#include "utilities.h"

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
	std::vector<std::array<float, 4>> sortedPoints ;

	Polygons()  {
		genBuffers();

		
		/*oldPositions = positions_;*/




		
	}







	

	





	//mete esto dentro cuando acabes
	std::vector<float> edges; //mejor un vector de pairs?
	std::vector<std::deque<unsigned int>> chain;


	void sweepTriangulation(int i) {
		/*for (size_t i = 0; i < sortedPoints.size(); ++i)
		{*/
			////
			/*if (i == 14)
				break;*/


			size_t size = sortedPoints.size();

			std::array<float, 4> b = sortedPoints[i];

			std::array<float, 4> a = Points[(static_cast<int>(b[0]) + size - 1) % size]; //for i=0, a=size-1 (.back), size-1 % size = size-1
			std::array<float, 4> c = Points[(static_cast<int>(b[0]) + 1) % size];        //for i=size-1 (.back), size % size = 0


			cout << endl << "**** b[0] = " << b[0] << endl;

			size_t n_chain = 0;


			//START
			if (a[1] > b[1] && c[1] > b[1])
			{
				bool isProper = true;


				int endEdgeTop = 0;

				for (int e = 0; e < edges.size(); e += 2)
				{
					float topCheck = isRightOfLine(Points[chain[e / 2].front()][1], Points[chain[e / 2].front()][2],
						Points[edges[e]][1], Points[edges[e]][2],
						Points[b[0]][1], Points[b[0]][2]);

					float bottomCheck = isRightOfLine(Points[chain[e / 2].back()][1], Points[chain[e / 2].back()][2],
						Points[edges[e + 1]][1], Points[edges[e + 1]][2],
						Points[b[0]][1], Points[b[0]][2]);

					//cout << "WA " << topCheck << " " <<bottomCheck << endl;

					if (topCheck < 0 && bottomCheck < 0)
					{
						endEdgeTop = e + 2;
						//keep looping
					}
					else if (topCheck < 0 && bottomCheck > 0)
					{
						isProper = false;

						endEdgeTop = e;

						break;

					}
					else if (topCheck > 0 && bottomCheck > 0)
					{
						endEdgeTop = e;
						//es proper pero te me paras
						break;
					}
				}




				//Proper start
				if (isProper)
				{
					//chain.emplace_back(1, b[0]); //mal, no va necesariamente en back


					chain.resize(chain.size() + 1);

					for (size_t i = chain.size() - 1; i > endEdgeTop / 2; --i) {//esto se podría acortar si es al final
						chain[i] = std::move(chain[i - 1]);
					}

					chain[endEdgeTop / 2].insert(chain[endEdgeTop / 2].end(), { static_cast<unsigned int>(b[0]) });


					edges.insert(edges.begin() + endEdgeTop, { a[0],c[0] });

				}
				//Improper start, it breaks the chain in b[0]
				else
				{
					for (size_t e = 0; e < chain[endEdgeTop / 2].size(); e++)
					{
						if (Points[chain[endEdgeTop / 2][e]][2] < Points[b[0]][2]) //it has found where to break
						{
							indices.insert(indices.end()
								, { chain[endEdgeTop / 2][e - 1], static_cast<unsigned int>(b[0]),chain[endEdgeTop / 2][e] });




							//inserting b[0] in the middle of the chain
							chain[endEdgeTop / 2].insert(chain[endEdgeTop / 2].begin() + e
								, { static_cast<unsigned int>(b[0]),static_cast<unsigned int>(b[0]) });

							chain.resize(chain.size() + 1);



							//moving next items one element down
							for (size_t i = chain.size() - 1; i > endEdgeTop / 2 + 1; --i) {
								chain[i] = std::move(chain[i - 1]);
							}


							chain[endEdgeTop / 2 + 1].insert(chain[endEdgeTop / 2 + 1].end(),
								std::make_move_iterator(chain[endEdgeTop / 2].begin() + e + 1), std::make_move_iterator(chain[endEdgeTop / 2].end()));


							//erasing the part of chain[endEdgeTop / 2] you have moved
							chain[endEdgeTop / 2].erase(chain[endEdgeTop / 2].begin() + e + 1, chain[endEdgeTop / 2].end());



							break;
						}

					}




					edges.insert(edges.begin() + endEdgeTop + 1, { c[0],a[0] });



				}
			}

			//END
			else if (b[1] > a[1] && b[1] > c[1])
			{

				//theorem, it is a proper end if the index belongs in both elements of a pair of edges. Improper 
				// if they belong to two pairs of edges

				int endEdges[2]; bool counter = 0;
				for (int e = 0; e < edges.size(); e++)
				{
					if (edges[e] == b[0])
					{
						endEdges[counter] = e;

						if (counter == 1) break;

						counter = 1;
					}
				}

				int endEdgeTop = endEdges[0] / 2; //tells what chain to take
				int endEdgeBottom = endEdges[1] / 2;


				//Si hubiera un circulo, no sería proper end, sería otra condición distinta donde se iría edge pero seguiría habiendo una chain

				// proper end

				if (endEdgeTop == endEdgeBottom)
				{
					chain[endEdgeTop].emplace_front(b[0]);

					for (size_t i = 1; i < chain[endEdgeTop].size() - 1; i++)
					{
						indices.insert(indices.end(), { chain[endEdgeTop][0],chain[endEdgeTop][i],chain[endEdgeTop][i + 1] });
					}
					chain.erase(chain.begin() + endEdgeTop); //erase the chain
					edges.erase(edges.begin() + endEdges[0], edges.begin() + endEdges[1] + 1);
				}
				// improper end
				else
				{
					//top part

					chain[endEdgeTop].emplace_back(b[0]);

					for (size_t i = chain[endEdgeTop].size() - 2; i >= 1; i--)
					{
						/*cout << Points[chain.back()][1] << " " << Points[chain.back()][2] << " "
							<< Points[chain[i]][1] << " " << Points[chain[i]][2] << " "
							<< Points[chain[i - 1]][1] << " " << Points[chain[i - 1]][2] << " " << endl;*/


						if (crossProduct(Points[chain[endEdgeTop].back()][1], Points[chain[endEdgeTop].back()][2]
							, Points[chain[endEdgeTop][i]][1], Points[chain[endEdgeTop][i]][2]
							, Points[chain[endEdgeTop][i - 1]][1], Points[chain[endEdgeTop][i - 1]][2]) < 0) //<0 because order is clockwise (result still cc)
						{
							indices.insert(indices.end()
								, { chain[endEdgeTop][i - 1],chain[endEdgeTop][i],chain[endEdgeTop].back() });
							chain[endEdgeTop].erase(chain[endEdgeTop].begin() + i);
						}

					}


					//bottom part

					chain[endEdgeBottom].emplace_front(b[0]);


					for (size_t i = 1; i < chain[endEdgeBottom].size() - 1; i++)
					{
						/*cout << Points[chain.front()][1] << " " << Points[chain.front()][2] << " "
							<< Points[chain[i]][1] << " " << Points[chain[i]][2] << " "
							<< Points[chain[i + 1]][1] << " " << Points[chain[i + 1]][2] << " " << endl;*/


						if (crossProduct(Points[chain[endEdgeBottom].front()][1], Points[chain[endEdgeBottom].front()][2]
							, Points[chain[endEdgeBottom][i]][1], Points[chain[endEdgeBottom][i]][2]
							, Points[chain[endEdgeBottom][i + 1]][1], Points[chain[endEdgeBottom][i + 1]][2]) > 0)
						{


							indices.insert(indices.end()
								, { chain[endEdgeBottom].front(),chain[endEdgeBottom][i],chain[endEdgeBottom][i + 1] });
							chain[endEdgeBottom].erase(chain[endEdgeBottom].begin() + i);

						}

					}


					//merging
					chain[endEdgeBottom].pop_front();

					chain[endEdgeTop].insert(chain[endEdgeTop].end(),
						std::make_move_iterator(chain[endEdgeBottom].begin()),
						std::make_move_iterator(chain[endEdgeBottom].end()));

					chain.erase(chain.begin() + endEdgeBottom); //erase the chain

					edges.erase(edges.begin() + endEdges[0], edges.begin() + endEdges[1] + 1);
				}




			}

			//BEND
			else
			{

				for (int j = 0; j < edges.size(); ++j)
				{

					if (b[0] == edges[j])
					{
						int n_chain = j / 2;
						//top edge
						if (j % 2 == 0)
						{

							chain[n_chain].emplace_front(b[0]);


							if (chain[n_chain].size() > 2)
							{
								for (size_t i = 1; i < chain[n_chain].size() - 1; i++)
								{
									/*cout << Points[chain.front()][1] << " " << Points[chain.front()][2] << " "
										<< Points[chain[i]][1] << " " << Points[chain[i]][2] << " "
										<< Points[chain[i + 1]][1] << " " << Points[chain[i + 1]][2] << " " << endl;*/


									if (crossProduct(Points[chain[n_chain].front()][1], Points[chain[n_chain].front()][2]
										, Points[chain[n_chain][i]][1], Points[chain[n_chain][i]][2]
										, Points[chain[n_chain][i + 1]][1], Points[chain[n_chain][i + 1]][2]) > 0)
									{


										indices.insert(indices.end()
											, { chain[n_chain].front(),chain[n_chain][i],chain[n_chain][i + 1] });
										chain[n_chain].erase(chain[n_chain].begin() + i);

									}

								}
							}

						}
						//bottom edge
						else
						{

							chain[n_chain].emplace_back(b[0]);

							if (chain[n_chain].size() > 2)
							{
								for (size_t i = chain[n_chain].size() - 2; i >= 1; i--)
								{
									/*cout << Points[chain.back()][1] << " " << Points[chain.back()][2] << " "
										<< Points[chain[i]][1] << " " << Points[chain[i]][2] << " "
										<< Points[chain[i - 1]][1] << " " << Points[chain[i - 1]][2] << " " << endl;*/



									if (crossProduct(Points[chain[n_chain].back()][1], Points[chain[n_chain].back()][2]
										, Points[chain[n_chain][i]][1], Points[chain[n_chain][i]][2]
										, Points[chain[n_chain][i - 1]][1], Points[chain[n_chain][i - 1]][2]) < 0) //<0 because order is clockwise (result still cc)
									{
										indices.insert(indices.end()
											, { chain[n_chain][i - 1],chain[n_chain][i],chain[n_chain].back() });
										chain[n_chain].erase(chain[n_chain].begin() + i);
									}

								}
							}
						}
						//cout << " i: " << i << ", sustituyendo edge[j] = " << j << ", por " << max(a[0], c[0]) << endl;
						if (a[1] > c[1])
							edges[j] = a[0];
						else
							edges[j] = c[0];


						break;
					}

				}



			}

			cout << "chain:" << endl;
			for (const auto& row : chain) {
				for (const auto& value : row) {
					std::cout << value << " ";
				}cout << endl;
			}cout << endl;

			cout << "edges:" << endl;
			for (const auto& value : edges) {
				std::cout << value << " ";
			}cout << endl << endl;


			//cout <<  "    abc: " << a[0] << " " << b[0] << " " << c[0] << "    tipo:" << type << endl << endl;
		//}
	}

	void createNewPolygon() {
		edges.clear();
		chain.clear();
		dlines.clear();

		indices.clear();
		Points.clear();
		sortedPoints.clear();

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

		dlines.clear();

		indices.clear();
		Points.clear();
		sortedPoints.clear();
	}


	void addSet(vector<float> items) {

		edges.clear();
		chain.clear();
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
	void hop() {

		if (abs(cursorX - positions[0]) <= 20 && abs(cursorY - positions[1]) <= 20) {
			cursorX = positions[0];
			cursorY = positions[1];
		}
		positions[positions.size() - 2] = cursorX;
		positions.back() = cursorY;

	}
	void draw() {

		

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

		indices.reserve(positions.size()/2);

		indicesAll.reserve(positions.size() / 2);
		for (unsigned int i = 0; i < positions.size() / 2 - 1; i++) 
		{
			indicesAll.emplace_back( i );
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