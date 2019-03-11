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
    
}

void Wireframe::rotate(Point center, int degree){
    for (int i=0; i < lines.size();i++){
        lines[i].rotate(center, degree);
    }
    innerPoint.rotate(center,degree);
    updateEnvelope();
}

void Wireframe::scale(Point center, float skala){
    for (int i=0; i < lines.size();i++){
        lines[i].scale(center,skala);
    }
    innerPoint.scale(center,skala);
    updateEnvelope();
}

void Wireframe::updateEnvelope(){
    int mLeft=lines[0].start().getX(),mRight=lines[0].start().getX();
    int mTop=lines[0].start().getY(),mBottom=lines[0].start().getY();
    
    for (int i=1; i < lines.size(); i++){
        // get top left and bottom right
        if (lines[i].start().getX() < mLeft){
            mLeft = lines[i].start().getX();
        } else if (lines[i].start().getX() > mRight){
            mRight = lines[i].start().getX();
        }

        if (lines[i].start().getY() < mTop){
            mTop = lines[i].start().getY();
        } else if (lines[i].start().getY() > mBottom){
            mBottom = lines[i].start().getY();
        }
    }

    topLeft = Point(mLeft,mTop);
    bottomRight = Point(mRight,mBottom);
}

vector<Line> Wireframe::getLines(){
    return lines;
}

Point Wireframe::getTopLeft(){
    return topLeft;
}

Point Wireframe::getBottomRight(){
    return bottomRight;
}

Point Wireframe::getInnerPoint() {
    return this->innerPoint;
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

void Wireframe::setLines(vector<Line> _lines){
    lines = _lines;
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

/*** Return true if point is in perimeter of the wireframe ***/
bool Wireframe::is_in_perimeter(Point point) {
    for (int i=0; i<lines.size(); i++) {
        if (lines[i].isMember(point)) {
            return true;
        }
    }
    return false;
}

/*** Return true if point is inside or is in perimeter of wireframe ***/
bool Wireframe::is_inside(Point point) {

    if (is_in_perimeter(point)) return true;

    int count = 0;
    while (point.getX() <= bottomRight.getX()) {
        if (is_in_perimeter(point)) {
            count++;
        }
    }
    return count%2 == 1;
}

Point Wireframe::getMostTopLeftPoint() {
    Point point = lines[0].getMostTopLeftMember();
    for (int i=1; i<lines.size(); i++) {
        Point cmpPoint = lines[i].getMostTopLeftMember();
        if (cmpPoint.getY() > point.getY()) {
            point = cmpPoint;
        }
    }
    return point;
}

bool Wireframe::isInEnvelope(Point point) {
    int minX = topLeft.getX(), maxX = bottomRight.getX();
    int minY = topLeft.getY(), maxY = bottomRight.getY();

    return minX <= point.getX() && point.getX() <= maxX && minY <= point.getY() && point.getY() <= maxY;
}

Wireframe Wireframe::clippingResult(Wireframe window) {
    Point previous;
    Point current;
    vector<Point> points;
    for (int i=0; i<lines.size(); i++) {
        previous = lines[i].start();
        current = lines[i].end();
        if (isInClip(previous, window) && isInClip(current, window)) {
            points.push_back(current);
        } else if (isInClip(previous, window) && !isInClip(current, window)) {
            points.push_back(intersect(previous, current, window));
        } else if (!isInClip(previous, window) && isInClip(current, window)) {
            points.push_back(intersect(current, previous, window));
            points.push_back(current);
        }
    }

    if (points.size() == 0) {
        return Wireframe();
    } else {
        Wireframe wireframe(points, innerPoint);
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