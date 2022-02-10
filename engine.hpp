#pragma once

#include "displays.hpp"
#include "shader.hpp"
#include "perlinNoise.hpp"
#include "camera.hpp"
#include "terrain.hpp"

class Engine {
public:
	Engine() {
		vertices = (float*)malloc(sizeof(float) * 6 * (GRID_SUB_SIZE + 1) * (GRID_SUB_SIZE + 1));
		indices = (unsigned int*)malloc(sizeof(unsigned int) * 6 * GRID_SUB_SIZE * GRID_SUB_SIZE);
	}

	void generateVertices() {
		generateVertices(vertices);
	}

	void generateVertices(float *vertices) {
		cout << __FUNCTION__ << "->starting.." << endl;
		float x = 0.0f, z = 0.0f;
		unsigned int index = 0;
		for (unsigned int i = 0; i <= GRID_SUB_SIZE; i++) {
			x = (float)i / GRID_RESOLUTION - IN_CHUNK_VERTICES_OFFSET;
			for (unsigned int j = 0; j <= GRID_SUB_SIZE; j++){
				z = (float)j / GRID_RESOLUTION - IN_CHUNK_VERTICES_OFFSET;
				//**Coords
				vertices[index] = x;
				index++;
				vertices[index] = perlin.noise(x * 0.5f, 0.0f, z * 0.5f);
				index++;
				vertices[index] = z;
				index++;
				//**Norms
				vertices[index] = 0.0f;
				index++;
				vertices[index] = 1.0f;
				index++;
				vertices[index] = 0.0f;
				index++;
			}
		}
		cout << __FUNCTION__ << "->FINISHED !" << endl;
	}

	void generateIndices() {
		generateIndices(indices);
	}

	void generateIndices(unsigned int *indices) {
		cout << __FUNCTION__ << "->starting.." << endl;
		unsigned int index = 0, indexu = 0;
		for (unsigned int i = 0; i < GRID_SUB_SIZE; i++) {
			for (unsigned int j = 0; j < GRID_SUB_SIZE; j++) {
				index = i * (GRID_SUB_SIZE + 1) + j;
				//**First triangle
				indices[indexu] = index;
				indexu++;
				indices[indexu] = index + 1;
				indexu++;
				indices[indexu] = index + (GRID_SUB_SIZE + 1);
				indexu++;
				//**Second triangle
				indices[indexu] = index + 1;
				indexu++;
				indices[indexu] = index + (GRID_SUB_SIZE + 1) + 1;
				indexu++;
				indices[indexu] = index + (GRID_SUB_SIZE + 1);
				indexu++;
			}
		}
		cout << __FUNCTION__ << "->FINISHED !" << endl;
	}

	void setBuffers() {
		cout << __FUNCTION__ << "->starting.." << endl;
		shader = Shader("resources/shaders/vShaderSource3D_OLD.vs", "resources/shaders/fShaderSource3D_OLD.fs");

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * (GRID_SUB_SIZE + 1) * (GRID_SUB_SIZE + 1), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6 * GRID_SUB_SIZE * GRID_SUB_SIZE, indices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

		cout << __FUNCTION__ << "->FINISHED !" << endl;
	}

	void render(Camera pov, float ratio) {
		render(VAO, pov, ratio);
	}

	void render(GLuint aVAO, Camera pov, float ratio) {
		shader.use();
		shader.setVec3("objectColor", vec3(1.0f, 1.0f, 1.0f));
		shader.setVec3("lightColor", vec3(1.0f, 1.0f, 0.9f));
		shader.setVec3("lightPos", vec3(-25.0f, 50.0f, -25.0f));
		shader.setMat4("view", pov.lookAt());
		shader.setVec3("viewPos", pov.getPosition());
		shader.setVec3("frontView", pov.getFront());
		shader.setMat4("projection", perspective(radians(pov.getFOV()), ratio, 0.1f, 10000.0f));
		shader.setMat4("model", mat4(1.0f));

		glBindVertexArray(aVAO);
		glDrawElements(GL_TRIANGLES, 6 * (GRID_SUB_SIZE) * (GRID_SUB_SIZE), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	GLuint VAO, VBO, EBO;
	Terrain terrain;

private:
	int seed = 0;
	PerlinNoise perlin;
	//float vertices_[6 * (GRID_SUB_SIZE + 1) * (GRID_SUB_SIZE + 1)];
	float* vertices;
	//unsigned int indices_[6 * (GRID_SUB_SIZE) * (GRID_SUB_SIZE)];
	unsigned int* indices;
	Shader shader;
};