#pragma once

#include "Common.cpp"

#include "newPolygons.h"
#include "newFourier.hpp"

struct NewWettedSurface
{
	struct Intersections {
		p point;
		unsigned int segment;
		unsigned int imm; //immediate
		Intersections() : point({ 0,0 }), segment(0), imm(0) {} //initializing paramenters to 0 if not specified 

		Intersections(p point_, unsigned int segment_, unsigned int imm_)
			: point(point_), segment(segment_), imm(imm_) {}
	};



	std::vector<p> positions;
	vector<Intersections> intersections;



	NewPolygons& inputPolygon;

	vector<p>& polygonPositions = inputPolygon.positions;
	vector<unsigned int>& polygonIndices = inputPolygon.dlines.indices;


	NewPolygons outputPolygon;



	NewFourier& fourier;
	vector<p>& fourierPositions = fourier.dlines.positions; //for reducing the overhead of dereferencing the member each time

	NewWettedSurface(NewPolygons& inputPolygon_, NewFourier& fourier_)
		: inputPolygon(inputPolygon_), fourier(fourier_)
	{}


	//single intersections points. The bool tells you if a intersection has happened and if so it will be stored in i
	bool calculateIntersectionPoints(const p& A, const p& B, const p& C, const p& D, p& i);


	//Finds all intersections and fills it with intersections, segments and immediates 
	void calculateIntersections();

	void getWavePoints(float firstX, float secondX);

	void calculatePositions();

	void getImmediates(int firstImm, int secondImm);

	unsigned int calculateSecondIt_Initial(bool& areImmediates, unsigned int& distancefirstSecondImm
		, std::unordered_map<int, vector<int>>& mapSharedSegments, unsigned int& distanceInitialImm, unsigned int& firstImm, unsigned int& initialImm);

	unsigned int calculateSecondIt_notInitial(bool& areImmediates, unsigned int& distancefirstSecondImm
		, std::unordered_map<int, vector<int>>& mapSharedSegments, unsigned int& distanceFirstImm
		, unsigned int& distanceInitialImm, unsigned int& firstImm, unsigned int& initialImm);


};

/////////////A las funciones les faltan consts!!