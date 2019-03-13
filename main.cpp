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

void drawObjects(){
    for (auto itr = wireframes.begin(); itr!=wireframes.end();itr++){
            // cout << itr->first << endl;
            drawer.draw_wireframe(itr->second);
            drawer.queueFloodFill(itr->second);
    }
}

int main() {    
    // Build window
    Point cornerWindow_1(drawer.vinfo.xres / 4 - 50, 0), cornerWindow_2(drawer.vinfo.xres - 50, 0), cornerWindow_3(drawer.vinfo.xres - 50, drawer.vinfo.yres - 50), cornerWindow_4(drawer.vinfo.xres / 4 - 50, drawer.vinfo.yres - 50);
    cornerWindow.push_back(cornerWindow_1);
    cornerWindow.push_back(cornerWindow_2);
    cornerWindow.push_back(cornerWindow_3);
    cornerWindow.push_back(cornerWindow_4);    
    Color green(0,250,0);
    Wireframe window(cornerWindow, green);
    drawer.draw_wireframe(window);

    Point centerPoint(600,600);
    Wireframe polygon(50, 3, centerPoint, green);
    drawer.draw_wireframe(polygon);
    
    // Build scrollbar
    // Point corner
    
    // Load file
    string inputCommand;
    cout << "filename : ";
    cin >> filename;
    wireframes = controller.load(filename);
    drawObjects();

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
            
            for (auto itr = wireframes.begin(); itr!=wireframes.end();itr++){
                // cout << itr->first << endl;
                drawer.erase_wireframe(itr->second);
                drawer.unfill_wireframe(itr->second);
            }

            wireframes.find(currentWireframe)->second.translate(move_x, move_y);
            drawObjects();
        } else if (inputCommand == "rotate" && currentWireframe != ""){
            int degree;
            cout << "degree: ";
            cin >> degree;

            for (auto itr = wireframes.begin(); itr!=wireframes.end();itr++){
                // cout << itr->first << endl;
                drawer.erase_wireframe(itr->second);
                drawer.unfill_wireframe(itr->second);
            }

            wireframes.find(currentWireframe)->second.rotate(degree);
            drawObjects();
        } else if (inputCommand == "scale" && currentWireframe != "") {
            int scale;
            cout << "scale: ";
            cin >> scale;
            
            for (auto itr = wireframes.begin(); itr!=wireframes.end();itr++){
                // cout << itr->first << endl;
                drawer.erase_wireframe(itr->second);
                drawer.unfill_wireframe(itr->second);
            }

            wireframes.find(currentWireframe)->second.scale(scale);
            drawObjects();
        } else {
            cout << "Please enter a valid command" << endl;
        }
    }

    tcsetattr( fileno( stdin ), TCSANOW, &oldSettings );
    
}
