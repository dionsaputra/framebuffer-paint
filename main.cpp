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
    window = createRectangle(Point(250,50), Point(xres-50, yres-50));
    window.setBorderColor(Color(0,250,0));
    drawer.draw_wireframe(window);
}

void setupHorizontalScrollBar() {
    int xres = drawer.vinfo.xres, yres = drawer.vinfo.yres;
    horizontalScrollBar = createRectangle(Point(250,yres-50), Point(xres-50, yres-30));
    horizontalScrollBar.setBorderColor(Color(0,250,0));
    drawer.draw_wireframe(horizontalScrollBar);
}

void setupVerticalScrollBar() {
    int xres = drawer.vinfo.xres, yres = drawer.vinfo.yres;
    verticalScrollBar = createRectangle(Point(xres-50,50), Point(xres-30, yres-50));
    verticalScrollBar.setBorderColor(Color(0,250,0));
    drawer.draw_wireframe(verticalScrollBar);
}

void setup() {
    setupWindow();
    setupHorizontalScrollBar();
    setupVerticalScrollBar();
}

int main() {    

    setup();
    
    // Load file
    string inputCommand;
    for (int i=1; i<=100; i++) printf("\n");
    cout << "filename : ";
    cin >> filename;
    wireframes = controller.load(filename);
    drawer.draw_canvas(wireframes, window);
    // drawer.drawLineWidth(Point(100, 100), Point(200, 200), 2, Color(123, 32, 231));
    // drawer.drawLineWidth(Point(200, 100), Point(300, 200), 10, Color(123, 32, 231));

    // Setup input mode
    struct termios oldSettings, newSettings;

    tcgetattr( fileno( stdin ), &oldSettings );
    newSettings = oldSettings; 
    newSettings.c_lflag &= (~ICANON & ~ECHO); // one key mode

    // Load file
    flush(cout);

    // Receive command
    while(1){

        cout << "$";
        flush(cout);
        cin >> inputCommand;

        if(inputCommand == "select"){
            cout << "=----list----" << endl;
            for (auto itr = wireframes.begin(); itr!=wireframes.end();itr++){
                if (currentWireframe == itr->first){
                    cout << itr->first << " Selected" << endl;
                } else {
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
            cout << "Enter 'x' to exit from scroll mode" << endl;
            while (1){
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
                } else if(c=='x'){
                    // Change settings
                    tcsetattr( fileno( stdin ), TCSANOW, &oldSettings );    
                    break;
                }
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
                    drawer.erase_canvas(wireframes);
                    wireframes.find(currentWireframe)->second.translate(0, -10);
                } else if (c == 'a'){
                    drawer.erase_canvas(wireframes);
                    wireframes.find(currentWireframe)->second.translate(-10, 0);
                } else if (c == 's'){
                    drawer.erase_canvas(wireframes);
                    wireframes.find(currentWireframe)->second.translate(0, 10);
                } else if (c == 'd'){
                    drawer.erase_canvas(wireframes);
                    wireframes.find(currentWireframe)->second.translate(10, 0);
                } else if(c=='x'){
                    // Change settings
                    tcsetattr( fileno( stdin ), TCSANOW, &oldSettings );    
                    break;
                }

                drawer.draw_canvas(wireframes,window);
            }
        } else if (inputCommand == "rotate" && currentWireframe != ""){
            int degree;
            cout << "degree: ";
            cin >> degree;

            drawer.erase_canvas(wireframes);

            wireframes.find(currentWireframe)->second.rotate(degree);
            drawer.draw_canvas(wireframes,window);
        } else if (inputCommand == "scale" && currentWireframe != "") {
            float scale;
            // scanf("%f", &scale);
            cin >> scale;
            cout << scale << endl;

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
        } else if (inputCommand == "edit-line" && currentWireframe != "") {
            float thickness;
            char lineStyle;

            cout << "thickness: ";
            cin >> thickness;
            cout << "line style (d/s): ";
            cin >> lineStyle;
            
            drawer.erase_canvas(wireframes);
            if(thickness >= 1.0f){
                // cout << "before " << wireframes.find(currentWireframe)->second.getThickness() << thickness << endl;
                wireframes.find(currentWireframe)->second.setThickness(thickness);
                // cout << "after " << wireframes.find(currentWireframe)->second.getThickness() << endl;
            }   

            if(lineStyle == 's' || lineStyle == 'd'){
                wireframes.find(currentWireframe)->second.setLineStyle(lineStyle);
            }
            drawer.draw_canvas(wireframes,window);
        } else {
            cout << "Please enter a valid command" << endl;
        }
    }

    tcsetattr( fileno( stdin ), TCSANOW, &oldSettings );

}
