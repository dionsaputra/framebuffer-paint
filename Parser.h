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
    vector<string> split(string str, string sep);
};

#endif