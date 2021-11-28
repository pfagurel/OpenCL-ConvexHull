// QuickHullCPU.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//
#pragma once
#include "QuickHullCPU.h"




float QuickHullCPU:: distance(Point p1, Point p2, Point p)
{
    return abs((p.y - p1.y) * (p2.x - p1.x) - (p2.y - p1.y) * (p.x - p1.x));
}


int QuickHullCPU::side(Point p1, Point p2, Point p)
{
    float side = (p.y - p1.y) * (p2.x - p1.x) - (p2.y - p1.y) * (p.x - p1.x);
    if (side > 0) return 1;
    if (side < 0) return -1;
    return 0;
}

void QuickHullCPU::find_hull(Point* points, unsigned size, Point p1, Point p2, int _side, std::vector<Point>& result)
{
    int maxIndex = -1;
    float maxDistance = 0.0;

    for (unsigned i = 0; i < size; i++)
    {
        if (side(p1, p2, points[i]) != _side) continue;

        float dist = distance(p1, p2, points[i]);
        if (dist <= maxDistance) continue;

        maxIndex = i;
        maxDistance = dist;
    }

    if (maxIndex == -1)
    {
        result.push_back(p1);
        result.push_back(p2);
        return;
    }

    int newSide = side(points[maxIndex], p1, p2);
    find_hull(points, size, points[maxIndex], p1, -newSide, result);
    find_hull(points, size, points[maxIndex], p2, newSide, result);
}


std::vector<Point> QuickHullCPU::quick_hull(Point* points, unsigned size)
{

    std::vector<Point> result;

    if (size <= 2) return result;

    Point left = points[0];
    Point right = points[0];
    for (unsigned i = 1; i < size; i++)
    {
        if (points[i].x < left.x) left = points[i];
        if (points[i].x > right.x) right = points[i];
    }


    find_hull(points, size, left, right, 1, result);
    find_hull(points, size, left, right, -1, result);

    return result;
}

