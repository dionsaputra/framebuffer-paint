#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <time.h>
#include <vector>
#include "Drawer.h"
#include "Point.h"
#include "Color.h"
#include "Wireframe.h"
using namespace std;

int main() {
    Drawer drawer;
    
    Color red(10,80,100);
    Color green(0,250,0);
    vector<Point> controlPoint;
    controlPoint.push_back(Point(100,100));
    controlPoint.push_back(Point(100,200));
    controlPoint.push_back(Point(200,100));

    Wireframe wireframe(controlPoint,red);
    drawer.clear_screen();
    drawer.draw_wireframe(wireframe);
}
