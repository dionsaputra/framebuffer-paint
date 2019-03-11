#include "Wireframe.h"

Wireframe::Wireframe(){}

Wireframe::Wireframe(vector<Point> _controlPoint, Color _borderColor) {
    points = _controlPoint;
    borderColor = _borderColor;
    updateEnvelope();
}

Wireframe::Wireframe(vector<Point> _controlPoint, Point _innerPoint){
    points = _controlPoint;
    innerPoint = _innerPoint;
    updateEnvelope();
}

Wireframe::Wireframe(vector<Point> _controlPoint, Color _fillColor, Color _borderColor, int _priority) {
    fillColor = _fillColor;
    borderColor = _borderColor;
    points = _controlPoint;
    priority = _priority;
    updateEnvelope();
}

void Wireframe::translate(int dx, int dy){
    topLeft.translate(dx,dy);
    bottomRight.translate(dx,dy);
    innerPoint.translate(dx,dy);

    for (int i=0; i<points.size(); i++) {
        points[i].translate(dx, dy);
    }
}

void Wireframe::rotate(Point center, int degree){
    innerPoint.rotate(center,degree);
    for (int i=0; i<points.size(); i++) {
        points[i].translate(dx,dy);
    }
    updateEnvelope();
}

void Wireframe::scale(Point center, float skala){
    innerPoint.scale(center,skala);
    for (int i=0; i<points.size(); i++) {
        points[i].scale(dx, dy);
    }
    updateEnvelope();
}

void Wireframe::updateEnvelope(){
    if (points.size() == 0) {
        return;
    }

    int minX = points[0].getX();
    int minY = points[0].getY();
    int maxX = points[0].getX();
    int maxY = points[0].getY();
    
    for (int i=1; i<points.size(); i++) {
        int currentX = points[i].getX();
        int currentY = points[i].getY();

        if (currentX < minX) minX = currentX;
        if (currentX > maxX) maxX = currentX;
        if (currentY < minY) minY = currentY;
        if (currentY > maxY) maxY = currentY;
    }

    topLeft = Point(minX,minY);
    bottomRight = Point(maxX, maxY);
}

vector<Point> Wireframe::getPoints() {
    return points;
}

Point Wireframe::getTopLeft(){
    return topLeft;
}

Point Wireframe::getBottomRight(){
    return bottomRight;
}

Point Wireframe::getInnerPoint() {
    return innerPoint;
}

Color Wireframe::getFillColor(){
    return fillColor;
}

Color Wireframe::getBorderColor(){
    return borderColor;
}

int Wireframe::getPriority() {
    return priority;
}

void Wireframe::setPoints(vector<Point> _points) {
    points = _points;
}

void Wireframe::setBottomRight(Point _bottomRight){
    bottomRight = _bottomRight;
}

void Wireframe::setTopLeft(Point _topLeft){
    topLeft = _topLeft;
}

void Wireframe::setInnerPoint(Point _innerPoint) {
    this->innerPoint = _innerPoint;
}

void Wireframe::setFillColor(Color _color){
    fillColor = _color;
};

void Wireframe::setBorderColor(Color _color){
    borderColor = _color;
};

void Wireframe::setPriority(int _priority) {
    priority = _priority;
}

Wireframe Wireframe::clippingResult(Wireframe window) {
    if (points.size() < 2) {
        return Wireframe();
    }
    
    vector<Point> clippingPoints;
    for (int i=1; i<points.size(); i++) {
        Point previous = points[i-1];
        Point current = points[i];

        if (isInClip(previous, window) && isInClip(current, window)) {
            clippingPoints.push_back(current);
        } else if (isInClip(previous, window) && !isInClip(current, window)) {
            clippingPoints.push_back(intersect(previous, current, window));
        } else if (!isInClip(previous, window) && isInClip(current, window)) {
            clippingPoints.push_back(intersect(current, previous, window));
            clippingPoints.push_back(current);
        }
    }

    if (clippingPoints.size() == 0) {
        return Wireframe();
    } else {
        Wireframe wireframe(clippingPoints, innerPoint);
        wireframe.setBorderColor(borderColor);
        wireframe.setFillColor(fillColor);
        wireframe.setPriority(priority);
        return wireframe;
    }
}

Point Wireframe::intersect(Point inside, Point outside, Wireframe window) {
    int xMin = window.topLeft.getX(), xMax = window.bottomRight.getX();
    int yMin = window.topLeft.getY(), yMax = window.bottomRight.getY();

    int xOut = outside.getX(), yOut = outside.getY();

    int dx = inside.getX() - xOut;
    int dy = inside.getY() - yOut;

    if (xOut <= xMin) {
        if (yOut <= yMin) { // case 0
            int xSearch = (yMin - yOut) * (dx/dy) + xOut;
            int ySearch = (xMin - xOut) * (dy/dx) + yOut;

            if (xSearch < xMin) return Point(xMin, ySearch);
            return Point(xSearch, yMin);            

        } else if (yOut >= yMax) {  // case 6
            int xSearch = (yMax - yOut) * (dx/dy) + xOut;
            int ySearch = (xMin - xOut) * (dy/dx) + yOut;

            if (xSearch < xMin) return Point(xMin, ySearch);
            return Point(xSearch, yMax);            

        } else {    // case 3
            int ySearch = (xMin - xOut) * (dy/dx) + yOut;
            return Point(xMin, ySearch);
        }
    } else if (xOut >= xMax) {
        if (yOut <= yMin) { // case 2
            int xSearch = (yMin - yOut) * (dx/dy) + xOut;
            int ySearch = (xMax - xOut) * (dy/dx) + yOut;

            if (xSearch < xMax) return Point(xMax, ySearch);
            return Point(xSearch, yMin);            
        } else if (yOut >= yMax) {  // case 8
            int xSearch = (yMax - yOut) * (dx/dy) + xOut;
            int ySearch = (xMax - xOut) * (dy/dx) + yOut;

            if (xSearch < xMax) return Point(xMax, ySearch);
            return Point(xSearch, yMax);            

        } else {    // case 5
            int ySearch = (xMax - xOut) * (dy/dx) + yOut;
            return Point(xMax, ySearch);
        }
    } else {
        if (yOut <= yMin) { // case 1
            int xSearch = (yMin - yOut) * (dx/dy) + xOut;
            return Point(xSearch, yMin);
        } else if (yOut >= yMax) {  // case 7
            int xSearch = (yMax - yOut) * (dx/dy) + xOut;
            return Point(xSearch, yMax);
        } else {    // case 7
            // DO NOTHING
        }
    }
    
}

bool Wireframe::isInClip(Point point, Wireframe window) {
    int xMin = window.topLeft.getX(), xMax = window.bottomRight.getX();
    int yMin = window.topLeft.getY(), yMax = window.bottomRight.getY();

    return xMin <= point.getX() && point.getX() <= xMax && yMin <= point.getY() && point.getY() <= yMax;
}