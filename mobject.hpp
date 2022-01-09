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
	unsigned int getNbrFaces();
	unsigned int getDataSize();
	vector<float> getData();
	vector<float>* getDataPtr();
	void initAndFillBuffers();

	GLuint getVAO();
	GLuint getVBO();

	vector<float> data;
	unsigned int nbrFaces = 0;

private:
	GLuint VAO, VBO;
};