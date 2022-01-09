#include "objImporter.hpp"

ObjImporter::ObjImporter(){
    //cout << __FUNCTION__ << endl;
}

void ObjImporter::loadMObject(MObject* aMObject) {
    if (Data.size() == 0) {
        cout << __FUNCTION__ << "-> ###!! data empty, cannot load on the MObject. Exiting the function. !!###" << endl;
        return;
    }
    aMObject->data = Data;
    aMObject->nbrFaces = faces;
}

void ObjImporter::extract(string path){
    cout << __FUNCTION__;
    fstream file;
    file.open(path);
    if(file.is_open()) cout << "->File "+path+" openned." << endl;
    else cout << "->###!! File "+path+" failed to open. !!###" << endl;

    string line;
    char tmp;
    string name;
    stringstream ss;

    double *coords, *norms, *txtCoords;
    int *order;

    //**Main loop which get each line of the obj file
    while(getline(file, line)){
        //**Test to determine what the current line is coding for
        switch(line.at(0)){
            case 'o':
                ss << line;
                ss >> tmp >> name;
                //cout << "Object: "+name << endl;
                break;

            case 'v':
                switch(line.at(1)){
                    case ' ':
                        coords = (double*)malloc(sizeof(double)*3);
                        getCoords(line, coords);
                        Coords.push_back(coords);
                        break;

                    case 't':
                        txtCoords = (double*)malloc(sizeof(double)*2);
                        getTxtCoords(line, txtCoords);
                        TxtCoords.push_back(txtCoords);
                        break;
                    
                    case 'n':
                        norms = (double*)malloc(sizeof(double)*3);
                        getNormals(line, norms);
                        Normals.push_back(norms);
                        break;
                }
                break;

            case 'f':
                order = (int*)malloc(sizeof(int)*3);
                loadFace(line, order);
                break;
        }
    }

    //**Simple debug prompt of the Data vector
    if (debug) {
        cout << "Data size = " << Data.size() << endl;
        int index = 0;
        for (int i = 0; i < Data.size(); i++) {
            cout << Data[i] << " ";
            if (index == 2)
                cout << "\t";
            if (index == 5) {
                cout << endl;
                index = -1;
            }
            index++;
        }
    }
    file.close();
    cout << __FUNCTION__ << "->Extraction finished." << endl;
}

void ObjImporter::getCoords(string line, double* coords){
    //Cheap and dirty way of retriving the double for the coordinates form a string
    char c;
    string tmp;
    int index = 2;
    bool flag = false;
    while(index < line.length()){
        c = line.at(index);
        if(c != ' ')
            tmp+=c;
        else{
            if(!flag){
                coords[0] = stod(tmp);
                flag = true;
            }else coords[1] = stod(tmp);
            tmp = "";
        }
        index++;
    }
    coords[2] = stod(tmp);
    //cout << "Vertices x: " << vertices[0] << " y: " << vertices[1] << " z: " << vertices[2] << endl;
}

void ObjImporter::getNormals(string line, double* normals){
    //Cheap and dirty way of retriving the double for the normals form a string
    char c;
    string tmp;
    int index = 3;
    bool flag = false;
    while(index < line.length()){
        c = line.at(index);
        if(c != ' ')
            tmp+=c;
        else{
            if(!flag){
                normals[0] = stod(tmp);
                flag = true;
            }else normals[1] = stod(tmp);
            tmp = "";
        }
        index++;
    }
    normals[2] = stod(tmp);
    //cout << "Normals x: " << normals[0] << " y: " << normals[1] << " z: " << normals[2] << endl;
}

void ObjImporter::getTxtCoords(string line, double* txtCoords){
    char c;
    string tmp;
    int index = 3;
    bool flag = false;
    while(index < line.length()){
        c = line.at(index);
        if(c != ' ')
            tmp+=c;
        else{
            txtCoords[0] = stod(tmp);
            tmp = "";
        }
        index++;
    }
    txtCoords[1] = stod(tmp);
    //cout << "TxtCoords x: " << txtCoords[0] << " y: " << txtCoords[1] << endl;
}

void ObjImporter::loadFace(string line, int* order){
    stringstream ss;
    ss << line;
    int x0, y0, z0, x1, y1, z1, x2, y2, z2, i;
    string tmp;
    char c;

    faces++;

    //**Exctraction of the indices for coordinates, textures coordinates
    //**and normals for the three vertices of the triangle
    ss >> tmp >> x0 >> c >> y0 >> c >> z0 >> x1 >> c >> y1 >> c >> z1 >> x2 >> c >> y2 >> c >> z2;

    //**Manual calculation of the normals
    vec3 v0 = vec3(Coords.at(x0-1)[0], Coords.at(x0-1)[1], Coords.at(x0-1)[2]);
    vec3 v1 = vec3(Coords.at(x1-1)[0], Coords.at(x1-1)[1], Coords.at(x1-1)[2]);
    vec3 v2 = vec3(Coords.at(x2-1)[0], Coords.at(x2-1)[1], Coords.at(x2-1)[2]);

    vec3 norm = normalize(cross(v1 - v0, v2 - v1));
    
    //**Storing of datas for the first vertex
    //**TxtCoords temporaly ignored (indexted by y)
    for(i = 0 ; i < 3 ; i++)//Coordinates loop
        Data.push_back(Coords.at(x0-1)[i]);
    for(i = 0 ; i < 3 ; i++)//Normals loop
        //Data.push_back(Normals.at(z0-1)[i]);
        Data.push_back(norm[i]);

    //**Storing of datas for the second vertex
    for(i = 0 ; i < 3 ; i++)//Coordinates loop
        Data.push_back(Coords.at(x1-1)[i]);
    for(i = 0 ; i < 3 ; i++)//Normals loop
        //Data.push_back(Normals.at(z1-1)[i]);
        Data.push_back(norm[i]);

    //**Storing of datas for the third vertex
    for(i = 0 ; i < 3 ; i++)//Coordinates loop
        Data.push_back(Coords.at(x2-1)[i]);
    for(i = 0 ; i < 3 ; i++)//Normals loop
        //Data.push_back(Normals.at(z2-1)[i]);
        Data.push_back(norm[i]);


    /*vec3 v0 = vec3(Vertices.at(x0-1)[0], Vertices.at(x0-1)[1], Vertices.at(x0-1)[2]);
    vec3 v1 = vec3(Vertices.at(x1-1)[0], Vertices.at(x1-1)[1], Vertices.at(x1-1)[2]);
    vec3 v2 = vec3(Vertices.at(x2-1)[0], Vertices.at(x2-1)[1], Vertices.at(x2-1)[2]);

    vec3 norm = normalize(cross(v1 - v0, v2 - v1));*/

    /*cout << "v0=" << v0.x << " " << v0.y << " " << v0.z<< endl;
    cout << "v1=" << v1.x << " " << v1.y << " " << v1.z<< endl;
    cout << "v2=" << v2.x << " " << v2.y << " " << v2.z<< endl;*/
    if(debug)
        cout << "norm=" << norm.x << " " << norm.y << " " << norm.z<< endl;
}

vector<float> ObjImporter::getData(){
    return Data;
}

int ObjImporter::getDataSize(){
    return Data.size();
}