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

    Point topLeft(drawer.vinfo.xres/2,0);
    Point bottomRight(drawer.vinfo.xres,drawer.vinfo.yres/2);
    drawer.clear_screen();

    char c;
    Point center(drawer.vinfo.xres/2,drawer.vinfo.yres/2);
}
