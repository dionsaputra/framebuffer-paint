#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <string>
#include <vector>
#include "Wireframe.h"
#include "Point.h"

using namespace std;

class Parser {
private:
    vector<Wireframe> wireframes;
    vector<string> arr_str;
public:
    Parser();
    ~Parser();
    vector<Wireframe> parseFile(string fileName);
    vector<string> split(string str, string sep);
};

#endif