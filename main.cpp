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
Wireframe statusBar;

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
    window = createRectangle(Point(50,50), Point(xres-76, yres-176));
    window.setBorderColor(Color(0,250,0));
    drawer.draw_wireframe(window);
}

void setupHorizontalScrollBar() {
    int xres = drawer.vinfo.xres, yres = drawer.vinfo.yres;
    horizontalScrollBar = createRectangle(Point(50,yres-175), Point(xres-50, yres-150));
    horizontalScrollBar.setBorderColor(Color(0,250,0));
    drawer.draw_wireframe(horizontalScrollBar);
}

void setupVerticalScrollBar() {
    int xres = drawer.vinfo.xres, yres = drawer.vinfo.yres;
    verticalScrollBar = createRectangle(Point(xres-75,50), Point(xres-50, yres-176));
    verticalScrollBar.setBorderColor(Color(0,250,0));
    drawer.draw_wireframe(verticalScrollBar);
}

void setupStatusBar() {
    int xres = drawer.vinfo.xres, yres = drawer.vinfo.yres;
    statusBar = createRectangle(Point(50,yres-125), Point(xres-50, yres-25));
    statusBar.setBorderColor(Color(0,250,0));
    drawer.draw_wireframe(statusBar);
}

void setup() {
    setupWindow();
    setupHorizontalScrollBar();
    setupVerticalScrollBar();
    setupStatusBar();
}

int main() {    

    setup();

    // Build scrollbar
    // Point corner
    
    // Load file
    string inputCommand;
    cout << "filename : ";
    cin >> filename;
    wireframes = controller.load(filename);
    drawer.draw_canvas(wireframes, window);

    // Setup input mode
    struct termios oldSettings, newSettings;

    tcgetattr( fileno( stdin ), &oldSettings );
    newSettings = oldSettings; 
    newSettings.c_lflag &= (~ICANON & ~ECHO); // one key mode

    fd_set set;
    struct timeval tv;

    tv.tv_sec = 10;
    tv.tv_usec = 0;

    FD_ZERO( &set );
    FD_SET( fileno( stdin ), &set );

    int res = select( fileno( stdin )+1, &set, NULL, NULL, &tv );
    // tcsetattr( fileno( stdin ), TCSANOW, &newSettings ); // change to one key mode
    // Load file


    // Receive command
    while(1){
        cout << "$";
        flush(cout);
        cin >> inputCommand;

        if(inputCommand == "select"){
            cout << "----list----" << endl;
            for (auto itr = wireframes.begin(); itr!=wireframes.end();itr++){
                if (currentWireframe == itr->first){
                    cout << itr->first << " Selected" << endl;
                }else{
                    cout << itr->first << endl;
                }
            }
            cout << "------------" << endl;
            cin >> currentWireframe;
            cout << currentWireframe <<" Selected" << endl;
        } else if (inputCommand == "save") {
            cout << "filename: ";
            cin >> filename;
            controller.save(wireframes,filename);
            cout << "saved" << endl;
        } else if (inputCommand == "current") {
            cout << currentWireframe << endl;
        } else if(inputCommand == "scroll"){
            tcsetattr( fileno( stdin ), TCSANOW, &newSettings );
            cout << "Use WASD to navigate" << endl;
            cout << "Enter 'i' to exit from scroll mode" << endl;
            while (1){
                if( res > 0 ){
                    char c;
                    read( fileno( stdin ), &c, 1 );
                    // printf( "Input available %c %d\n",c,c);
                    if (c == 'w'){
                        // Scroll up
                    } else if (c == 'a'){
                        // Scroll left
                    } else if (c == 's'){
                        // Scroll down
                    } else if (c == 'd'){
                        // Scroll right
                    } else if(c=='i'){
                        // Change settings
                        tcsetattr( fileno( stdin ), TCSANOW, &oldSettings );    
                        break;
                    }
                }
                else if( res < 0 )
                {
                    perror( "select error" );
                    break;
                }
                else
                {
                    printf( "Select timeout\n" );
                }
            }
        } else if (inputCommand == "exit") {
            exit(1);
        } else if (inputCommand == "translate" && currentWireframe != ""){
            int move_x, move_y;
            cout << "x y: ";
            cin >> move_x >> move_y;
            
            drawer.erase_canvas(wireframes);

            wireframes.find(currentWireframe)->second.translate(move_x, move_y);
            drawer.draw_canvas(wireframes,window);

        } else if (inputCommand == "rotate" && currentWireframe != ""){
            int degree;
            cout << "degree: ";
            cin >> degree;

            drawer.erase_canvas(wireframes);

            wireframes.find(currentWireframe)->second.rotate(degree);
            drawer.draw_canvas(wireframes,window);
        } else if (inputCommand == "scale" && currentWireframe != "") {
            int scale;
            cout << "scale: ";
            cin >> scale;
            
            drawer.erase_canvas(wireframes);

            wireframes.find(currentWireframe)->second.scale(scale);
            drawer.draw_canvas(wireframes,window);
        } else if (inputCommand == "fill" && currentWireframe != "") {
            int red, green, blue;
            cout << "fill color (r g b): ";
            cin >> red >> green >> blue;
            
            drawer.erase_canvas(wireframes);
            wireframes.find(currentWireframe)->second.setFillColor(Color(red, green, blue));
            drawer.draw_canvas(wireframes,window);
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
            cout << "color (r g b):";
            cin >> red >> green >> boy;

            Wireframe wireframe(radius, nPoint, Point(xCenter, yCenter), Color(red, green, boy));
            wireframes.insert(pair<string, Wireframe>(nameShape, wireframe));
            drawer.draw_canvas(wireframes,window);
        } else {
            cout << "Please enter a valid command" << endl;
        }
    }

    tcsetattr( fileno( stdin ), TCSANOW, &oldSettings );
    
}
