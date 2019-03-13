#ifndef PAINT_CONTROLLER_H
#define PAINT_CONTROLLER_H 

#include <bits/stdc++.h>
#include "Parser.h"
#include "Wireframe.h"

class PaintController {
public:
    map<string, Wireframe> load(string filename);
    void save(map<string,Wireframe> map, string filename);
    void receiveCommand(string command);
};

#endif