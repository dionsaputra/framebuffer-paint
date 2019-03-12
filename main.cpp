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
    string inputCommand;
    string currentWireframe;
    cout << "input filename: ";
    cin >> filename;

    // <label,wireframe>
    map<string,Wireframe> wireframes = parser.parseFile(filename);

    drawer.clear_screen();

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
