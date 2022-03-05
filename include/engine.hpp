#pragma once

#include "displays.hpp"
#include "shader.hpp"
#include "perlinNoise.hpp"
#include "camera.hpp"
#include "terrain.hpp"
#include "marchingCube.hpp"
#include "constant.hpp"

#define LENGTH 2
#define WIDTH 2
#define HEIGHT 2
#define GRID_RES 1

typedef struct myPoint {
	vec3 aPos = vec3(0.0f);
	float value = 0.0f;
}myPoint;

class Engine {
public:
	Engine() {
		vertices = (float*)malloc(sizeof(float) * 6 * (GRID_SUB_SIZE + 1) * (GRID_SUB_SIZE + 1));
		indices = (unsigned int*)malloc(sizeof(unsigned int) * 6 * GRID_SUB_SIZE * GRID_SUB_SIZE);
		points = (float*)malloc(sizeof(float) * LENGTH * WIDTH * HEIGHT * 6);
		mPoints = (myPoint***)malloc(sizeof_3);
		if (mPoints) {
			for (unsigned int i = 0; i < LENGTH * GRID_RES; i++) {
				mPoints[i] = (myPoint**)malloc(sizeof_2);
				if (mPoints[i]) {
					for (unsigned int j = 0; j < HEIGHT * GRID_RES; j++)
						mPoints[i][j] = (myPoint*)malloc(sizeof_1);
				}
			}
		}
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

	void initShaders() {
		pointShader = Shader("resources/shaders/vShaderSourcePoint.vs", "resources/shaders/fShaderSourcePoint.fs");
	}

	void fillPoints() {
		float x_offset = LENGTH / 2.0f;
		float y_offset = HEIGHT / 2.0f;
		float z_offset = WIDTH / 2.0f;

		vec3 position = vec3(0.0f);

		for (unsigned int i = 0; i < LENGTH * GRID_RES; i++) {
			for (unsigned int j = 0; j < HEIGHT * GRID_RES; j++) {
				for (unsigned int k = 0; k < WIDTH * GRID_RES; k++) {
					position.x = i * (1.0 / GRID_RES) - x_offset;
					position.y = j * (1.0 / GRID_RES) - y_offset;
					position.z = k * (1.0 / GRID_RES) - z_offset;

					mPoints[i][j][k].aPos = position;
					mPoints[i][j][k].value = perlin.noise(0.15f * position.x, 0.15f * position.y, 0.15f * position.z);

					if (mPoints[i][j][k].value < pointValue_min)
						pointValue_min = mPoints[i][j][k].value;
					if (mPoints[i][j][k].value > pointValue_max)
						pointValue_max = mPoints[i][j][k].value;

				}
			}
		}
	}

	void normalizePointsValue() {
		Display::disp("Min value = ");
		cout << pointValue_min << endl;
		Display::disp("Max value = ");
		cout << pointValue_max << endl;

		for (int i = 0; i < LENGTH * GRID_RES; i++) {
			for (int j = 0; j < HEIGHT * GRID_RES; j++) {
				for (int k = 0; k < WIDTH * GRID_RES; k++)
					mPoints[i][j][k].value = (mPoints[i][j][k].value - pointValue_min)/(pointValue_max - pointValue_min);
			}
		}
	}

	void thresholdingPointsToVector() {
		for (int i = 0; i < LENGTH * GRID_RES; i++) {
			for (int j = 0; j < HEIGHT * GRID_RES; j++) {
				for (int k = 0; k < WIDTH * GRID_RES; k++) {
					if (mPoints[i][j][k].value < threshold) {
						vecPoints.push_back(mPoints[i][j][k].aPos.x);
						vecPoints.push_back(mPoints[i][j][k].aPos.y);
						vecPoints.push_back(mPoints[i][j][k].aPos.z);

						vecPoints.push_back(0.0f);
						vecPoints.push_back(mPoints[i][j][k].value);
						vecPoints.push_back(0.0f);
					}
				}
			}
		}
	}

	void mergeToVector() {
		for (int i = 0; i < LENGTH * GRID_RES; i++) {
			for (int j = 0; j < HEIGHT * GRID_RES; j++) {
				for (int k = 0; k < WIDTH * GRID_RES; k++) {
					vecPoints.push_back(mPoints[i][j][k].aPos.x);
					vecPoints.push_back(mPoints[i][j][k].aPos.y);
					vecPoints.push_back(mPoints[i][j][k].aPos.z);

					vecPoints.push_back(0.0f);
					vecPoints.push_back(mPoints[i][j][k].value);
					vecPoints.push_back(0.0f);
				}
			}
		}
	}

	void GFSBuffersVectorPoints() {
		glGenVertexArrays(1, &VAO_points);
		glGenBuffers(1, &VBO_points);

		glBindVertexArray(VAO_points);

		glBindBuffer(GL_ARRAY_BUFFER, VBO_points);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vecPoints.size(), &vecPoints[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

		cout << __FUNCTION__ << "->FINISHED !" << endl;
	}

	void renderPoints(Camera pov, float ratio) {
		pointShader.use();

		pointShader.setMat4("view", pov.lookAt());
		pointShader.setMat4("projection", perspective(radians(pov.getFOV()), ratio, 0.1f, 10000.0f));
		pointShader.setMat4("model", mat4(1.0f));

		glBindVertexArray(VAO_points);
		glDrawArrays(GL_POINTS, 0, vecPoints.size());
		glBindVertexArray(0);
	}

	void renderMesh(Camera pov, float ratio) {
		pointShader.use();

		pointShader.setMat4("view", pov.lookAt());
		pointShader.setMat4("projection", perspective(radians(pov.getFOV()), ratio, 0.1f, 10000.0f));
		pointShader.setMat4("model", mat4(1.0f));

		glBindVertexArray(VAO_mesh);
		glDrawArrays(GL_TRIANGLES, 0, testMeshData.size() / 6);
		glBindVertexArray(0);
	}

	void refreshMCdebug(GRIDCELL *aGrid, vector<float> *vertices, vector<float> *points, vector<TRIANGLE> *triangles,  int cubeIndex) {
		for (int i = 7; i >= 0; i--) {
			if ((cubeIndex & (int)pow(2, i)) == (int)pow(2, i))
				aGrid->val[i] = 1.0f;
			else
				aGrid->val[i] = 0.0f;
		}
		
		triangles->clear();
		vertices->clear();
		points->clear();

		nbrTriangles = mCube.polygonise(*aGrid, 0.5f, triangles);
		for (int i = 0; i < nbrTriangles; i++) {
			for (int j = 0; j < 3; j++) {
				vertices->push_back(triangles->at(i).p[j].x);
				vertices->push_back(triangles->at(i).p[j].y);
				vertices->push_back(triangles->at(i).p[j].z);

				vertices->push_back(1.0f);
				vertices->push_back(1.0f);
				vertices->push_back(0.0f);
			}
		}
		mCube.loadGridcellPointToVector(*aGrid, points);
	}

	void marchThrough() {
		GRIDCELL gridcell;
		int index = 0;
		testTriangles = *(new vector<TRIANGLE>);
		for (unsigned int i = 0; i < (LENGTH * GRID_RES) - 1; i++) {
			for (unsigned int j = 0; j < (HEIGHT * GRID_RES) - 1; j++) {
				for (unsigned int k = 0; k < (WIDTH * GRID_RES) - 1; k++) {
					index = 0;
					for (unsigned int l = 0; l < 2; l++) {
						for (unsigned int m = 0; m < 2; m++) {
							for (unsigned int n = 0; n < 2; n++) {
								gridcell.p[index].x = mPoints[i + l][j + m][k + n].aPos.x;
								gridcell.p[index].y = mPoints[i + l][j + m][k + n].aPos.y;
								gridcell.p[index].z = mPoints[i + l][j + m][k + n].aPos.z;
								gridcell.val[index] = mPoints[i + l][j + m][k + n].value;
								index++;
							}
						}
					}
					nbrTriangles = mCube.polygonise(gridcell, 0.5f, &testTriangles);
				}
			}
		}
		cout << __FUNCTION__ << "->number of triangles =" << nbrTriangles << endl;
	}

	void setMesh() {
		for (unsigned int i = 0; i < testTriangles.size(); i++) {
			for (int j = 0; j < 3; j++) {
				testMeshData.push_back(testTriangles.at(i).p[j].x);
				testMeshData.push_back(testTriangles.at(i).p[j].y);
				testMeshData.push_back(testTriangles.at(i).p[j].z);

				testMeshData.push_back(1.0f);
				testMeshData.push_back(1.0f);
				testMeshData.push_back(0.0f);
			}
		}
	}

	void GFSBuffersMesh() {
		glGenVertexArrays(1, &VAO_mesh);
		glGenBuffers(1, &VBO_mesh);

		glBindVertexArray(VAO_mesh);

		glBindBuffer(GL_ARRAY_BUFFER, VBO_mesh);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * testMeshData.size(), &testMeshData[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

		cout << __FUNCTION__ << "->FINISHED !" << endl;
	}

	~Engine() {
		if(mPoints)
			free(mPoints);
	}

	GLuint VAO, VBO, EBO;
	GLuint VAO_points, VBO_points;
	GLuint VAO_mesh, VBO_mesh;
	Terrain terrain;

private:
	int seed = 0;
	PerlinNoise perlin;
	MarchingCube mCube;
	float threshold = 0.50f;
	vector<TRIANGLE> testTriangles;
	vector<float> testMeshData;
	int nbrTriangles;

	//** Single layer heigth map section
	Shader shader;
	float* vertices;
	unsigned int* indices;

	//** Marching cube testing section
	Shader pointShader;
	float* points;
	int sizeof_1 = sizeof(myPoint) * (WIDTH * GRID_RES);
	int sizeof_2 = sizeof_1 * (HEIGHT * GRID_RES);
	int sizeof_3 = sizeof_2 * (LENGTH * GRID_RES);
	myPoint*** mPoints;
	vector<float> vecPoints;
	float pointValue_min = HUGE_VALF;
	float pointValue_max = 0.0f;

	//**Marching cube

};