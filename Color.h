#ifndef COLOR_H
#define COLOR_H

#include <iostream> 
using namespace std;

class Color {
private:
    int red;
    int green;
    int blue;

public:
    Color();
    Color(int _red, int _green, int _blue);
    Color(const Color& _color);
    ~Color();

    Color& operator=(const Color& _color);

    bool isEqual(Color _color);
    static Color background();

    // getter
    int getRed();
    int getGreen();
    int getBlue();

    // setter
    void setRed(int _red);
    void setGreen(int _green);
    void setBlue(int _blue);
    
    void display();
};
#endif