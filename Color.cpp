#include "Color.h"

Color::Color(){
    red = 0;
    green = 0;
    blue = 0;
}

Color::Color(int _red, int _green, int _blue) {
    red = _red;
    green = _green;
    blue = _blue;
}

Color::Color(const Color& _color){
    red = _color.red;
    green = _color.green;
    blue = _color.blue;
}

Color::~Color(){}

Color& Color::operator=(const Color& _color){
    red = _color.red;
    green = _color.green;
    blue = _color.blue;
    return *this;
}

bool Color::isEqual(Color _color) {
    return (red == _color.getRed()) && (green == _color.getGreen()) && (blue == _color.getBlue());
}

Color Color::background() {
    Color black(1,1,1);
    return black;
}

int Color::getRed() {
    return red;
}

int Color::getGreen() {
    return green;
}

int Color::getBlue() {
    return blue;
}

void Color::setRed(int _red) {
    red = _red;
}

void Color::setBlue(int _blue) {
    blue = _blue;
}

void Color::setGreen(int _green){
    green = _green;
}

void Color::display() {
    cout << "{" << red << "," << green << "," << blue << "}" << endl;
}