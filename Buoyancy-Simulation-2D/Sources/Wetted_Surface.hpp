#pragma once



#include "Common.cpp"

#include "Polygons.hpp"
#include "Fourier.hpp"

struct WettedSurface
{
	struct Intersections {
		p point;
		unsigned int segment;
		unsigned int imm; //immediate
		Intersections() : point({ 0,0 }), segment(0), imm(0) {} //initializing paramenters to 0 if not specified 

		Intersections(p point_, unsigned int segment_, unsigned int imm_)
			: point(point_), segment(segment_), imm(imm_) {}
	};

	Polygons& inputPolygon;
	vector<Polygons> outputPolygon;
	vector<p>& polygonPositions = inputPolygon.lines.positions;

	std::vector<p> positions;
	vector<Intersections> intersections;

	Fourier& fourier;
	vector<p>& fourierPositions = fourier.lines.positions;

	


	
	vector<unsigned int> batchIndices;




	WettedSurface(Polygons& inputPolygon_, Fourier& fourier_)
		: inputPolygon(inputPolygon_), fourier(fourier_)
	{}

	void draw() {
		//reserves, creo que hay algo que no pasa el soak test
		outputPolygon.clear();

		outputPolygon.resize(batchIndices.size() - 1);

		for (unsigned int i = 0; i < batchIndices.size()-1; i++)
		{
			vector<p>interm;

			interm.insert(interm.end(), positions.begin() + batchIndices[i], positions.begin() + batchIndices[i + 1]);

			//No puedo meter iteradores directamente en addset?
			outputPolygon[i].addSet(interm);
			outputPolygon[i].lines.draw();
			outputPolygon[i].draw();
			//Mira indices en catamarán que creo que está duplicandolos

			

			//printv2(outputPolygon[i].positions); cout << endl;

		}

		
	}

	//single intersections points. The bool tells you if a intersection has happened and if so it will be stored in i
	bool calculateIntersectionPoints(const p& A, const p& B, const p& C, const p& D, p& i);


	//Finds all intersections and fills it with intersections, segments and immediates 
	void calculateIntersections();

	void getWavePoints(float firstX, float secondX);

	void calculatePositions();

	void getImmediates(int firstImm, int secondImm);

	unsigned int calculateDistanceSecondIt(bool isInitial, auto& firstIt, unsigned int& distanceInitialIt, unsigned int& distancefirstIt);
	

};

