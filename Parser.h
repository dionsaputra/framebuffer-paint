#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <iterator>
#include "Wireframe.h"
#include "Point.h"

using namespace std;

class Parser {

public:
    
    map<string,Wireframe> parseFile(string fileName);
    vector<string> split(string str, string sep);
    void save(map<string,Wireframe> map, string filename);

};

#endif