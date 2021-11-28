#pragma once
#include "Point.h"
#include <vector>


class QuickHullCPU
{
	float distance(Point p1, Point p2, Point p);


	int side(Point p1, Point p2, Point p);

	void find_hull(Point* points, unsigned size, Point p1, Point p2, int _side, std::vector<Point>& result);

public:
	std::vector<Point> quick_hull(Point* points, unsigned size);

};

