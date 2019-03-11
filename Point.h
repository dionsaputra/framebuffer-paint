#ifndef POINT_H
#define POINT_H
#include <bits/stdc++.h>
#include <iostream>
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>
using namespace std;

class Point {
private:
    int x, y;

public:
    Point();
    Point(int _x, int _y);
    Point(const Point& _point);

    ~Point();

    Point& operator=(const Point& _point);

    void translate(int dx, int dy);
    void rotate(Point center, int degree);
    void scale(Point center, float skala);

    void display();

    // getter
    int getX();
    int getY();

    // setter
    void setX(int _x);
    void setY(int _y);

    bool isEqual(Point _point);

    Point getLeft();
    Point getRight();
    Point getTop();
    Point getBottom();

};
#endif