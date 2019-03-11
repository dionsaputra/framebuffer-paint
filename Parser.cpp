#include "Parser.h"

vector<Wireframe> Parser::parseFile(string fileName) {
    vector<Wireframe> wireframes;

    ifstream file;
    file.open(fileName);
    if (file.is_open()) {
        string line;
        while (getline(file,line)) {
            vector<Point> points;
            vector<string> vec = Map::split(line,"|");
            vector<string> strPoints = Map::split(vec[1]," ");
            for (int i=0; i<strPoints.size(); i++) {
                vector<string> strXY = Map::split(strPoints[i].substr(1,strPoints[i].length()-2),",");
                Point point(stoi(strXY[0]) * 10, stoi(strXY[1]) * 10);
                points.push_back(point);
            }
            vector<string>strXY = Map::split(vec[2].substr(1,vec[2].length()-2),",");
            Point innerPoint(stoi(strXY[0]) * 10, stoi(strXY[1]) * 10);
            Wireframe wireframe(points, innerPoint);
            wireframes.push_back(wireframe);
        }
    }

    return wireframes;
}

vector<string> Parser::split(string str, string sep){
    char* cstr=const_cast<char*>(str.c_str());
    char* current;
    std::vector<std::string> arr;
    current=strtok(cstr,sep.c_str());
    while(current!=NULL){
        arr.push_back(current);
        current=strtok(NULL,sep.c_str());
    }
    return arr;
}