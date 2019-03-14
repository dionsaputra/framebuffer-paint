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
#include <map>
#include <iterator>
#include "Drawer.h"
#include "Point.h"
#include "Color.h"
#include "Parser.h"
#include "Wireframe.h"
#include "PaintController.h"
using namespace std;

#include <stdio.h>
#include <sys/types.h>
#include <termios.h>

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
            cout << "Use IJKL to move " << currentWireframe << " object" << endl;
            cout << "Enter 'x' to finish" << endl;
            while (1){
                char c;
                read( fileno( stdin ), &c, 1 );
                if (c == 'i'){
                    drawer.erase_canvas(wireframes);
                    wireframes.find(currentWireframe)->second.translate(0, -10);
                } else if (c == 'j'){
                    drawer.erase_canvas(wireframes);
                    wireframes.find(currentWireframe)->second.translate(-10, 0);
                } else if (c == 'k'){
                    drawer.erase_canvas(wireframes);
                    wireframes.find(currentWireframe)->second.translate(0, 10);
                } else if (c == 'l'){
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
        } else {
            cout << "Please enter a valid command" << endl;
        }
    }

    tcsetattr( fileno( stdin ), TCSANOW, &oldSettings );

}
