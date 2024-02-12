#pragma once

#include "newWetted_Surface.hpp"


bool calculateIntersectionPoints(const p& A, const p& B, const p& C, const p& D, p& i) {
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

void newWettedSurface::calculateAllIntersections() {
	
}

