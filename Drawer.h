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

#include "Point.h"
#include "Color.h"
#include "Line.h"
#include "Wireframe.h"
#include "Map.h"
#include "Window.h"

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
    
    /*** Return the color of point in screen by reading framebuffer or background color if its outside screen ***/
    Color getColor(Point point);
    
    /*** Draw point by background color ***/
    void erase_point(Point point);

    /*** Draw all point in line with line color ***/
    void draw_line(Line line);

    /*** Draw line by background color ***/
    void erase_line(Line line);

    /*** Draw all line in wireframe with its color ***/
    void draw_wireframe(Wireframe wireframe);

    /*** Erase all line in wireframes ***/
    void erase_wireframe(Wireframe wireframe);
    
    /*** Draw and fill all wireframes in map ***/
    void draw_Map(Map map, Wireframe window);

    /*** Erase and unfill all wireframes in map ***/
    void erase_Map(Map map);

    void fill_util(int x, int y, Color prevColor, Color newColor, Point topLeft, Point bottomRight, Point topLeft2, Point bottomRight2);

    void fill_wireframe(Wireframe wireframe, Point topLeft = Point(0,0), Point bottomRight = Point(-1,-1));

    void unfill_wireframe(Wireframe wireframe);

    void clear_window(Wireframe window);

    void queueFloodFill(Wireframe wireframe);
};

#endif