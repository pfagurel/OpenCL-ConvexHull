#include "Point.h"
Point::Point(float x, float y) {

    this->x = x;
    this->y = y;
}

bool Point::operator==(const Point& obj) const
{
    if (x == obj.x && y == obj.y)
        return true;
    else
        return false;
}

