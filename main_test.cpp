#include "Drawer.h"
#include "Point.h"
#include "Color.h"
#include "Letter.h"
#include <string>

int main() {
    Drawer drawer;
    Point p(600, 200);
    Color green(0, 255, 0);
    Letter A('M', p, 5, green);
    A.printInfo();
    // drawer.draw_word("ILYAS", p, 50, 10, green);
    string name = "ADAM";
    drawer.draw_word(name, p, 30, 5, Color(255,255,255));
    return 0;
}