#ifndef LETTER_H
#define LETTER_H

#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include "Point.h"
#include "Color.h"

#define GRID_WIDTH 5
#define GRID_HEIGHT 5

using namespace std;

class Letter {
private:
    char character;
    Point startPoint;
    int scale;
    Color color;

public:
    // Constructor
    Letter();
    Letter(char c, Point startPoint, int scale, Color color);

    // Destructor
    ~Letter();

    // Setter
    void setCharacter(char _character);
    void setStartPoint(Point _point);
    void setScale(int _scale);
    void setColor(Color _color);

    void printInfo();

    // Getter
    char getCharacter();
    Point getStartPoint();
    int getScale();
    Color getColor();

    int getLimitX();
    int getLimitY();

    // int limit_x(int scale, int pos_x);
    // int limit_y(int scale, int pos_y);
    int condition_A(int x, int y, int scale, int pos_x, int pos_y);
    int condition_B(int x, int y, int scale, int pos_x, int pos_y);
    int condition_C(int x, int y, int scale, int pos_x, int pos_y);
    int condition_D(int x, int y, int scale, int pos_x, int pos_y);
    int condition_E(int x, int y, int scale, int pos_x, int pos_y);
    int condition_F(int x, int y, int scale, int pos_x, int pos_y);
    int condition_G(int x, int y, int scale, int pos_x, int pos_y);
    int condition_H(int x, int y, int scale, int pos_x, int pos_y);
    int condition_I(int x, int y, int scale, int pos_x, int pos_y);
    int condition_J(int x, int y, int scale, int pos_x, int pos_y);
    int condition_K(int x, int y, int scale, int pos_x, int pos_y);
    int condition_L(int x, int y, int scale, int pos_x, int pos_y);
    int condition_M(int x, int y, int scale, int pos_x, int pos_y);
    int condition_N(int x, int y, int scale, int pos_x, int pos_y);
    int condition_O(int x, int y, int scale, int pos_x, int pos_y);
    int condition_P(int x, int y, int scale, int pos_x, int pos_y);
    int condition_Q(int x, int y, int scale, int pos_x, int pos_y);
    int condition_R(int x, int y, int scale, int pos_x, int pos_y);
    int condition_S(int x, int y, int scale, int pos_x, int pos_y);
    int condition_T(int x, int y, int scale, int pos_x, int pos_y);
    int condition_U(int x, int y, int scale, int pos_x, int pos_y);
    int condition_V(int x, int y, int scale, int pos_x, int pos_y);
    int condition_W(int x, int y, int scale, int pos_x, int pos_y);
    int condition_X(int x, int y, int scale, int pos_x, int pos_y);
    int condition_Y(int x, int y, int scale, int pos_x, int pos_y);
    int condition_Z(int x, int y, int scale, int pos_x, int pos_y);
    int condition_dash(int x, int y, int scale, int pos_x, int pos_y);
    int condition_0(int x, int y, int scale, int pos_x, int pos_y);
    int condition_1(int x, int y, int scale, int pos_x, int pos_y);
    int condition_2(int x, int y, int scale, int pos_x, int pos_y);
    int condition_3(int x, int y, int scale, int pos_x, int pos_y);
    int condition_4(int x, int y, int scale, int pos_x, int pos_y);
    int condition_6(int x, int y, int scale, int pos_x, int pos_y);
    int condition_7(int x, int y, int scale, int pos_x, int pos_y);
    int condition_8(int x, int y, int scale, int pos_x, int pos_y);
    int condition(Point point);

};

#endif
