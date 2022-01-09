#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <vector>

using namespace std;
using namespace glm;

using namespace std;
using namespace glm;

class MObject {
public:
	//Return as a (unsigned int) the number of faces of the object/the value of the "nbrFaces" parameter
	unsigned int getNbrFaces();
	//Return as a (unsigned int) the size of the "data" vector
	unsigned int getDataSize();
	//Return as a (vector<float>) the "data" vector
	vector<float> getData();
	//Return as a (*vector<float>) the pointer of the "data" vector
	vector<float>* getDataPtr();
	//Generate, bind, set-up and fill the vertex array object (GLuint) and the vertex buffer object (GLuint) with the "data" vector (vector<float>)
	void initAndFillBuffers();

	//Return as a (GLuint) the value of the vertex array object
	GLuint getVAO();
	//Return as a (GLuint) the value of the vertex buffer object
	GLuint getVBO();

	vector<float> data;
	unsigned int nbrFaces = 0;

private:
	GLuint VAO, VBO;
};