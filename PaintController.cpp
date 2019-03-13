#include "PaintController.h"
using namespace std;

map<string, Wireframe> PaintController::load(string filename) {
    Parser parser;
    map<string,Wireframe> wireframes;

    ifstream file;
    file.open(filename);
    if (file.is_open()) {
        string line;
        while (getline(file,line)) {
            if(line!=""){
                vector<string> vec = parser.split(line,"|");

                // control point
                vector<Point> points;
                vector<string> strPoints = parser.split(vec[1]," ");
                for (int i=0; i<strPoints.size(); i++) {
                    vector<string> strXY = parser.split(strPoints[i].substr(1,strPoints[i].length()-2),",");
                    Point point(stoi(strXY[0]) * 10, stoi(strXY[1]) * 10);
                    points.push_back(point);
                }

                // inner point
                vector<string>strXY = parser.split(vec[2].substr(1,vec[2].length()-2),",");
                Point innerPoint(stoi(strXY[0]) * 10, stoi(strXY[1]) * 10);

                // borderColor
                vector<string> borderColorRGB = parser.split(vec[3].substr(1,vec[3].length()-2),",");
                Color borderColor(stoi(borderColorRGB[0]),stoi(borderColorRGB[1]),stoi(borderColorRGB[2]));

                // fillColor
                vector<string> fillColorRGB = parser.split(vec[4].substr(1,vec[4].length()-2),",");
                Color fillColor(stoi(fillColorRGB[0]),stoi(fillColorRGB[1]),stoi(fillColorRGB[2]));

                // priority
                int priority = stoi(vec[5]);
                
                Wireframe wireframe(points, innerPoint,borderColor,fillColor,priority);
                wireframes.insert(pair<string,Wireframe>(vec[0],wireframe));
            }
        }
    }

    return wireframes;
}

void PaintController::save(map<string, Wireframe> map, string filename) {
    ofstream file;
    file.open(filename);

    for (auto itr=map.begin(); itr!=map.end(); itr++){
        // label
        file << itr->first << "|";
        
        // control point
        vector<Point> points = itr->second.getPoints(); 
        for(int i=0; i<points.size();i++){
            file << "(" << points[i].getX()/10 << "," << points[i].getY()/10 << ")";
            if (i!=points.size()-1) file << " ";
        }
        file << "|";

        // inner point
        Point innerPoint = itr->second.getInnerPoint();
        file << "(" << innerPoint.getX()/10 << "," << innerPoint.getY()/10 << ")";
        file << "|";
        
        // border color
        Color borderColor = itr->second.getBorderColor();
        file << "(" << borderColor.getRed() << "," << borderColor.getGreen() << "," << borderColor.getBlue() << ")";
        file << "|";

        // fill color
        Color fillColor = itr->second.getFillColor();
        file << "(" << fillColor.getRed() << "," << fillColor.getGreen() << "," << fillColor.getBlue() << ")";
        file << "|";

        // priority
        file << itr->second.getPriority();
        
        file << endl;
    }
}

void PaintController::receiveCommand(string command) {
    if (command == "select") {
        
    }
}