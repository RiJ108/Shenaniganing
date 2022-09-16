#pragma once

#include <ctime>

#include <mutex>
#include <thread>
#include <condition_variable>

#include <cmath>

#include "displays.hpp"
#include "shader.hpp"
#include "perlinNoise.hpp"
#include "camera.hpp"
#include "marchingCube.hpp"
#include "mesh.hpp"
#include "constant.hpp"
#include "entity.hpp"

class Engine {
public:
	//______________________________________________________________________COMPUTATION
	void updateSurrounding(Entity entity) {
		vec3 newCP;
		for (int i = 0; i < 3; i++) {
			if (entity.position[i] > 0) newCP[i] = (int)(entity.position[i] + (DIM - 1) / 2) / (DIM - 1);
			else newCP[i] = (int)(entity.position[i] - (DIM - 1) / 2) / (DIM - 1);
		}
		if (newCP != oldCP) {
			moveVec = newCP - oldCP;
			//Display::dispLn(moveVec);
			/*int offset = 0;
			for (int i = 0; i < 3; i++)
				offset += pow(kernelSize[i],2-i) * moveVec[i];
			int index = 0;

			vector<pair<int, int>>a;

			for (int i = 0; i < kernelSize[0]; i++) {
				for (int j = 0; j < kernelSize[1]; j++) {
					for (int k = 0; k < kernelSize[2]; k++) {
						if ((moveVec.x != 0 && (moveVec.x + i) == 1) || (moveVec.y != 0 && (moveVec.y + j) == 1) || (moveVec.z != 0 && (moveVec.z + k) == 1)) {}
						else a.push_back(make_pair(index-offset, index));
						index++;
					}
				}
			}*/
			oldCP = newCP;
			/*sort(a.begin(), a.end());
			vector<pair<int, int>>::iterator it;
			ivec3 pos;
			int buff = 0; bool found;
			for (int i = 0; i < index; i++) {
				it = ranges::find(a, i, &pair<int, int>::first);
				found = it != a.end();
				if (!found) {
					pos.x = i / (pow(kernelSize[0], 2));
					pos.y = (i - pos.x * (pow(kernelSize[0], 2))) / kernelSize[0];
					pos.z = (i - pos.x * (pow(kernelSize[0], 2)) - pos.y * kernelSize[0]);
					iwmPtr->push_back(genChunk(vec3(pos) - offsets + oldCP));
				}
				else iwmPtr->push_back(awmPtr->at((*it).second));
			}
			*awmPtr = *iwmPtr;*/

			int index = 0;
			for (int i = 0; i < kernelSize[0]; i++) {
				for (int j = 0; j < kernelSize[1]; j++) {
					for (int k = 0; k < kernelSize[2]; k++) {
						Mesh* tmp = activeWorldMesh.at(index);
						generateMeshTriangles(tmp, vec3(i, j, k) - offsets + oldCP);
						tmp->loadDataFromTriangles();
						tmp->setBuffers();
						tmp->fillBuffers();
						tmp->clears();
						index++;
					}
				}
			}
		}
	}

	void genSurroundingChunks() {
		for (int i = 0; i < kernelSize[0]; i++) {
			for (int j = 0; j < kernelSize[1]; j++) {
				for (int k = 0; k < kernelSize[2]; k++) {
					activeWorldMesh.push_back(genChunk(vec3(i, j, k) - offsets + oldCP));
				}
			}
		}
	}

	shared_ptr<Mesh*> genChunk_(vec3 position) {
		Mesh* meshPtr;
		meshPtr = new Mesh();
		generateMeshTriangles(meshPtr, position);
		setMesh(meshPtr);
		auto sp = make_shared<Mesh*>(meshPtr);
		return sp;
	}

	Mesh* genChunk(vec3 position) {
		Mesh* meshPtr;
		meshPtr = new Mesh();
		generateMeshTriangles(meshPtr, position);
		setMesh(meshPtr);
		return meshPtr;
	}

	void setGC(GRIDCELL* aGc, vec3 aPos) {
		vec3 coordOffset[8] = { vec3(0.0f),
								vec3(1.0f, 0.0f, 0.0f),
								vec3(1.0f, 0.0f, 1.0f),
								vec3(0.0f, 0.0f, 1.0f),
								vec3(0.0f, 1.0f, 0.0f),
								vec3(1.0f, 1.0f, 0.0f),
								vec3(1.0f, 1.0f, 1.0f),
								vec3(0.0f, 1.0f, 1.0f) };
		vec3 pOffset;
		float noiseValue;
		for (int i = 0; i < 8; i++) {
			pOffset = aPos + STEP * coordOffset[i];
			noiseValue = 1.0f * perlin.noise(0.15f * pOffset);
			aGc->apex[i].coordinates = pOffset;
			aGc->apex[i].value = noiseValue;
		}
	}
	void generateMeshTriangles(Mesh* mesh) { generateMeshTriangles(mesh, vec3(0.0f)); };
	void generateMeshTriangles(Mesh* mesh, vec3 chunkPosition) {
		vec3 position;
		for (unsigned int x = 0; x < DIM_AUG - 1; x++) {
			for (unsigned int y = 0; y < HEIGHT_AUG - 1; y++) {
				for (unsigned int z = 0; z < DIM_AUG - 1; z++) {
					position = vec3(x * STEP - x_offset, y * STEP - y_offset, z * STEP - z_offset);
					position += chunkPosition * vec3(DIM-1);
					setGC(gridcellPtr, position);
					mCube.polygonise(*gridcellPtr, 
						0.5f,
						&mesh->triangles);
					for (int i = 0; i < 5 - mCube.nbrTriangle[mCube.cubeindex]; i++)
						mesh->triangles.push_back(emptyTriangle);
				}
			}
		}
	}
	void setMesh(Mesh *mesh) {
		mesh->loadDataFromTriangles();
		mesh->genBuffers();
		mesh->setBuffers();
		mesh->fillBuffers();
		mesh->clears();
	}
	vec3 capsuleMeshCollision(vec2 capsuleDim, Mesh *mesh) {

	};
	//______________________________________________________________________BUFFERS/SHADERS SETTING
	void initShaders() {
		pointShader = Shader("resources/shaders/vShaderSourcePoint.glsl", "resources/shaders/fShaderSourcePoint.glsl");
		//meshShader = Shader("resources/shaders/vShaderSource3D_OLD.glsl", "resources/shaders/fShaderSource3D_OLD.glsl");
		meshShader = Shader("resources/shaders/vShaderSource3D.glsl", "resources/shaders/fShaderSource3D.glsl");
	}
	//______________________________________________________________________RENDERS
	void renderMeshForDepth(vec3 position, vec3 front, float fov, float ratio) {
		meshShader.use();
	}
	void renderActiveMeshs(Entity entity, float ratio) {
		for (int i = 0; i < awmPtr->size(); i++) {
			float aColor = (float)(i + 1) / awmPtr->size();
			renderMesh(entity, ratio, awmPtr->at(i), vec3(aColor, 1-aColor, -aColor));
		}
	}
	void renderMesh(Camera pov, float ratio, Mesh* mesh) {
		meshShader.use();
		meshShader.setVec3("objectColor", vec3(1.0f, 1.0f, 1.0f));
		meshShader.setVec3("lightColor", vec3(1.0f, 1.0f, 0.9f));
		meshShader.setVec3("lightPos", vec3(-25.0f, 50.0f, -25.0f));
		meshShader.setMat4("view", pov.lookAt());
		meshShader.setMat4("projection", perspective(radians(pov.getFOV()), ratio, 0.1f, 10000.0f));
		meshShader.setMat4("model", mat4(1.0f));

		mesh->render();
	}
	void renderMesh(Entity entity, float ratio, Mesh* mesh) { renderMesh(entity, ratio, mesh, vec3(1.0f)); }
	void renderMesh(Entity entity, float ratio, Mesh* mesh, vec3 aObjectColor) {
		meshShader.use();
		meshShader.setVec3("objectColor", aObjectColor);
		meshShader.setVec3("lightColor", vec3(1.0f, 1.0f, 0.9f));
		meshShader.setVec3("lightPos", vec3(-25.0f, 50.0f, -25.0f));
		meshShader.setMat4("view", entity.lookAt());
		meshShader.setMat4("projection", perspective(radians(entity.FOV), ratio, 0.1f, 10000.0f));
		meshShader.setMat4("model", mat4(1.0f));

		mesh->render();
	}

	//** mesh var
	vector<shared_ptr<Mesh*>> aWM_;
	vector<Mesh*> activeWorldMesh, activeWorldMesh_tmp;
	vector<Mesh*> *awmPtr = &activeWorldMesh, *iwmPtr = &activeWorldMesh_tmp;
	GRIDCELL* gridcellPtr = new GRIDCELL;
	//** gen var
	vec3 moveVec;

	int seed = time(nullptr);
	PerlinNoise perlin = PerlinNoise(seed);
	MarchingCube mCube;
	TRIANGLE emptyTriangle;
	float threshold = 0.50f;
	float x_offset = DIM / 2.0f, y_offset = DIM / 2.0f, z_offset = DIM / 2.0f;
	vec3 oldCP = vec3(0.0f);
	vec3 kernelSize = vec3(3.0f);
	vec3 offsets = 0.5f * (kernelSize - vec3(1));
	//** rendering var
	Shader shader;
	Shader pointShader;
	Shader meshShader;
};