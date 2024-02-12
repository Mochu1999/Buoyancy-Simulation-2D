#pragma once

#include "Common.cpp"

#include "newLines.hpp"


struct newWettedSurface
{
	std::vector<p> positions;
	std::vector<std::array<float, 4>> intersections;

	newDLines dlines;
	//Polygons inputPolygon;

	//vector<float>& polygonPositions = inputPolygon.positions;
	//vector<unsigned int>& polygonIndices = inputPolygon.dlines.indices;


	//Polygons outputPolygon;



	//FourierMesh& fourier;
	//vector<float>& fourierPositions = fourier.dlines.positions; //for reducing the overhead of dereferencing the member each time

	/*newWettedSurface(	)
	{};*/
	/*newWettedSurface(Polygons& inputPolygon_, FourierMesh& fourier_)
		: inputPolygon(inputPolygon_), fourier(fourier_)
	{}*/

	bool calculateIntersectionPoints(const p& A, const p& B, const p& C, const p& D, p& i);


	//fills intersections with intersections, segments and immediates 
	void calculateAllIntersections();
};







