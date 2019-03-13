#ifndef WIREFRAME_H
#define WIREFRAME_H

#include <bits/stdc++.h>
#include <vector>
#include "Point.h"
#include "Color.h"

class Wireframe {
private:
    Point topLeft;
    Point bottomRight;
    Point innerPoint;
    Color fillColor;
    Color borderColor;
    vector<Point> points;
    int priority;

public:
    Wireframe();
    Wireframe(vector<Point> _controlPoint, Color _borderColor);
    Wireframe(vector<Point> _controlPoint, Point innerPoint);
    Wireframe(vector<Point> _controlPoint, Point _innerPoint, Color _borderColor, Color _colorFill, int _priority);
    Wireframe(int radius, int numPoint, Point centerPoint, Color color);
    // ~Wireframe();

    // Wireframe& operator=(const Wireframe& _wireframe);

    void translate(int dx, int dy);
    void rotate(int degree);
    void scale(float skala);
    void rotate(Point center, int degree);
    void scale(Point center, float skala);

    void updateEnvelope();

    //getter
    vector<Point> getPoints();
    Point getTopLeft();
    Point getBottomRight();
    Point getInnerPoint();
    Color getFillColor();
    Color getBorderColor();
    int getPriority();

    //setter
    void setPoints(vector<Point> points);
    void setTopLeft(Point _topLeft);
    void setBottomRight(Point _bottomRight);
    void setInnerPoint(Point _innerPoint);
    void setFillColor(Color _color);
    void setBorderColor(Color _color);
    void setPriority(int _priority);

    bool isInEnvelope(Point point);

    Wireframe clippingResult(Wireframe window);
    bool isInClip(Point point, Wireframe window);
    Point intersect(Point inside, Point outside, Wireframe window);
};

#endif