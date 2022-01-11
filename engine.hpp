#pragma once

#include "displays.hpp"
#include "shader.hpp"
#include "perlinNoise.hpp"
#include "camera.hpp"

class Engine {
public:
	void generateVertices() {
		cout << __FUNCTION__ << "->starting.." << endl;
		float x = 0.0f, z = 0.0f;
		unsigned int index = 0;
		vec3 norm = vec3(0.0f, 1.0f, 0.0f);
		for (unsigned int i = 0; i <= chunkSize*terrainResolution; i++) {
			x = (float)i / terrainResolution - inChunkVerticeOffset;
			for (unsigned int j = 0; j <= chunkSize*terrainResolution; j++){
				z = (float)j / terrainResolution - inChunkVerticeOffset;			
				//**Coords
				vertices_[index] = x;
				index++;
				vertices_[index] = perlin.noise(x * 0.5f, 0.0f, z * 0.5f);
				index++;
				vertices_[index] = z;
				index++;
				//**Norms
				vertices_[index] = 0.0f;
				index++;
				vertices_[index] = 1.0f;
				index++;
				vertices_[index] = 0.0f;
				index++;
			}
		}
		cout << __FUNCTION__ << "->FINISHED !" << endl;
	}

	void generateIndices() {
		cout << __FUNCTION__ << "->starting.." << endl;
		unsigned int index = 0, indexu = 0;
		for (unsigned int i = 0; i < chunkSize * terrainResolution; i++) {
			for (unsigned int j = 0; j < chunkSize * terrainResolution; j++) {
				index = i * (chunkSize * terrainResolution + 1) + j;
				//**First triangle
				indices_[indexu] = index;
				indexu++;
				indices_[indexu] = index + 1;
				indexu++;
				indices_[indexu] = index + (chunkSize * terrainResolution + 1);
				indexu++;
				//**Second triangle
				indices_[indexu] = index + 1;
				indexu++;
				indices_[indexu] = index + (chunkSize * terrainResolution + 1) + 1;
				indexu++;
				indices_[indexu] = index + (chunkSize * terrainResolution + 1);
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
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_), vertices_, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_), indices_, GL_STATIC_DRAW);
		
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		
		cout << __FUNCTION__ << "->FINISHED !" << endl;
	}

	void render(Camera pov, float ratio) {
		shader.use();
		shader.setVec3("objectColor", vec3(1.0f, 1.0f, 1.0f));
		shader.setVec3("lightColor", vec3(1.0f, 1.0f, 0.9f));
		shader.setVec3("lightPos", vec3(-25.0f, 50.0f, -25.0f));
		shader.setMat4("view", pov.lookAt());
		shader.setVec3("viewPos", pov.getPosition());
		shader.setVec3("frontView", pov.getFront());
		shader.setMat4("projection", perspective(radians(pov.getFOV()), ratio, 0.1f, 10000.0f));
		shader.setMat4("model", mat4(1.0f));

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6 * (chunkSize * terrainResolution) * (chunkSize * terrainResolution), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

private:
	int seed = 0;
	PerlinNoise perlin;
	static const int terrainResolution = 8;
	static const int chunkSize = 10;
	float inChunkVerticeOffset = chunkSize / 2.0f;
	float vertices_[6 * (chunkSize * terrainResolution + 1) * (chunkSize * terrainResolution + 1)];
	unsigned int indices_[6 * (chunkSize * terrainResolution) * (chunkSize * terrainResolution)];
	GLuint VAO, VBO, EBO;
	Shader shader;
};