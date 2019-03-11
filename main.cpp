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
#include "Line.h"
#include "Color.h"
#include "Wireframe.h"
#include "Map.h"
#include "Window.h"
using namespace std;

int main() {
    Drawer drawer;
    
    Color red(10,80,100);
    Color green(0,250,0);
    
    Map map("test.txt",red,green,drawer.vinfo.xres/2,drawer.vinfo.yres, 0);

    Point topLeft(drawer.vinfo.xres/2,0);
    Point bottomRight(drawer.vinfo.xres,drawer.vinfo.yres/2);
    Window window(topLeft,bottomRight,map);
    
    drawer.clear_screen();

    drawer.draw_Map(map, window);

    char c;
    Point center(drawer.vinfo.xres/2,drawer.vinfo.yres/2);
    // while(1){
    //     cin >> c;
    //     switch(c){
    //         case 't':
    //             int dx,dy;
    //             cout << "enter dx dy:";
    //             cin >> dx >> dy;
    //             window.translateMap(dx,dy);
    //             break;
    //         case 'r':
    //             int degree;
    //             cout << "enter degree:";
    //             cin >> degree;
    //             window.rotateMap(degree);
    //             break;
    //         case 's':
    //             float skala;
    //             cout << "enter skala:";
    //             cin >> skala;
    //             window.scaleMap(skala);
    //             break;
    //         case '1':
    //             float skal;
    //             cout << "enter skala:";
    //             cin >> skal;
    //             window.scaleWindow(skal);
    //             break;
    //         case '2':
    //             int x,y;
    //             cout << "enter dx dy:";
    //             cin >> x >> y;
    //             window.translateWindow(x,y);
    //             break;
    //         default:
    //             break;
    //     }
    //     if (c == 'x'){
    //         break;
    //     }
    // }
}
