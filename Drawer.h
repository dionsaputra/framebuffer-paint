#ifndef DRAWER_H
#define DRAWER_H
#include <bits/stdc++.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <time.h>

#include <pthread.h>
#include <strings.h>
#include <string.h>
#include <termios.h>
#include <sys/types.h>
#include <map>

#include "Point.h"
#include "Color.h"
#include "Wireframe.h"
#include "Letter.h"

class Drawer {
public:
    int fbfd;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo; 
    long int screen_size;
    char* fbp;   

public:
    /*** Constructor ***/
    Drawer();

    /*** Destructor ***/
    ~Drawer();

    /*** Clear whole screen using erase_point ***/
    void clear_screen();

    /*** Return true if point in frame. Frame is either wireframe envelope or screen ***/
    bool is_in_frame(Point point, Point topLeft = Point(0,0), Point bottomRight = Point(-1,-1));
    
    /*** Return the memory location of framebuffer for point in screen ***/
    long int location(Point point);
    
    /*** Draw point in screen by putting color pixel in framebuffer ***/
    void draw_point(Point point, Color color);

    void drawLine(Point start, Point end, Color color);

    void drawLineWidth(Point pointStart, Point pointEnd, float wd, Color color);
            
    /*** Return the color of point in screen by reading framebuffer or background color if its outside screen ***/
    Color getColor(Point point);
    
    /*** Draw point by background color ***/
    void erase_point(Point point);

    /*** Draw all line in wireframe with its color ***/
    void draw_wireframe(Wireframe wireframe);

    /*** Erase all line in wireframes ***/
    void erase_wireframe(Wireframe wireframe);

    void unfill_wireframe(Wireframe wireframe);

    void queueFloodFill(Wireframe wireframe);

    void draw_canvas(map<string,Wireframe> canvas, Wireframe window);

    void erase_canvas(map<string,Wireframe> canvas);

    void draw_letter(Letter letter);
};

#endif