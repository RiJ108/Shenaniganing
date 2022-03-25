#pragma once

#include "displays.hpp"
#include "shader.hpp"
#include "perlinNoise.hpp"
#include "camera.hpp"
#include "marchingCube.hpp"
#include "mesh.hpp"
#include "constant.hpp"

class Engine {
public:
	Engine() {
		worldGridcellIndex = (int*)malloc(sizeof(int) * CUBE_LIMIT);
	}

	~Engine() {
		if (c_pointPtr)
			free(c_pointPtr);
		free(worldGridcellIndex);
	}

	//______________________________________________________________________COMPUTATION

	void generateMeshTriangles() {
		vec3 coordOffset[8] = { vec3(0.0f), vec3(1.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f), vec3(1.0f, 1.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 1.0f, 1.0f) };
		vec3 position, positionOffsetted;
		float noiseValue;
		unsigned int index = 0;

		for (unsigned int x = 0; x < (LENGTH * GRID_RES) - 1; x++) {
			for (unsigned int y = 0; y < (HEIGHT * GRID_RES) - 1; y++) {
				for (unsigned int z = 0; z < (WIDTH * GRID_RES) - 1; z++) {
					position = vec3(x * (1.0f / GRID_RES) - x_offset, y * (1.0f / GRID_RES) - y_offset, z * (1.0f / GRID_RES) - z_offset);
					for (int i = 0; i < 8; i++) {
						positionOffsetted = position + (1.0f / GRID_RES) * coordOffset[i];
						noiseValue = 1.0f * perlin.noise(0.15f * positionOffsetted);
						//noiseValue += (positionOffsetted.y / HEIGHT);
						gridcellPtr->points[i].coord = positionOffsetted;
						gridcellPtr->points[i].val = noiseValue;
					}
					mCube.polygonise(*gridcellPtr, 0.5f, &meshTriangles);
					mCube.polygonise(*gridcellPtr, 0.5f, &worldMesh.triangles);
					for (int i = 0; i < 5 - mCube.nbrTriangle[mCube.cubeindex]; i++) {
						meshTriangles.push_back(emptyTriangle);
						worldMesh.triangles.push_back(emptyTriangle);
					}
					worldGridcellIndex[index] = mCube.cubeindex;
					index++;
				}
			}
		}
	}

	ivec3 indices;
	int gridcellIndexTrack;

	bool computeCollision(vec3 position) {
		unsigned int ind;
		vector<TRIANGLE> tmpTriangles;
		vector<vec3> norms;
		vector<vec3> centers;
		vec3 a, b;
		float angle;
		indices.x = (position.x + x_offset) / (1.0f / GRID_RES);
		indices.y = (position.y + y_offset) / (1.0f / GRID_RES);
		indices.z = (position.z + z_offset) / (1.0f / GRID_RES);
		gridcellIndexTrack = -1;
		if (indices.z >= 0 && indices.z < ((WIDTH * GRID_RES) - 1) &&
			indices.y >= 0 && indices.y < ((HEIGHT * GRID_RES) - 1) &&
			indices.x >= 0 && indices.x < ((LENGTH * GRID_RES) - 1)) {
			ind = indices.z +
				(indices.y * ((WIDTH * GRID_RES) - 1)) +
				(indices.x * (((WIDTH * GRID_RES) - 1) * ((HEIGHT * GRID_RES) - 1)));
			gridcellIndexTrack = worldGridcellIndex[ind];
			//Test of collision here. Retrieve the triangles values from the GPU buffer to have the mesh on which the computation will need to be performed.
			if (gridcellIndexTrack != 0) {
				if (gridcellIndexTrack == 0) {
					return false;
				}
				else if (gridcellIndexTrack == 255) {
					return true;
				}
				else {
					for (int i = 0; i < mCube.nbrTriangle[gridcellIndexTrack]; i++) {
						a = vec3(meshTriangles.at((ind * 5) + i).points[1].x - meshTriangles.at((ind * 5) + i).points[0].x,
							meshTriangles.at((ind * 5) + i).points[1].y - meshTriangles.at((ind * 5) + i).points[0].y,
							meshTriangles.at((ind * 5) + i).points[1].z - meshTriangles.at((ind * 5) + i).points[0].z);
						b = vec3(meshTriangles.at((ind * 5) + i).points[2].x - meshTriangles.at((ind * 5) + i).points[1].x,
							meshTriangles.at((ind * 5) + i).points[2].y - meshTriangles.at((ind * 5) + i).points[1].y,
							meshTriangles.at((ind * 5) + i).points[2].z - meshTriangles.at((ind * 5) + i).points[1].z);
						norms.push_back(normalize(cross(a, b)));
						centers.push_back((1.0f / 3.0f) * vec3(meshTriangles.at((ind * 5) + i).points[0].x + meshTriangles.at((ind * 5) + i).points[1].x + meshTriangles.at((ind * 5) + i).points[2].x,
							meshTriangles.at((ind * 5) + i).points[0].y + meshTriangles.at((ind * 5) + i).points[1].y + meshTriangles.at((ind * 5) + i).points[2].y,
							meshTriangles.at((ind * 5) + i).points[0].z + meshTriangles.at((ind * 5) + i).points[1].z + meshTriangles.at((ind * 5) + i).points[2].z));
					}
					for (int i = 0; i < mCube.nbrTriangle[gridcellIndexTrack]; i++) {
						angle = dot(norms.at(i), position - centers.at(i));
						//cout << angle << endl;
						if (angle <= 0)
							return true;
					}
				}
			}
		}
		return false;
	}

	void setMesh() {
		worldMesh.loadDataFromTriangles();
		worldMesh.setBuffers();
		cout << __FUNCTION__ << "->" << meshData.size() / 6 << " Triangles in the mesh\n";
	}

	//______________________________________________________________________BUFFERS/SHADERS SETTING

	void initShaders() {
		pointShader = Shader("resources/shaders/vShaderSourcePoint.glsl", "resources/shaders/fShaderSourcePoint.glsl");
		meshShader = Shader("resources/shaders/vShaderSource3D_OLD.glsl", "resources/shaders/fShaderSource3D_OLD.glsl");
	}

	//______________________________________________________________________RENDERS

	void renderMesh(Camera pov, float ratio) {
		meshShader.use();
		meshShader.setVec3("objectColor", vec3(1.0f, 1.0f, 1.0f));
		meshShader.setVec3("lightColor", vec3(1.0f, 1.0f, 0.9f));
		meshShader.setVec3("lightPos", vec3(-25.0f, 50.0f, -25.0f));
		meshShader.setMat4("view", pov.lookAt());
		meshShader.setMat4("projection", perspective(radians(pov.getFOV()), ratio, 0.1f, 10000.0f));
		meshShader.setMat4("model", mat4(1.0f));

		worldMesh.render();
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

	GLuint VAO_gcp, VBO_gcp;
	vector<float> gcp;
	unsigned int step = 0, lastOffset = 0, meshSize = 0, cubeIndex = 0;
	int nbrTriangles;

private:
	//__OPT           !!VALIDE!!
	Mesh worldMesh;
	vector<GRIDCELL> worldGridcells;
	vector<TRIANGLE> meshTriangles;
	TRIANGLE emptyTriangle;
	vector<float> meshData;
	float worldPointsValue_min = HUGE_VALF;
	float worldPointsValue_max = 0.0f;
	C_POINT* c_pointPtr = new C_POINT;
	GRIDCELL* gridcellPtr = new GRIDCELL;
	GLuint VAO_mesh, VBO_mesh;
	float threshold = 0.50f;
	float x_offset = LENGTH / 2.0f;
	float y_offset = 0.0f; // HEIGHT / 2.0f;
	float z_offset = WIDTH / 2.0f;
	int* worldGridcellIndex;

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