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
    cout << "input filename: ";
    cin >> filename;

    map<string,Wireframe> wireframes = parser.parseFile(filename);

    drawer.clear_screen();
    
    for (auto itr = wireframes.begin(); itr!=wireframes.end();itr++){
        cout << itr->first << endl;
        drawer.draw_wireframe(itr->second);
        drawer.queueFloodFill(itr->second);
    }

    parser.save(wireframes,"test.txt");
}
