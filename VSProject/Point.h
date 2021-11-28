#pragma once

class Point
{
public:
	float x;
	float y;

	Point() = default;
	Point(float x, float y);

	bool operator ==(const Point& obj) const;

};

