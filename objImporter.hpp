#pragma once OBJIMPORTER

#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <vector>
#include <sstream>
#include <typeinfo>
#include "mobject.hpp"
//#include <filesystem>

using namespace std;
using namespace glm;
//namespace fs = std::filesystem;

class ObjImporter{
public:
    ObjImporter();
    void extract(string path);
    void getVertices(string line, double* vertices);
    void getNormals(string line, double* normals);
    void getTxtCoords(string line, double* txtCoords);
    void getOrder(string line, int* order);
    void loadMObject(MObject *aMObject);
    vector<float> getData();
    int getDataSize();
    int faces = 0;
    bool debug = false;

private:
    vector<double*> Vertices;
    vector<double*> Normals; 
    vector<double*> TxtCoords;
    vector<float> Data;
};