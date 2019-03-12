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
using namespace std;

#include <stdio.h>
#include <sys/types.h>
#include <termios.h>

// thread
#include <cstdlib>
#include <pthread.h>
#define NUM_THREADS 1

// Global variable
Drawer drawer;
Parser parser;
string filename;

vector<Point> cornerWindow;
string currentWireframe;

map<string,Wireframe> wireframes;


void *keyListener(void *threadid){
    // thread identity
    long tid;
    tid = (long)threadid;

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

    // Receive command
    string inputCommand;
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
        } else if (inputCommand == "save"){
            cout << "filename: ";
            cin >> filename;
            parser.save(wireframes,filename);
            cout << "saved" << endl;
        } else if (inputCommand == "current"){
            cout << currentWireframe << endl;
        } else if(inputCommand == "scroll"){
            tcsetattr( fileno( stdin ), TCSANOW, &newSettings );
            while ( 1 ){
                if( res > 0 ){
                    char c;
                    read( fileno( stdin ), &c, 1 );
                    printf( "Input available %c %d\n",c,c);
                    if(c=='i'){
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
        } else {
            cout << "Command tidak ditemukan";
        }
    }


    

    tcsetattr( fileno( stdin ), TCSANOW, &oldSettings );
    return 0;
}

int main() {    
    Point cornerWindow_1(drawer.vinfo.xres / 4 - 50, 0), cornerWindow_2(drawer.vinfo.xres - 50, 0), cornerWindow_3(drawer.vinfo.xres - 50, drawer.vinfo.yres - 50), cornerWindow_4(drawer.vinfo.xres / 4 - 50, drawer.vinfo.yres - 50);
    cornerWindow.push_back(cornerWindow_1);
    cornerWindow.push_back(cornerWindow_2);
    cornerWindow.push_back(cornerWindow_3);
    cornerWindow.push_back(cornerWindow_4);    
    Color green(0,250,0);
    Wireframe window(cornerWindow, green);
    drawer.draw_wireframe(window);
    string inputCommand;
    cout << "input filename: ";
    cin >> filename;
    wireframes = parser.parseFile(filename);

    // Start key listener 
    pthread_t threads[0];
    int rc;
    
    rc = pthread_create(&threads[0], NULL, keyListener, (void *)0);
    
    if (rc) {
        cout << "Error:unable to create thread," << rc << endl;
        exit(-1);
    }

    while(1){
        for (auto itr = wireframes.begin(); itr!=wireframes.end();itr++){
            // cout << itr->first << endl;
            drawer.draw_wireframe(itr->second);
            drawer.queueFloodFill(itr->second);
        }
    }
    
}
