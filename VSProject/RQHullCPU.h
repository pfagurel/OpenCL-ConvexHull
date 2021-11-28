#pragma once
#include "Point.h"
#include <vector>
#include <stdlib.h>
#include <iostream>

class RQHullCPU
{
	float distance(Point p1, Point p2, Point p);


	int side(Point p1, Point p2, Point p);


	float odet(Point a, Point b, Point c);

	bool verif_point_inclusion(Point a, Point b, Point c, Point d);

	void find_hull(Point* points, unsigned size, Point p1, Point p2, int _side, std::vector<Point>& result);

	
	bool* used_points;

public:
	std::vector<Point> rq_hull(Point* points, unsigned size);

	~RQHullCPU()
	{
		delete[] used_points;
	}

};