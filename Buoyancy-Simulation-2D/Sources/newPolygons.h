#pragma once


#include "Common.cpp"
#include "newLines.hpp"

// refactor to decordator pattern to include the functionality of CAD at runtime when triggered

struct NewPolygons {
	GLenum usageHint = GL_DYNAMIC_DRAW;

	bool isBufferUpdated = false;

	newDLines dlines;

	vector<p>& positions = dlines.positions;
	vector <unsigned int> indices; //triangle indices, do not mistake with the dlines indices
	vector <unsigned int> indicesAll;
	vector <unsigned int> indicesRemaining;


	unsigned int vertexBuffer;
	unsigned int vertexArray;
	unsigned int indexBuffer;


	//Setting initial buffers size
	size_t currentPositionsBufferSize = 0;
	size_t currentIndicesBufferSize = 0;
	size_t currentPositionsDataSize = 0;
	size_t currentIndicesDataSize = 0;



	NewPolygons() {
		genBuffers();


		/*oldPositions = positions_;*/


	}

	void genBuffers();

	~NewPolygons();

	void draw();




	



	struct SuperPoints {
		p point;
		unsigned int index;

		SuperPoints() : point({ 0,0 }), index(0) {} //initializing paramenters to 0 if not specified 

		SuperPoints(p point_, unsigned int index_)
			: point(point_), index(index_) {}
	};


	//mete esto dentro cuando acabes
	std::vector<unsigned int> edges; //mejor un vector de pairs?
	std::vector<std::deque<unsigned int>> chain;
	std::unordered_set<int> elementsToAvoid;


	std::vector<SuperPoints> Points;
	std::vector<unsigned int> sortedIndices;

	//A ver, que hace addSet exactamente? Porque el modelo ya está en positions, y no hace falta hacer sweep todo el rato
	void addSet(vector<p> items) {

		isBufferUpdated = true;




		dlines.addSet(items);




		for (unsigned int i = 0; i < positions.size() - 1; i++)
		{
			Points.insert(Points.end(), { positions[i],i });
		}

		


		sortedIndices.resize(Points.size());
		std::iota(sortedIndices.begin(), sortedIndices.end(), 0); //initializing it as 0,1,2,3...
		
		//I could have only used [this] but I would be capturing the entire class
		std::sort(sortedIndices.begin(), sortedIndices.end(), [&Points = this->Points](unsigned int i1, unsigned int i2)
			{
			if (Points[i1].point.x == Points[i2].point.x)
				return Points[i1].point.y > Points[i2].point.y;
			return Points[i1].point.x < Points[i2].point.x;
			});





		cout << "new Points:" << endl;
		for (const auto& entry : Points) {
			cout << entry.index << " " << entry.point.x << " " << entry.point.y << endl;
		}std::cout << std::endl;

		cout << "sortedIndices:" << endl;
		for (auto entry : sortedIndices) {
			cout << entry << " ";
		}cout << endl;





		indices.clear(); //quitar de aquí cuando esté terminado sweep
		//sweepTriangulation();

		printflat3(indices);
		




		/*cout << "dlines.indices" << endl;
		for (unsigned int i = 0; i < dlines.indices.size(); i++) {
			cout << dlines.indices[i] << ", ";
		}cout << endl;*/




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

	void clear() {
		edges.clear();
		chain.clear();
		elementsToAvoid.clear();
		sortedIndices.clear();
		dlines.clear();

		indices.clear();
		Points.clear();

	}


	void sweepTriangulation(/*int i*/);

	//the new point is added at the back and we are triangulation from that one to till the start of the chain
	void trChainBack(const unsigned int& currentChain) {

		auto backIndex = chain[currentChain].back();
		auto PenultimateIndex = chain[currentChain][chain[currentChain].size() - 2];

		for (int k = chain[currentChain].size() - 3; k >= 0; k--)
		{
			if (newcrossProduct(Points[backIndex].point,
				Points[PenultimateIndex].point,
				Points[chain[currentChain][k]].point) <= 0)
				//<0 because order is clockwise (result still cc), =0 bc the area can be 0 in collinear and need to erase
			{
				indices.insert(indices.end(), { chain[currentChain][k],PenultimateIndex,backIndex });

				chain[currentChain].erase(chain[currentChain].begin() + k + 1);
				//I don't need elementsToErase if the order is from end to start in the for loop

				PenultimateIndex = chain[currentChain][k];
			}

		}
	}

	//the new point is added at the front and we are triangulation from that one to till the end of the chain
	void trChainFront(const unsigned int& currentChain) {

		if (chain[currentChain].size() > 2)
		{
			vector<int> elementsToErase; //separating them to remove them from in a backwards motion
			elementsToErase.reserve(5);

			auto frontIndex = chain[currentChain].front();
			auto secondIndex = chain[currentChain][1];

			for (size_t k = 2; k < chain[currentChain].size(); k++)
			{
				if (newcrossProduct(Points[frontIndex].point,
					Points[secondIndex].point,
					Points[chain[currentChain][k]].point) > 0)
				{
					indices.insert(indices.end()
						, { frontIndex,secondIndex,chain[currentChain][k] });

					elementsToErase.emplace_back(k-1);

					secondIndex = chain[currentChain][k];
				}

			}

			for (int k = elementsToErase.size() - 1; k >= 0; k--)
			{
				chain[currentChain].erase(chain[currentChain].begin() + elementsToErase[k]);
			}

		}
	}



	void ConvexTriangulation() {

		for (unsigned int i = 1; i < positions.size() - 2; i++) {
			indices.insert(indices.end(),{ 0, i, i + 1 });
		}

	}

	///*vector <float> oldPositions;*/
	//float area; //m^2
	//float densityArea = 500; //kg/m^2
	//float mass; //kg
	//float centroid[2] = { 0,0 };
	//float force[2] = { 0,0 }; //N
	//float velocity[2] = { 0,0 }; //m/s
	//float acceleration[2] = { 0,0 }; //m/s^2
	//float totalPolarInertia; //centroids

	//float angle = 0;
	//float oldAngle = 0;

	//void getDownwardForce() { //inheritance?
	//	mass = area * densityArea;
	//	force[1] = -9.81 * mass;
	//}

	//void transform(float translationX, float translationY) {
	//	for (int i = 0; i < positions.size(); i += 2) {
	//		positions[i] += translationX;
	//		positions[i + 1] += translationY;
	//	}
	//}

	//void rotate(float centerX, float centerY, float angle) {

	//	float cosAngle = cos(angle);
	//	float sinAngle = sin(angle);

	//	for (int i = 0; i < positions.size(); i += 2) {
	//		float x = positions[i] - centerX;
	//		float y = positions[i + 1] - centerY;

	//		positions[i] = x * cosAngle - y * sinAngle + centerX;
	//		positions[i + 1] = x * sinAngle + y * cosAngle + centerY;
	//	}
	//}

	//void areaCalculation() {	//surveyor's formula
	//	area = 0;
	//	for (int i = 0; i < positions.size() - 2; i += 2) {
	//		area -= (positions[i + 2] - positions[i]) * (positions[i + 3] + positions[i + 1]) * 1e-6; //1e-6 to pass to meters
	//	}
	//	area /= 2.0;

	//}

	//void centroidCalculation() {
	//	std::fill(centroid, centroid + 2, 0);


	//	for (int i = 0; i < positions.size() - 2; i += 2) {
	//		float Ai = (positions[i] * positions[i + 3] - positions[i + 2] * positions[i + 1]) * 1e-6;

	//		centroid[0] += (positions[i] + positions[i + 2]) * 1e-3 * Ai;
	//		centroid[1] += (positions[i + 1] + positions[i + 3]) * 1e-3 * Ai;
	//	}

	//	centroid[0] /= (6.0 * area);
	//	centroid[1] /= (6.0 * area);
	//}


	//void polarAreaMomentOfInertia() {

	//	totalPolarInertia = 0;
	//	float inv36 = 1.0f / 36; //sacar a utilities

	//	float centroidSubTriangles[2];
	//	for (int i = 0; i < indices.size(); i += 3) {

	//		centroidSubTriangles[0] = (positions[indices[i] * 2] + positions[indices[i + 1] * 2] + positions[indices[i + 2] * 2]) / 3 * 1e-3;
	//		centroidSubTriangles[1] = (positions[indices[i] * 2 + 1] + positions[indices[i + 1] * 2 + 1] + positions[indices[i + 2] * 2 + 1]) / 3 * 1e-3;



	//		float x01 = positions[indices[i + 1] * 2] - positions[indices[i] * 2];
	//		float y01 = positions[indices[i + 1] * 2 + 1] - positions[indices[i] * 2 + 1];

	//		float x02 = positions[indices[i + 2] * 2] - positions[indices[i] * 2];
	//		float y02 = positions[indices[i + 2] * 2 + 1] - positions[indices[i] * 2 + 1];

	//		//b 01
	//		float b = sqrt(x01 * x01 + y01 * y01); //modulus

	//		//h (cross product 01,02)/01
	//		float h = std::abs(-x01 * y02 + x02 * y01) / b;

	//		//a projection of 02 into 01
	//		float a = (x01 * x02 + y01 * y02) / b;

	//		b *= 1e-3; h *= 1e-3; a *= 1e-3;

	//		float bh = b * h;

	//		float areaSubTriangle = bh * 0.5f;

	//		/*float xInertia = bh * h * h * inv36;
	//		float yInertia = bh * (b * b + a * a - a * b) * inv36;
	//		float polarInertia = xInertia + yInertia;*/




	//		float d2 = ((centroid[0] - centroidSubTriangles[0]) * (centroid[0] - centroidSubTriangles[0])
	//			+ (centroid[1] - centroidSubTriangles[1]) * (centroid[1] - centroidSubTriangles[1]));

	//		float polarInertia = bh * (b * b + a * a - a * b + h * h) * inv36 + areaSubTriangle * d2;

	//		totalPolarInertia += polarInertia;

	//	}
	//}







	//void earClipping() {
	//	indices.clear();
	//
	//	for (unsigned int i = 1; i < dlines.indices.size() / 2 - 1; i++)
	//	{
	//		indices.insert(indices.end(), { 0,i,i + 1 });
	//	}
	//}
	//
	////renombrar a earclipping
	//void createIndices() {	//grouping indices
	//	indices.clear(); indicesAll.clear(); indicesRemaining.clear();
	//
	//	indices.reserve(positions.size() / 2);
	//
	//	indicesAll.reserve(positions.size() / 2);
	//	for (unsigned int i = 0; i < positions.size() / 2 - 1; i++)
	//	{
	//		indicesAll.emplace_back(i);
	//	}
	//	indicesRemaining = indicesAll;
	//
	//
	//	//when there are no more indices remaining, the surface has closed
	//	while (!indicesRemaining.empty())
	//	{
	//
	//		//will try to triangulate indicesRemaining[i] with its nearest indices from indicesRemaining
	//		for (int i = 0; i < indicesRemaining.size(); i++)
	//		{
	//			//ends while loop, closes polygon
	//			if (indicesRemaining.size() == 3)
	//			{
	//
	//				indices.insert(indices.end(), { indicesRemaining[0],indicesRemaining[1],indicesRemaining[2] });
	//				indicesRemaining.clear();
	//				break;
	//			}
	//
	//			else
	//			{
	//				//current index is b, nearests indices a and c
	//				unsigned int b = indicesRemaining[i];
	//				unsigned int a, c;
	//
	//				//contemplating the cases where b is at the start or at the end
	//				if (b == indicesRemaining[0])
	//					a = indicesRemaining.back();
	//				else
	//					a = indicesRemaining[i - 1];
	//
	//				if (b == indicesRemaining.back())
	//					c = indicesRemaining[0];
	//				else
	//					c = indicesRemaining[i + 1];
	//
	//
	//				bool barycentricFlag = 0;
	//
	//				for (int k = 0; k < indicesAll.size(); k++)
	//				{
	//					//are there points inside abc
	//					if (k != a && k != b && k != c) {
	//						if (checkBarycentric(positions[k * 2], positions[k * 2 + 1], positions[a * 2], positions[a * 2 + 1]
	//							, positions[b * 2], positions[b * 2 + 1], positions[c * 2], positions[c * 2 + 1]))
	//						{
	//							barycentricFlag = 1;
	//							break;
	//						}
	//					}
	//				}
	//
	//				if (!barycentricFlag)
	//				{
	//					//must be concave to be valid
	//					if (isConcave(absoluteAngle(positions[b * 2] - positions[a * 2], positions[b * 2 + 1] - positions[a * 2 + 1]),
	//						absoluteAngle(positions[c * 2] - positions[b * 2], positions[c * 2 + 1] - positions[b * 2 + 1])))
	//					{
	//						//errasing b and adding the indices
	//						indicesRemaining.erase(indicesRemaining.begin() + i);
	//						indices.insert(indices.end(), { a,b,c });
	//						break;
	//					}
	//				}
	//
	//
	//			}
	//		}
	//	}
	//}


};