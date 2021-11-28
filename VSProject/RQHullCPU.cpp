#include "RQHullCPU.h"


float RQHullCPU::distance(Point p1, Point p2, Point p)
{
    return abs((p.y - p1.y) * (p2.x - p1.x) - (p2.y - p1.y) * (p.x - p1.x));
}


int RQHullCPU::side(Point p1, Point p2, Point p)
{
    float side = (p.y - p1.y) * (p2.x - p1.x) - (p2.y - p1.y) * (p.x - p1.x);
    if (side > 0) return 1;
    if (side < 0) return -1;
    return 0;
}

float RQHullCPU::odet(Point a, Point b, Point c) {
    // Orientaion determinant
    float side = a.x * (b.y - c.y) - b.x * (a.y - c.y) + c.x * (a.y - b.y);
    if (side > 0) return 1;
    if (side < 0) return -1;
    return 0;
}

bool RQHullCPU::verif_point_inclusion(Point a, Point b, Point c, Point d) {
    //Verif if the point d is included in the triangle abc

    float dcb = odet(d, c, b);
    float dba = odet(d, b, a);
    float dac = odet(d, a, c);

    return (dcb < 0 && dba < 0 && dac < 0) || (dcb > 0 && dba > 0 && dac > 0);
}

void RQHullCPU::find_hull(Point* points, unsigned size, Point p1, Point p2, int _side, std::vector<Point>& result)
{
    int maxIndex = -1;
    float maxDistance = 0.0;

    Point q1;
    Point q2;


    int q1_i;
    int q2_i;
    do
    {
        q1_i = rand() % size;
        q2_i = rand() % size;
        q1 = points[q1_i];
        q2 = points[q2_i];

        int count = 0;
        for (int i = 0; i < size; i++)
        {
            count += used_points[i];
        }
        if (count == size - 1)
        {
            for (int i = 0; i < size; i++)
            {
                if (!used_points[i]) result.push_back(points[i]);
                return;
            }
        };

        if (!(q1 == q2))
        {
            if (verif_point_inclusion(p1, q2, p2, q1))used_points[q1_i] = true;
            if (verif_point_inclusion(p1, q1, p2, q2)) used_points[q2_i] = true;
        }
      

       
    } while (q1 == q2 || q1==p1 || q1==p2 || q2==p1 || q2==p2 || verif_point_inclusion(p1, q2, p2, q1) || verif_point_inclusion(p1, q1, p2, q2) || used_points[q1_i] || used_points[q2_i]);
       
    int p1_side = side(q1, q2, p1);

    for (unsigned i = 0; i < size; i++)
    {
        int i_side = side(p1, p2, points[i]);
        if (i_side != _side || used_points[i]) continue;

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


std::vector<Point> RQHullCPU::rq_hull(Point* points, unsigned size)
{

    std::vector<Point> result;
    used_points = new bool[size];
    for (int i = 0; i < size; i++)
    {
        used_points[i] = false;
    }

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