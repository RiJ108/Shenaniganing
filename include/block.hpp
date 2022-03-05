#pragma once

#include <map>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_angle.hpp>

using namespace std;
using namespace glm;

class Block {
public:
	void setIndex(unsigned int aIndex) {
		index = aIndex;
	}

	void setData(vec3 aPosition) {
		position = aPosition;
		for (unsigned int i = 0; i < dataSize; i+=0) {
			for (unsigned int j = 0; j < 3; j++) {
				data[i] = CUBE_REFS[i] + aPosition[j];
				i++;
			}
			for (unsigned int j = 0; j < 6; j++) {
				data[i] = CUBE_REFS[i];
				i++;
			}
		}
	}

	void setData() {
		for (unsigned int i = 0; i < dataSize; i++)
			data[i] = CUBE_REFS[i];
	};

	void gfsBuffers() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * dataSize, NULL, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * dataSize, &data[0]);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(8 * sizeof(float)));
		glBindVertexArray(0);
	};

	GLuint getVAO() {
		return VAO;
	};

	unsigned int getNbrVertices() {
		return nbrVertices;
	};

	~Block() {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}

private:
	GLuint VAO, VBO;
	vec3 position = vec3(0.0f);
	unsigned int index = 0;
	unsigned int dataIndex = 0;
	unsigned int dataSize = 324;
	unsigned int nbrFaces = 12;
	unsigned int nbrVertices = 36;
	unsigned int sizeOfData = 324 * sizeof(float);
	float data[324];
	float CUBE_REFS[324] = {
	-0.5, 0.5, -0.5,        -0, 1, 0,       0.375, 0.75,    0,
	0.5, 0.5, 0.5,          -0, 1, 0,       0.625, 0.5,     0,
	0.5, 0.5, -0.5,         -0, 1, 0,       0.875, 0.5,     0,
	0.5, 0.5, 0.5,          -0, 0, 1,       0.625, 0.5,     0,
	-0.5, -0.5, 0.5,        -0, 0, 1,       0.375, 0,       0,
	0.5, -0.5, 0.5,         -0, 0, 1,       0.375, 1,       0,
	-0.5, 0.5, 0.5,         -1, -0, 0,      0.375, 0.25,    0,
	-0.5, -0.5, -0.5,       -1, -0, 0,      0.625, 0,       0,
	-0.5, -0.5, 0.5,        -1, -0, 0,      0.375, 0,       0,
	0.5, -0.5, -0.5,        -0, -1, -0,     0.625, 0.75,    0,
	-0.5, -0.5, 0.5,        -0, -1, -0,     0.375, 0,       0,
	-0.5, -0.5, -0.5,       -0, -1, -0,     0.625, 0,       0,
	0.5, 0.5, -0.5,         1, 0, 0,        0.875, 0.5,     0,
	0.5, -0.5, 0.5,         1, 0, 0,        0.375, 1,       0,
	0.5, -0.5, -0.5,        1, 0, 0,        0.625, 0.75,    0,
	-0.5, 0.5, -0.5,        -0, -0, -1,     0.375, 0.75,    0,
	0.5, -0.5, -0.5,        -0, -0, -1,     0.625, 0.75,    0,
	-0.5, -0.5, -0.5,       -0, -0, -1,     0.625, 0,       0,
	-0.5, 0.5, -0.5,        0, 1, 0,        0.375, 0.75,    0,
	-0.5, 0.5, 0.5,         0, 1, 0,        0.375, 0.25,    0,
	0.5, 0.5, 0.5,          0, 1, 0,        0.625, 0.5,     0,
	0.5, 0.5, 0.5,          0, 0, 1,        0.625, 0.5,     0,
	-0.5, 0.5, 0.5,         0, 0, 1,        0.375, 0.25,    0,
	-0.5, -0.5, 0.5,        0, 0, 1,        0.375, 0,       0,
	-0.5, 0.5, 0.5,         -1, -0, -0,     0.375, 0.25,    0,
	-0.5, 0.5, -0.5,        -1, -0, -0,     0.375, 0.75,    0,
	-0.5, -0.5, -0.5,       -1, -0, -0,     0.625, 0,       0,
	0.5, -0.5, -0.5,        0, -1, 0,       0.625, 0.75,    0,
	0.5, -0.5, 0.5,         0, -1, 0,       0.375, 1,       0,
	-0.5, -0.5, 0.5,        0, -1, 0,       0.375, 0,       0,
	0.5, 0.5, -0.5,         1, 0, -0,       0.875, 0.5,     0,
	0.5, 0.5, 0.5,          1, 0, -0,       0.625, 0.5,     0,
	0.5, -0.5, 0.5,         1, 0, -0,       0.375, 1,       0,
	-0.5, 0.5, -0.5,        0, 0, -1,       0.375, 0.75,    0,
	0.5, 0.5, -0.5,         0, 0, -1,       0.875, 0.5,     0,
	0.5, -0.5, -0.5,        0, 0, -1,       0.625, 0.75,    0 };
};