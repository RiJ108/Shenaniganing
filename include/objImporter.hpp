#pragma once

#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <vector>
#include <sstream>
#include <typeinfo>
#include "mobject.hpp"
#include "displays.hpp"

using namespace std;
using namespace glm;

class ObjImporter {
public:
    ObjImporter();
    //Function that read a .Obj file with a given path and fill the Coords, Normals and TxtCoords vector (vector<double*>) and the Data vector (vector<float>)
    void extract(string path);
    //Function that extract the coordinates data from a "Coordinates" string provided by the .Obj file
    void getCoords(string line, double* coords);
    //Function that extract the normals data from a "Normal" string provided by the .Obj file
    void getNormals(string line, double* normals);
    //Function that extract the textures coordinates data from a "Texture coordinates" string provided by the .Obj file
    void getTxtCoords(string line, double* txtCoords);
    //Function that merge the coordinates, textures coordinates and normals from the Vertices, Normals and TxtCoords vector (vector<double*>) that constitute a face
    //and push them in the Data vector (vector<float>) according form the "Face" string provided by the .Obj file
    void loadFace(string line, int* order);
    void loadFace_OLD(string line, int* order);
    //Function that fill the MObject given instance with the previously loaded data form the .Obj file
    void loadMObject(MObject* aMObject);
    void loadMObject_OLD(MObject* aMObject);
    //Function that return the data vector (vector<float>) of the importer
    vector<float> getData();
    //Return as an unsigned int the size of the "Data" vector (vector<float>)
    unsigned int getDataSize();
    
    int faces = 0;
    bool debug = false;

private:
    vector<double*> Coords;
    vector<double*> Normals;
    vector<double*> TxtCoords;
    vector<float> Data;
};