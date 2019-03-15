#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <sys/types.h>
#include <termios.h>
#include <ctype.h>
#include <time.h>
#include <vector>
#include <map>
#include <iterator>
#include "Drawer.h"
#include "Point.h"
#include "Color.h"
#include "Parser.h"
#include "Wireframe.h"
#include "PaintController.h"
#include "Letter.h"
#define CANVAS_WIDTH 1200
#define CANVAS_LENGTH 900
using namespace std;


// Global variable
PaintController controller;
Drawer drawer;
Parser parser;
string filename;

vector<Point> cornerWindow;
string currentWireframe;
map<string,Wireframe> wireframes;

Wireframe window;
Wireframe horizontalScrollBar;
Wireframe verticalScrollBar;
Wireframe horizontalScrollBarBorder;
Wireframe verticalScrollBarBorder;
Wireframe statusBar;
Point disorientation;
float diffX,diffY;
bool useBatik;

Wireframe createRectangle(Point topLeft, Point bottomRight) {
    vector<Point> points;
    points.push_back(topLeft); 
    points.push_back(Point(bottomRight.getX(), topLeft.getY()));
    points.push_back(bottomRight);
    points.push_back(Point(topLeft.getX(), bottomRight.getY()));

    Wireframe wireframe;
    wireframe.setPoints(points);
    wireframe.updateEnvelope();
    return wireframe;
}

void setupWindow() {
    int xres = drawer.vinfo.xres, yres = drawer.vinfo.yres;
    window = createRectangle(Point(350,50), Point(xres-50, yres-250));
    window.setBorderColor(Color(0,250,0));
    disorientation = window.getTopLeft();
    useBatik = false;
    diffX = (float) (xres-350)/CANVAS_WIDTH;
    diffY = (float) (yres-100)/CANVAS_LENGTH;
    drawer.draw_wireframe(window);

    statusBar = createRectangle(Point(350,yres-230),Point(xres-30,yres-50));
    statusBar.setBorderColor(Color(0,250,0));
    drawer.draw_wireframe(statusBar);
}

void setupHorizontalScrollBarBorder() {
    int xres = drawer.vinfo.xres, yres = drawer.vinfo.yres;
    horizontalScrollBarBorder = createRectangle(Point(350,yres-249), Point(xres-50, yres-230));
    horizontalScrollBarBorder.setBorderColor(Color(0,250,0));
    drawer.draw_wireframe(horizontalScrollBarBorder);
}

void setupVerticalScrollBarBorder() {
    int xres = drawer.vinfo.xres, yres = drawer.vinfo.yres;
    verticalScrollBarBorder = createRectangle(Point(xres-49,50), Point(xres-31, yres-250));
    verticalScrollBarBorder.setBorderColor(Color(0,250,0));
    drawer.draw_wireframe(verticalScrollBarBorder);
}

void setupScrollbars() {
    int xres = drawer.vinfo.xres, yres = drawer.vinfo.yres;
    horizontalScrollBar = createRectangle(Point(450,yres-249), Point(xres-250, yres-230));
    horizontalScrollBar.setBorderColor(Color(0,250,0));
    horizontalScrollBar.setFillColor(Color(0,250,0));
    horizontalScrollBar.setInnerPoint(Point((450+xres-250)/2, (yres-249+yres-230)/2));
    drawer.draw_wireframe(horizontalScrollBar);
    drawer.queueFloodFill(horizontalScrollBar);

    verticalScrollBar = createRectangle(Point(xres-50,250), Point(xres-30, yres-450));
    verticalScrollBar.setBorderColor(Color(0,250,0));
    verticalScrollBar.setFillColor(Color(0,250,0));
    verticalScrollBar.setInnerPoint(Point((xres-50+xres-30)/2, (250+yres-450)/2));
    drawer.draw_wireframe(verticalScrollBar);
    drawer.queueFloodFill(verticalScrollBar);
}

void resizeHorizontalScrollBar(int dx){
    int xres = drawer.vinfo.xres, yres = drawer.vinfo.yres;
    Point newTopLeft =  Point(horizontalScrollBar.getTopLeft().getX()+dx,yres-249);
    Point newBottomRight = Point(horizontalScrollBar.getBottomRight().getX()-dx,yres-230);
    horizontalScrollBar = createRectangle(newTopLeft, newBottomRight);
    horizontalScrollBar.setBorderColor(Color(0,250,0));
    horizontalScrollBar.setFillColor(Color(0,250,0));
    horizontalScrollBar.setInnerPoint(Point((newTopLeft.getX() + newBottomRight.getX())/2, (newTopLeft.getY() + newBottomRight.getY())/2));
}

void resizeVerticalScrollBar(int dy){
    int xres = drawer.vinfo.xres, yres = drawer.vinfo.yres;
    Point newTopLeft =  Point(xres-50,verticalScrollBar.getTopLeft().getY()+dy);
    Point newBottomRight = Point(xres-30,verticalScrollBar.getBottomRight().getY()-dy);
    verticalScrollBar = createRectangle(newTopLeft, newBottomRight);
    verticalScrollBar.setBorderColor(Color(0,250,0));
    verticalScrollBar.setFillColor(Color(0,250,0));
    verticalScrollBar.setInnerPoint(Point((newTopLeft.getX() + newBottomRight.getX())/2, (newTopLeft.getY() + newBottomRight.getY())/2));
}

void redrawScrollbars(){
    drawer.draw_wireframe(horizontalScrollBar);
    drawer.draw_wireframe(verticalScrollBar);
    drawer.queueFloodFill(horizontalScrollBar);
    drawer.queueFloodFill(verticalScrollBar);
    drawer.draw_wireframe(horizontalScrollBarBorder);
    drawer.draw_wireframe(verticalScrollBarBorder);
}

void scroll(int dx, int dy){
    
    int leftScrollbarBorder = horizontalScrollBarBorder.getTopLeft().getX();
    int rightScrollbarBorder = horizontalScrollBarBorder.getBottomRight().getX();
    int topScrollbarBorder = verticalScrollBarBorder.getTopLeft().getY();
    int bottomScrollbarBorder = verticalScrollBarBorder.getBottomRight().getY();

    if (horizontalScrollBar.getTopLeft().getX()+dx >= leftScrollbarBorder 
    && horizontalScrollBar.getBottomRight().getX()+dx <= rightScrollbarBorder
    && verticalScrollBar.getTopLeft().getY()+dy >= topScrollbarBorder
    && verticalScrollBar.getBottomRight().getY()+dy <= bottomScrollbarBorder){ 

        drawer.erase_wireframe(horizontalScrollBar);
        drawer.unfill_wireframe(horizontalScrollBar);
        drawer.erase_wireframe(verticalScrollBar);
        drawer.unfill_wireframe(verticalScrollBar);

        drawer.erase_canvas(wireframes,disorientation);

        if (dx != 0)
            horizontalScrollBar.translate(dx,dy);
        if (dy != 0)
            verticalScrollBar.translate(dx,dy);
        for (auto itr = wireframes.begin(); itr!=wireframes.end();itr++){
            itr->second.translate(-dx,-dy);
        }
        drawer.draw_canvas(wireframes,window,disorientation,true,useBatik);
        redrawScrollbars();
    }
}

bool checkZoomOut(int dx, int dy){
    int leftScrollbarBorder = horizontalScrollBarBorder.getTopLeft().getX();
    int rightScrollbarBorder = horizontalScrollBarBorder.getBottomRight().getX();
    int topScrollbarBorder = verticalScrollBarBorder.getTopLeft().getY();
    int bottomScrollbarBorder = verticalScrollBarBorder.getBottomRight().getY();

    return (dx > 0 && horizontalScrollBar.getTopLeft().getX()+dx >= leftScrollbarBorder 
    && horizontalScrollBar.getBottomRight().getX()-dx <= rightScrollbarBorder
    && ((horizontalScrollBar.getBottomRight().getX() - horizontalScrollBar.getTopLeft().getX()) > 100)
    && verticalScrollBar.getTopLeft().getY()-dy >= topScrollbarBorder
    && verticalScrollBar.getBottomRight().getY()+dy <= bottomScrollbarBorder
    && (verticalScrollBar.getBottomRight().getY() - verticalScrollBar.getTopLeft().getY()> 55));
}

bool checkZoomIn(int dx, int dy){
    int leftScrollbarBorder = horizontalScrollBarBorder.getTopLeft().getX();
    int rightScrollbarBorder = horizontalScrollBarBorder.getBottomRight().getX();
    int topScrollbarBorder = verticalScrollBarBorder.getTopLeft().getY();
    int bottomScrollbarBorder = verticalScrollBarBorder.getBottomRight().getY();

    return (dx < 0 && horizontalScrollBar.getTopLeft().getX()+dx >= leftScrollbarBorder 
    && horizontalScrollBar.getBottomRight().getX()-dx <= rightScrollbarBorder
    && verticalScrollBar.getTopLeft().getY()-dy >= topScrollbarBorder
    && verticalScrollBar.getBottomRight().getY()+dy <= bottomScrollbarBorder);
}

void zoom(float scale){
    int dx,dy;
    int xres = drawer.vinfo.xres, yres = drawer.vinfo.yres;
    
    if (scale < 1){
        dx = -35;
        dy = -15;
    } else {
        dx = 35;
        dy = 15;
    }

    if (checkZoomOut(dx,dy) || checkZoomIn(dx,dy)){ 

        drawer.erase_wireframe(horizontalScrollBar);
        drawer.unfill_wireframe(horizontalScrollBar);
        drawer.erase_wireframe(verticalScrollBar);
        drawer.unfill_wireframe(verticalScrollBar);

        drawer.erase_canvas(wireframes,disorientation);
        for (auto itr = wireframes.begin(); itr!=wireframes.end();itr++){
            itr->second.scale(Point((250+xres-50)/2, (50+yres-50)/2), scale);
        }
        
        resizeHorizontalScrollBar(dx);
        resizeVerticalScrollBar(dy);
        drawer.draw_canvas(wireframes,window,disorientation,true,useBatik);
        redrawScrollbars();
        drawer.draw_wireframe(horizontalScrollBarBorder);
        drawer.draw_wireframe(verticalScrollBarBorder);
    }
}

void moveScrollBar(Wireframe* scrollbar,int x,int y){
    drawer.erase_wireframe(*scrollbar);
    drawer.unfill_wireframe(*scrollbar);
    scrollbar->translate(x,y);
    drawer.draw_wireframe(*scrollbar);
    drawer.queueFloodFill(*scrollbar);
    drawer.draw_wireframe(horizontalScrollBarBorder);
    drawer.draw_wireframe(verticalScrollBarBorder);
}

void setup() {
    setupWindow();
    setupHorizontalScrollBarBorder();
    setupVerticalScrollBarBorder();
    setupScrollbars();
    redrawScrollbars();
}

void hideLabels(){
    for (auto itr = wireframes.begin(); itr != wireframes.end(); itr++) {
        string name =  itr->first;
        Point loc = itr->second.getInnerPoint();
        loc.translate(200, 0);
        drawer.draw_word(name, loc, 25, 3, Color(0,0,0));
    }
    drawer.draw_canvas(wireframes,window,disorientation,true,useBatik);
}

int main() {        
    // Load file
    string inputCommand;
    for (int i=1; i<=100; i++) printf("\n");
    setup();
    cout << "filename : ";
    cin >> filename;
    wireframes = controller.load(filename);
    drawer.draw_canvas(wireframes, window, disorientation);

    // Setup input mode
    struct termios oldSettings, newSettings;

    tcgetattr( fileno( stdin ), &oldSettings );
    newSettings = oldSettings; 
    newSettings.c_lflag &= (~ICANON & ~ECHO); // one key mode

    // Load file
    flush(cout);

    // Receive command
    while(1){
        printf("$ ");
        flush(cout);
        cin >> inputCommand;

        hideLabels();

        if(inputCommand == "select"){
            cout << "----list----" << endl;
            for (auto itr = wireframes.begin(); itr!=wireframes.end();itr++){
                if (currentWireframe == itr->first){
                    cout << itr->first << " (selected)" << endl;
                } else {
                    cout << itr->first << endl;
                }
            }
            cout << "------------" << endl;
            cin >> currentWireframe;
            for (auto & c: currentWireframe) c = toupper(c);
            cout << currentWireframe <<" selected" << endl;
        } else if (inputCommand == "save") {
            cout << "filename: ";
            cin >> filename;
            controller.save(wireframes,filename);
            cout << "saved" << endl;
        } else if (inputCommand == "current") {
            cout << currentWireframe << endl;
            int xres = drawer.vinfo.xres, yres = drawer.vinfo.yres;
        } else if(inputCommand == "scroll"){
            tcsetattr( fileno( stdin ), TCSANOW, &newSettings );
            cout << "Use WASD to navigate" << endl;
            cout << "Enter 'x' to return" << endl;
            while (1){
                char c;
                read( fileno( stdin ), &c, 1 );
                // printf( "Input available %c %d\n",c,c);
                if (c == 'w' && verticalScrollBar.getTopLeft().getY()>50){
                    drawer.erase_canvas(wireframes,disorientation);
                    disorientation.translate(0,5);
                    moveScrollBar(&verticalScrollBar,0,-5 * diffY);
                    // Scroll up
                } else if (c == 'a' && horizontalScrollBar.getTopLeft().getX()>350){
                    drawer.erase_canvas(wireframes,disorientation);
                    disorientation.translate(5,0);
                    moveScrollBar(&horizontalScrollBar,-5 * diffX,0);
                    // Scroll left
                } else if (c == 's' && verticalScrollBar.getBottomRight().getY()<drawer.vinfo.yres-250){
                    drawer.erase_canvas(wireframes,disorientation);
                    disorientation.translate(0,-5);
                    moveScrollBar(&verticalScrollBar,0,5*diffY);
                    // Scroll down
                } else if (c == 'd' && horizontalScrollBar.getBottomRight().getX()<drawer.vinfo.xres-50){
                    drawer.erase_canvas(wireframes,disorientation);
                    disorientation.translate(-5,0);
                    moveScrollBar(&horizontalScrollBar,5*diffX,0);
                    // Scroll right
                } else if(c=='x'){
                    // Change settings
                    tcsetattr( fileno( stdin ), TCSANOW, &oldSettings );    
                    break;
                }
                
                drawer.draw_canvas(wireframes,window,disorientation,true,useBatik);
            }
        } else if (inputCommand == "exit") {
            exit(1);
        } else if (inputCommand == "translate" && currentWireframe != ""){
            tcsetattr( fileno( stdin ), TCSANOW, &newSettings );
            cout << "Use WASD to move " << currentWireframe << " object" << endl;
            cout << "Enter 'x' to finish" << endl;
            while (1){
                char c;
                read( fileno( stdin ), &c, 1 );
                if (c == 'w'){
                    drawer.erase_canvas(wireframes,disorientation);
                    wireframes.find(currentWireframe)->second.translate(0, -10);
                } else if (c == 'a'){
                    drawer.erase_canvas(wireframes,disorientation);
                    wireframes.find(currentWireframe)->second.translate(-10, 0);
                } else if (c == 's'){
                    drawer.erase_canvas(wireframes,disorientation);
                    wireframes.find(currentWireframe)->second.translate(0, 10);
                } else if (c == 'd'){
                    drawer.erase_canvas(wireframes,disorientation);
                    wireframes.find(currentWireframe)->second.translate(10, 0);
                } else if(c=='x'){
                    // Change settings
                    tcsetattr( fileno( stdin ), TCSANOW, &oldSettings );    
                    break;
                }

                drawer.draw_canvas(wireframes,window,disorientation,true,useBatik);
                drawer.draw_wireframe(horizontalScrollBarBorder);
                drawer.draw_wireframe(verticalScrollBarBorder);
                drawer.draw_wireframe(window);
            }
        } else if (inputCommand == "rotate" && currentWireframe != ""){
            tcsetattr( fileno( stdin ), TCSANOW, &newSettings );
            cout << "Use 'p' rotate clockwise " << currentWireframe << " object" << endl;
            cout << "Use 'o' rotate counter clockwise " << currentWireframe << " object" << endl;
            cout << "Enter 'x' to finish" << endl;
            while (1){
                char c;
                read( fileno( stdin ), &c, 1 );
                if (c == 'o'){
                    drawer.erase_canvas(wireframes,disorientation);
                    wireframes.find(currentWireframe)->second.rotate(-10);
                } else if (c == 'p'){
                    drawer.erase_canvas(wireframes,disorientation);
                    wireframes.find(currentWireframe)->second.rotate(10);
                } else if(c=='x'){
                    // Change settings
                    tcsetattr( fileno( stdin ), TCSANOW, &oldSettings );    
                    break;
                }
                drawer.draw_canvas(wireframes,window,disorientation,true,useBatik);
                drawer.draw_wireframe(horizontalScrollBarBorder);
                drawer.draw_wireframe(verticalScrollBarBorder);
                drawer.draw_wireframe(window);
            }
        } else if (inputCommand == "zoom") {
            tcsetattr( fileno( stdin ), TCSANOW, &newSettings );
            cout << "Use '=' to zoom in" << endl;
            cout << "Use '-' to zoom out" << endl;
            cout << "Enter 'x' to return" << endl;
            while (1){
                char c;
                read( fileno( stdin ), &c, 1 );
                if (c == '='){
                    zoom(1.15);
                } else if (c == '-'){
                    zoom(0.75);
                } else if(c=='x'){
                    // Change settings
                    tcsetattr( fileno( stdin ), TCSANOW, &oldSettings);    
                    break;
                }
            }
        } else if (inputCommand == "scale" && currentWireframe != "") {
            tcsetattr( fileno( stdin ), TCSANOW, &newSettings );
            cout << "Use '=' to scale up " << currentWireframe << " object" << endl;
            cout << "Use '-' to scale down " << currentWireframe << " object" << endl;
            cout << "Enter 'x' to finish" << endl;
            while (1){
                char c;
                read( fileno( stdin ), &c, 1 );
                if (c == '='){
                    drawer.erase_canvas(wireframes,disorientation);
                    wireframes.find(currentWireframe)->second.scale(1.15);
                } else if (c == '-'){
                    drawer.erase_canvas(wireframes,disorientation);
                    wireframes.find(currentWireframe)->second.scale(0.75);
                } else if(c=='x'){
                    // Change settings
                    tcsetattr( fileno( stdin ), TCSANOW, &oldSettings);    
                    break;
                }
            drawer.draw_canvas(wireframes,window,disorientation,true,useBatik);
            }
        } else if (inputCommand == "fill" && currentWireframe != "") {
            int red, green, blue;
            cout << "fill color (r g b): ";
            cin >> red >> green >> blue;
            
            drawer.erase_canvas(wireframes,disorientation);
            wireframes.find(currentWireframe)->second.setFillColor(Color(red, green, blue));
            drawer.draw_canvas(wireframes,window,disorientation,true,useBatik);
        } else if(inputCommand == "create"){
            int radius, nPoint, xCenter, yCenter, red, green, boy;
            string nameShape;

            cout << "shape's name: ";
            cin >> nameShape;
            cout << "center (x y): ";
            cin >> xCenter >> yCenter;
            cout << "radius: ";
            cin >> radius;
            cout << "number of point: ";
            cin >> nPoint;
            cout << "color (r g b): ";
            cin >> red >> green >> boy;

            for (auto & c: nameShape) c = toupper(c);

            Wireframe wireframe(radius, nPoint, Point(xCenter, yCenter), Color(red, green, boy));
            wireframes.insert(pair<string, Wireframe>(nameShape, wireframe));
            drawer.draw_canvas(wireframes,window,disorientation,true,useBatik);
        } else if (inputCommand == "edit-line" && currentWireframe != "") {
            float thickness;
            char lineStyle;

            cout << "thickness: ";
            cin >> thickness;
            cout << "line style (d/s): ";
            cin >> lineStyle;
            
            drawer.erase_canvas(wireframes,disorientation);
            if(thickness >= 1.0f){
                wireframes.find(currentWireframe)->second.setThickness(thickness);
            }   

            if(lineStyle == 's' || lineStyle == 'd'){
                wireframes.find(currentWireframe)->second.setLineStyle(lineStyle);
            }
            drawer.draw_canvas(wireframes,window, disorientation, true, useBatik);
        } else if (inputCommand == "show-label"){
            for (auto itr = wireframes.begin(); itr != wireframes.end(); itr++) {
                string name =  itr->first;
                Point loc = itr->second.getInnerPoint();
                loc.translate(200, 0);
                drawer.draw_word(name, loc, 25, 3, Color(255,255,255));
            }
        } else if (inputCommand == "hide-label"){
            hideLabels();
        } else if (inputCommand == "use-batik"){
            useBatik = true;
            drawer.erase_canvas(wireframes,disorientation);
            drawer.draw_canvas(wireframes,window,disorientation,true,useBatik);
        }else if (inputCommand == "off-batik"){
            useBatik = false;
            drawer.erase_canvas(wireframes,disorientation);
            drawer.draw_canvas(wireframes,window,disorientation,true,useBatik);
        }else{      
            cout << "Please enter a valid command" << endl;
        }
    }
    tcsetattr( fileno( stdin ), TCSANOW, &oldSettings );
}