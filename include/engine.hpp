#pragma once

#include "displays.hpp"
#include "shader.hpp"
#include "perlinNoise.hpp"
#include "camera.hpp"
#include "terrain.hpp"
#include "marchingCube.hpp"
#include "constant.hpp"

#define LENGTH 4
#define WIDTH 4
#define HEIGHT 5
#define GRID_RES 3
#define CUBE_LIMIT ((LENGTH * GRID_RES) - 1) * ((HEIGHT * GRID_RES) - 1) * ((WIDTH * GRID_RES) - 1)

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

	~Engine() {
		if (mPoints)
			free(mPoints);
	}

	//______________________________________________________________________COMPUTATION

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
					/*if (mPoints[i][j][k].value > threshold)
						mPoints[i][j][k].value = 1.0f;
					else
						mPoints[i][j][k].value = 0.0f;*/
			}
		}
	}

	void thresholdingPointsToVector() {
		for (int i = 0; i < LENGTH * GRID_RES; i++) {
			for (int j = 0; j < HEIGHT * GRID_RES; j++) {
				for (int k = 0; k < WIDTH * GRID_RES; k++) {
					if (mPoints[i][j][k].value > threshold) {
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

	void marchThrough() {
		GRIDCELL gridcell;
		int index = 0;
		triangles.clear();
		for (unsigned int x = 0; x < (LENGTH * GRID_RES) - 1; x++) {
			for (unsigned int y = 0; y < (HEIGHT * GRID_RES) - 1; y++) {
				for (unsigned int z = 0; z < (WIDTH * GRID_RES) - 1; z++) {
					index = 0;
					{gridcell.p[index].x = mPoints[x][y][z].aPos.x;
					gridcell.p[index].y = mPoints[x][y][z].aPos.y;
					gridcell.p[index].z = mPoints[x][y][z].aPos.z;
					gridcell.val[index] = mPoints[x][y][z].value;
					index++;

					gridcell.p[index].x = mPoints[x + 1][y][z].aPos.x;
					gridcell.p[index].y = mPoints[x + 1][y][z].aPos.y;
					gridcell.p[index].z = mPoints[x + 1][y][z].aPos.z;
					gridcell.val[index] = mPoints[x + 1][y][z].value;
					index++;

					gridcell.p[index].x = mPoints[x + 1][y][z + 1].aPos.x;
					gridcell.p[index].y = mPoints[x + 1][y][z + 1].aPos.y;
					gridcell.p[index].z = mPoints[x + 1][y][z + 1].aPos.z;
					gridcell.val[index] = mPoints[x + 1][y][z + 1].value;
					index++;

					gridcell.p[index].x = mPoints[x][y][z + 1].aPos.x;
					gridcell.p[index].y = mPoints[x][y][z + 1].aPos.y;
					gridcell.p[index].z = mPoints[x][y][z + 1].aPos.z;
					gridcell.val[index] = mPoints[x][y][z + 1].value;
					index++;

					gridcell.p[index].x = mPoints[x][y + 1][z].aPos.x;
					gridcell.p[index].y = mPoints[x][y + 1][z].aPos.y;
					gridcell.p[index].z = mPoints[x][y + 1][z].aPos.z;
					gridcell.val[index] = mPoints[x][y + 1][z].value;
					index++;

					gridcell.p[index].x = mPoints[x + 1][y + 1][z].aPos.x;
					gridcell.p[index].y = mPoints[x + 1][y + 1][z].aPos.y;
					gridcell.p[index].z = mPoints[x + 1][y + 1][z].aPos.z;
					gridcell.val[index] = mPoints[x + 1][y + 1][z].value;
					index++;

					gridcell.p[index].x = mPoints[x + 1][y + 1][z + 1].aPos.x;
					gridcell.p[index].y = mPoints[x + 1][y + 1][z + 1].aPos.y;
					gridcell.p[index].z = mPoints[x + 1][y + 1][z + 1].aPos.z;
					gridcell.val[index] = mPoints[x + 1][y + 1][z + 1].value;
					index++;

					gridcell.p[index].x = mPoints[x][y + 1][z + 1].aPos.x;
					gridcell.p[index].y = mPoints[x][y + 1][z + 1].aPos.y;
					gridcell.p[index].z = mPoints[x][y + 1][z + 1].aPos.z;
					gridcell.val[index] = mPoints[x][y + 1][z + 1].value;
					index++; }
					nbrTriangles = mCube.polygonise(gridcell, 0.5f, &triangles);
				}
			}
		}
		//cout << __FUNCTION__ << "->number of triangles =" << nbrTriangles << endl;
	}

	void meshStepping() {
		if (step == CUBE_LIMIT)
			return;
		unsigned int tmp;
		unsigned int x, y, z;
		unsigned int xSize, ySize;

		ySize = 1 * ((WIDTH * GRID_RES) - 1);
		xSize = ySize * ((HEIGHT * GRID_RES) - 1);

		tmp = step;
		x = tmp / xSize;
		tmp -= (xSize * x);
		y = tmp / ySize;
		tmp -= (ySize * y);
		z = tmp;

		step++;

		GRIDCELL gridcell;
		int index = 0;
		gcp.clear();

		{gridcell.p[index].x = mPoints[x][y][z].aPos.x;
		gridcell.p[index].y = mPoints[x][y][z].aPos.y;
		gridcell.p[index].z = mPoints[x][y][z].aPos.z;
		gridcell.val[index] = mPoints[x][y][z].value;
		index++;

		gcp.push_back(mPoints[x][y][z].aPos.x);
		gcp.push_back(mPoints[x][y][z].aPos.y);
		gcp.push_back(mPoints[x][y][z].aPos.z);

		gcp.push_back(1.0f); gcp.push_back(0.0f); gcp.push_back(0.0f);

		gridcell.p[index].x = mPoints[x + 1][y][z].aPos.x;
		gridcell.p[index].y = mPoints[x + 1][y][z].aPos.y;
		gridcell.p[index].z = mPoints[x + 1][y][z].aPos.z;
		gridcell.val[index] = mPoints[x + 1][y][z].value;
		index++;

		gcp.push_back(mPoints[x + 1][y][z].aPos.x);
		gcp.push_back(mPoints[x + 1][y][z].aPos.y);
		gcp.push_back(mPoints[x + 1][y][z].aPos.z);

		gcp.push_back(1.0f); gcp.push_back(0.0f); gcp.push_back(0.0f);

		gridcell.p[index].x = mPoints[x + 1][y][z + 1].aPos.x;
		gridcell.p[index].y = mPoints[x + 1][y][z + 1].aPos.y;
		gridcell.p[index].z = mPoints[x + 1][y][z + 1].aPos.z;
		gridcell.val[index] = mPoints[x + 1][y][z + 1].value;
		index++;

		gcp.push_back(mPoints[x + 1][y][z + 1].aPos.x);
		gcp.push_back(mPoints[x + 1][y][z + 1].aPos.y);
		gcp.push_back(mPoints[x + 1][y][z + 1].aPos.z);

		gcp.push_back(1.0f); gcp.push_back(0.0f); gcp.push_back(0.0f);

		gridcell.p[index].x = mPoints[x][y][z + 1].aPos.x;
		gridcell.p[index].y = mPoints[x][y][z + 1].aPos.y;
		gridcell.p[index].z = mPoints[x][y][z + 1].aPos.z;
		gridcell.val[index] = mPoints[x][y][z + 1].value;
		index++;

		gcp.push_back(mPoints[x][y][z + 1].aPos.x);
		gcp.push_back(mPoints[x][y][z + 1].aPos.y);
		gcp.push_back(mPoints[x][y][z + 1].aPos.z);

		gcp.push_back(1.0f); gcp.push_back(0.0f); gcp.push_back(0.0f);

		gridcell.p[index].x = mPoints[x][y + 1][z].aPos.x;
		gridcell.p[index].y = mPoints[x][y + 1][z].aPos.y;
		gridcell.p[index].z = mPoints[x][y + 1][z].aPos.z;
		gridcell.val[index] = mPoints[x][y + 1][z].value;
		index++;

		gcp.push_back(mPoints[x][y + 1][z].aPos.x);
		gcp.push_back(mPoints[x][y + 1][z].aPos.y);
		gcp.push_back(mPoints[x][y + 1][z].aPos.z);

		gcp.push_back(1.0f); gcp.push_back(0.0f); gcp.push_back(0.0f);

		gridcell.p[index].x = mPoints[x + 1][y + 1][z].aPos.x;
		gridcell.p[index].y = mPoints[x + 1][y + 1][z].aPos.y;
		gridcell.p[index].z = mPoints[x + 1][y + 1][z].aPos.z;
		gridcell.val[index] = mPoints[x + 1][y + 1][z].value;
		index++;

		gcp.push_back(mPoints[x + 1][y + 1][z].aPos.x);
		gcp.push_back(mPoints[x + 1][y + 1][z].aPos.y);
		gcp.push_back(mPoints[x + 1][y + 1][z].aPos.z);

		gcp.push_back(1.0f); gcp.push_back(0.0f); gcp.push_back(0.0f);

		gridcell.p[index].x = mPoints[x + 1][y + 1][z + 1].aPos.x;
		gridcell.p[index].y = mPoints[x + 1][y + 1][z + 1].aPos.y;
		gridcell.p[index].z = mPoints[x + 1][y + 1][z + 1].aPos.z;
		gridcell.val[index] = mPoints[x + 1][y + 1][z + 1].value;
		index++;

		gcp.push_back(mPoints[x + 1][y + 1][z + 1].aPos.x);
		gcp.push_back(mPoints[x + 1][y + 1][z + 1].aPos.y);
		gcp.push_back(mPoints[x + 1][y + 1][z + 1].aPos.z);

		gcp.push_back(1.0f); gcp.push_back(0.0f); gcp.push_back(0.0f);

		gridcell.p[index].x = mPoints[x][y + 1][z + 1].aPos.x;
		gridcell.p[index].y = mPoints[x][y + 1][z + 1].aPos.y;
		gridcell.p[index].z = mPoints[x][y + 1][z + 1].aPos.z;
		gridcell.val[index] = mPoints[x][y + 1][z + 1].value;
		index++;

		gcp.push_back(mPoints[x][y + 1][z + 1].aPos.x);
		gcp.push_back(mPoints[x][y + 1][z + 1].aPos.y);
		gcp.push_back(mPoints[x][y + 1][z + 1].aPos.z); }

		gcp.push_back(1.0f); gcp.push_back(0.0f); gcp.push_back(0.0f);

		glBindBuffer(GL_ARRAY_BUFFER, VBO_gcp);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 8 * 6, &gcp[0]);

		testTriangles.clear();
		nbrTriangles = mCube.polygonise(gridcell, threshold, &testTriangles);
		cubeIndex = mCube.cubeindex;

		//testMeshData.clear();

		for (unsigned int i = 0; i < testTriangles.size(); i++) {
			//cout << "Triangle_" << i << " :\n";
			for (int j = 0; j < 3; j++) {
				testMeshData.push_back(testTriangles.at(i).p[j].x);
				testMeshData.push_back(testTriangles.at(i).p[j].y);
				testMeshData.push_back(testTriangles.at(i).p[j].z);

				//cout << "\t" << testTriangles.at(i).p[j].x << ", " << testTriangles.at(i).p[j].y << ", " << testTriangles.at(i).p[j].z << endl;

				testMeshData.push_back(testTriangles.at(i).n.x);
				testMeshData.push_back(testTriangles.at(i).n.y);
				testMeshData.push_back(testTriangles.at(i).n.z);

				//testMeshData.push_back(1.0f);
				//testMeshData.push_back(1.0f);
				//testMeshData.push_back(0.2f);
			}
		}

		glBindBuffer(GL_ARRAY_BUFFER, VBO_meshTesting);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * testMeshData.size(), &testMeshData[0]);
		meshSize += testTriangles.size();
		lastOffset += sizeof(float) * testTriangles.size() * 3 * 6;
		if (!true) {
			cout << __FUNCTION__ << "->step " << step << endl;
			cout << "\t" << x << ", " << y << ", " << z << endl;
			Display::disp(gridcell);
			if (nbrTriangles != 0)
				cout << "Coordinates of the triangles :" << endl;
			cout << "Lastoffset = " << lastOffset << "\nMesh size = " << meshSize << endl;
		}
	}

	void refreshMCdebug(GRIDCELL *aGrid, vector<float> *vertices, vector<float> *points, vector<TRIANGLE> *triangles,  int cubeIndex) {
		for (int i = 7; i >= 0; i--) {
			if ((cubeIndex & (int)pow(2, i)) == (int)pow(2, i))
				aGrid->val[i] = 0.0f;
			else
				aGrid->val[i] = 1.0f;
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

	void setMesh() {
		for (unsigned int i = 0; i < triangles.size(); i++) {
			for (int j = 0; j < 3; j++) {
				meshData.push_back(triangles.at(i).p[j].x);
				meshData.push_back(triangles.at(i).p[j].y);
				meshData.push_back(triangles.at(i).p[j].z);

				meshData.push_back(triangles.at(i).n.x);
				meshData.push_back(triangles.at(i).n.y);
				meshData.push_back(triangles.at(i).n.z);
			}
		}
	}

	//______________________________________________________________________BUFFERS/SHADERS SETTING

	void initShaders() {
		pointShader = Shader("resources/shaders/vShaderSourcePoint.vs", "resources/shaders/fShaderSourcePoint.fs");
		meshShader = Shader("resources/shaders/vShaderSource3D_OLD.vs", "resources/shaders/fShaderSource3D_OLD.fs");
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

	void initBuffersTesting() {
		glGenVertexArrays(1, &VAO_meshTesting);
		glGenBuffers(1, &VBO_meshTesting);

		glBindVertexArray(VAO_meshTesting);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_meshTesting);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 5 * 3 * 6 * CUBE_LIMIT, nullptr, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glBindVertexArray(0);

		glGenVertexArrays(1, &VAO_gcp);
		glGenBuffers(1, &VBO_gcp);

		glBindVertexArray(VAO_gcp);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_gcp);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8 * 6, nullptr, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glBindVertexArray(0);
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

	void renderTestingMesh(Camera pov, float ratio) {
		meshShader.use();
		meshShader.setVec3("objectColor", vec3(1.0f, 1.0f, 1.0f));
		meshShader.setVec3("lightColor", vec3(1.0f, 1.0f, 0.9f));
		meshShader.setVec3("lightPos", vec3(-25.0f, -50.0f, -25.0f));
		meshShader.setMat4("view", pov.lookAt());
		meshShader.setMat4("projection", perspective(radians(pov.getFOV()), ratio, 0.1f, 10000.0f));
		meshShader.setMat4("model", mat4(1.0f));

		glBindVertexArray(VAO_meshTesting);
		glDrawArrays(GL_TRIANGLES, 0, testMeshData.size() / 6);
		glBindVertexArray(0);
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
	GLuint VAO_gcp, VBO_gcp;
	GLuint VAO_points, VBO_points;
	GLuint VAO_mesh, VBO_mesh;
	GLuint VAO_meshTesting, VBO_meshTesting;
	vector<float> gcp;
	Terrain terrain;
	unsigned int step = 0, lastOffset = 0, meshSize = 0, cubeIndex = 0;
	int nbrTriangles;

private:
	
	Shader meshShader;
	int seed = 0;
	PerlinNoise perlin;
	MarchingCube mCube;
	float threshold = 0.50f;
	vector<TRIANGLE> testTriangles;
	vector<TRIANGLE> triangles;
	vector<float> testMeshData;
	vector<float> meshData;
	

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