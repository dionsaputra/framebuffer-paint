#include "Point.h"

Point::Point() {
    x = 0; y = 0;
}

Point::Point(int _x, int _y) {
    x = _x; y = _y;
}

Point::Point(const Point& _point){
    x = _point.x; y =_point.y;
}

Point::~Point(){}

Point& Point::operator=(const Point& _point){
    x = _point.x; y = _point.y;
    return *this;
}

void Point::translate(int dx, int dy){
    x += dx;
    y += dy;
}

void Point::rotate(Point center, int degree) {
    float radian = degree * M_PI / 180.0;
    float sin_r = sin(radian);
    float cos_r = cos(radian);

    x -= center.x;
    y -= center.y;

    float x_new = x*cos_r - y*sin_r;
    float y_new = x*sin_r + y*cos_r;

    x = (int) (x_new + center.x);
    y = (int) (y_new + center.y);
}

void Point::scale(Point center, float skala){
    x -= center.x;
    y -= center.y;

    float x_new = x*skala;
    float y_new = y*skala;

    x = (int) (x_new + center.x);
    y = (int) (y_new + center.y);
}

void Point::display() {
    cout << "{" << x << "," << y << "}" << endl;
}

int Point::getX(){
    return x;
}

int Point::getY(){
    return y;
}

void Point::setX(int _x){
    x = _x;
}

void Point::setY(int _y){
    y = _y;
}

bool Point::isEqual(Point _point){
    return this->x == _point.x && this->y == _point.y;
};

Point Point::getLeft() {
    return Point(x-1, y);
}

Point Point::getRight() {
    return Point(x+1, y);
}

Point Point::getTop() {
    return Point(x, y-1);
}

Point Point::getBottom() {
    return Point(x, y+1);
}