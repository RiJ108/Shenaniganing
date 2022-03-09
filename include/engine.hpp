#pragma once

#include "displays.hpp"
#include "shader.hpp"
#include "perlinNoise.hpp"
#include "camera.hpp"
#include "terrain.hpp"
#include "marchingCube.hpp"
#include "constant.hpp"

#define LENGTH 10
#define WIDTH 10
#define HEIGHT 10
#define GRID_RES 10
#define CUBE_LIMIT ((LENGTH * GRID_RES) - 1) * ((HEIGHT * GRID_RES) - 1) * ((WIDTH * GRID_RES) - 1)

class Engine {
public:
	Engine() {
		worldPoints = (MPOINT***)malloc(sizeof_3);
		if (worldPoints) {
			for (unsigned int i = 0; i < LENGTH * GRID_RES; i++) {
				worldPoints[i] = (MPOINT**)malloc(sizeof_2);
				if (worldPoints[i]) {
					for (unsigned int j = 0; j < HEIGHT * GRID_RES; j++)
						worldPoints[i][j] = (MPOINT*)malloc(sizeof_1);
				}
			}
		}
	}

	~Engine() {
		free(pointPtr);
		free(gridcellPtr);
	}

	//______________________________________________________________________COMPUTATION

	void generateWorldPoints() {
		float x_offset = LENGTH / 2.0f;
		float y_offset = HEIGHT / 2.0f;
		float z_offset = WIDTH / 2.0f;

		vec3 position = vec3(0.0f);
		float noiseValue = 0.0f;

		int index = 0;

		for (unsigned int i = 0; i < LENGTH * GRID_RES; i++) {
			for (unsigned int j = 0; j < HEIGHT * GRID_RES; j++) {
				for (unsigned int k = 0; k < WIDTH * GRID_RES; k++) {
					position.x = i * (1.0 / GRID_RES) - x_offset;
					position.y = j * (1.0 / GRID_RES) - y_offset;
					position.z = k * (1.0 / GRID_RES) - z_offset;

					noiseValue = perlin.noise(0.15f * position.x, 0.15f * position.y, 0.15f * position.z);
					worldPoints[i][j][k].coord = position;
					worldPoints[i][j][k].norm = vec3(0.0f);
					worldPoints[i][j][k].val = noiseValue;

					if (noiseValue < worldPointsValue_min)
						worldPointsValue_min = noiseValue;
					if (noiseValue > worldPointsValue_max)
						worldPointsValue_max = noiseValue;
				}
			}
		}
	}

	void generateWorldGridcells() {
		for (unsigned int x = 0; x < (LENGTH * GRID_RES) - 1; x++) {
			for (unsigned int y = 0; y < (HEIGHT * GRID_RES) - 1; y++) {
				for (unsigned int z = 0; z < (WIDTH * GRID_RES) - 1; z++) {
					gridcellPtr->points[0] = &worldPoints[x][y][z];
					gridcellPtr->points[1] = &worldPoints[x+1][y][z];
					gridcellPtr->points[2] = &worldPoints[x+1][y][z+1];
					gridcellPtr->points[3] = &worldPoints[x][y][z+1];
					gridcellPtr->points[4] = &worldPoints[x][y+1][z];
					gridcellPtr->points[5] = &worldPoints[x+1][y+1][z];
					gridcellPtr->points[6] = &worldPoints[x+1][y+1][z+1];
					gridcellPtr->points[7] = &worldPoints[x][y+1][z+1];

					worldGridcells.push_back(*gridcellPtr);
					mCube.polygonise(*gridcellPtr, 0.5f, &meshTriangles);
				}
			}
		}
	}

	void setMesh() {
		for (unsigned int i = 0; i < meshTriangles.size(); i++) {
			for (int j = 0; j < 3; j++) {
				meshData.push_back(meshTriangles.at(i).points[j].x);
				meshData.push_back(meshTriangles.at(i).points[j].y);
				meshData.push_back(meshTriangles.at(i).points[j].z);

				meshData.push_back(meshTriangles.at(i).norm.x);
				meshData.push_back(meshTriangles.at(i).norm.y);
				meshData.push_back(meshTriangles.at(i).norm.z);
			}
		}
		cout << __FUNCTION__ << "->" << meshData.size() / 6 << " Triangles in the mesh\n";
	}

	//______________________________________________________________________BUFFERS/SHADERS SETTING

	void initShaders() {
		pointShader = Shader("resources/shaders/vShaderSourcePoint.glsl", "resources/shaders/fShaderSourcePoint.glsl");
		meshShader = Shader("resources/shaders/vShaderSource3D_OLD.glsl", "resources/shaders/fShaderSource3D_OLD.glsl");
	}

	void GFSBuffersMesh() {
		glGenVertexArrays(1, &VAO_mesh);
		glGenBuffers(1, &VBO_mesh);

		glBindVertexArray(VAO_mesh);

		glBindBuffer(GL_ARRAY_BUFFER, VBO_mesh);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * meshData.size(), &meshData[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

		cout << __FUNCTION__ << "->FINISHED !" << endl;
	}

	//______________________________________________________________________RENDERS

	void renderMesh(Camera pov, float ratio) {
		meshShader.use();
		meshShader.setVec3("objectColor", vec3(1.0f, 1.0f, 1.0f));
		meshShader.setVec3("lightColor", vec3(1.0f, 1.0f, 0.9f));
		meshShader.setVec3("lightPos", vec3(-25.0f, -50.0f, -25.0f));
		meshShader.setMat4("view", pov.lookAt());
		meshShader.setMat4("projection", perspective(radians(pov.getFOV()), ratio, 0.1f, 10000.0f));
		meshShader.setMat4("model", mat4(1.0f));

		glBindVertexArray(VAO_mesh);
		glDrawArrays(GL_TRIANGLES, 0, meshData.size() / 6);
		glBindVertexArray(0);
	}

	void renderGCPoints(Camera pov, float ratio) {
		pointShader.use();

		pointShader.setMat4("view", pov.lookAt());
		pointShader.setMat4("projection", perspective(radians(pov.getFOV()), ratio, 0.1f, 10000.0f));
		pointShader.setMat4("model", mat4(1.0f));

		glBindVertexArray(VAO_gcp);
		glDrawArrays(GL_POINTS, 0, gcp.size());
		glBindVertexArray(0);
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

	GLuint VAO_gcp, VBO_gcp;
	vector<float> gcp;
	Terrain terrain;
	unsigned int step = 0, lastOffset = 0, meshSize = 0, cubeIndex = 0;
	int nbrTriangles;

private:
	//__OPT           !!VALIDE!!
	MPOINT*** worldPoints;
	int sizeof_1 = sizeof(MPOINT) * (WIDTH * GRID_RES);
	int sizeof_2 = sizeof_1 * (HEIGHT * GRID_RES);
	int sizeof_3 = sizeof_2 * (LENGTH * GRID_RES);
	vector<GRIDCELL> worldGridcells;
	vector<TRIANGLE> meshTriangles;
	vector<float> meshData;
	float worldPointsValue_min = HUGE_VALF;
	float worldPointsValue_max = 0.0f;
	MPOINT* pointPtr = new MPOINT;
	GRIDCELL* gridcellPtr = new GRIDCELL;
	GLuint VAO_mesh, VBO_mesh;
	float threshold = 0.50f;
	
	//__OLD
	Shader meshShader;
	int seed = 0;
	PerlinNoise perlin;
	MarchingCube mCube;
	
	//** Single layer heigth map section
	Shader shader;

	//** Marching cube testing section
	Shader pointShader;
};