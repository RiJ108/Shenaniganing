#include "mobject.hpp"

void MObject::initAndFillBuffers() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data.size(), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * data.size(), &data[0]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);
}

GLuint MObject::getVAO() {
	return VAO;
}

GLuint MObject::getVBO() {
	return VBO;
}

unsigned int MObject::getNbrFaces() {
	return nbrFaces;
}

vector<float> MObject::getData() {
	return data;
}

vector<float>* MObject::getDataPtr() {
	return &data;
}

unsigned int MObject::getDataSize() {
	return data.size();
}