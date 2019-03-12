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
#include "Wireframe.h"
#include "Parser.h"
using namespace std;

int main() {
    Drawer drawer;
    Parser parser;
    string filename;
    // cout << "input filename: ";
    // cin >> filename;

    // map<string,Wireframe> wireframes = parser.parseFile(filename);

    // // drawer.clear_screen();
    
    // for (auto itr = wireframes.begin(); itr!=wireframes.end();itr++){
    //     cout << itr->first << endl;
    //     drawer.draw_wireframe(itr->second);
    //     drawer.queueFloodFill(itr->second);
    // }

    // parser.save(wireframes,"test.txt");
    // Point topLeft(drawer.vinfo.xres/2,0);
    // Point bottomRight(drawer.vinfo.xres,drawer.vinfo.yres/2);
    vector<Point> cornerWindow;
    // cout << drawer.vinfo.xres << endl;
    // cout << drawer.vinfo.yres << endl;
    Point cornerWindow_1(drawer.vinfo.xres / 4 - 50, 0), cornerWindow_2(drawer.vinfo.xres - 50, 0), cornerWindow_3(drawer.vinfo.xres - 50, drawer.vinfo.yres - 50), cornerWindow_4(drawer.vinfo.xres / 4 - 50, drawer.vinfo.yres - 50);
    cornerWindow.push_back(cornerWindow_1);
    cornerWindow.push_back(cornerWindow_2);
    cornerWindow.push_back(cornerWindow_3);
    cornerWindow.push_back(cornerWindow_4);    
    Color green(0,250,0);
    Wireframe window(cornerWindow, green);
    drawer.draw_wireframe(window);
    string inputCommand;
    string currentWireframe;
    cout << "input filename: ";
    cin >> filename;

    // <label,wireframe>
    map<string,Wireframe> wireframes = parser.parseFile(filename);

    // drawer.clear_screen();

    while(1){
        for (auto itr = wireframes.begin(); itr!=wireframes.end();itr++){
            // cout << itr->first << endl;
            drawer.draw_wireframe(itr->second);
            drawer.queueFloodFill(itr->second);
        }

        cout << "$";
        cin >> inputCommand;
        // switch
        if (inputCommand == "select"){
            cout << "----list----"<<endl;
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
        }else if (inputCommand == "save"){
            cout << "filename: ";
            cin >> filename;
            parser.save(wireframes,filename);
            cout << "saved" << endl;
        }else if (inputCommand == "current"){
            cout << currentWireframe << endl;
        }

    }
    
}
