#include "Drawer.h"
#include "Point.h"
#include "Color.h"
#include "Letter.h"

int main() {
    Drawer drawer;
    Point p(600, 200);
    Color green(0, 255, 0);
    Letter A('M', p, 5, green);
    A.printInfo();
    drawer.draw_letter(A);
    return 0;
}