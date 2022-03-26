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

	}
	~Engine() {

	}
	//______________________________________________________________________COMPUTATION
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
	void generateMeshTriangles() { generateMeshTriangles(&worldMesh); } void generateMeshTriangles(Mesh* mesh) {
		for (unsigned int x = 0; x < DIM_AUG - 1; x++) {
			for (unsigned int y = 0; y < HEIGHT_AUG - 1; y++) {
				for (unsigned int z = 0; z < DIM_AUG - 1; z++) {
					setGC(gridcellPtr, vec3(x * STEP - x_offset, y * STEP - y_offset, z * STEP - z_offset));
					mCube.polygonise(*gridcellPtr, 0.5f, &mesh->triangles);
					for (int i = 0; i < 5 - mCube.nbrTriangle[mCube.cubeindex]; i++)
						mesh->triangles.push_back(emptyTriangle);
				}
			}
		}
	}
	void setMesh() { setMesh(&worldMesh); } void setMesh(Mesh *mesh) {
		mesh->loadDataFromTriangles();
		mesh->setBuffers();
		cout << __FUNCTION__ << "->" << mesh->data.size() / 6 << " Triangles in the mesh\n";
	}
	//______________________________________________________________________BUFFERS/SHADERS SETTING
	void initShaders() {
		pointShader = Shader("resources/shaders/vShaderSourcePoint.glsl", "resources/shaders/fShaderSourcePoint.glsl");
		meshShader = Shader("resources/shaders/vShaderSource3D_OLD.glsl", "resources/shaders/fShaderSource3D_OLD.glsl");
	}
	//______________________________________________________________________RENDERS
	void renderMesh(Camera pov, float ratio) { renderMesh(pov, ratio, &worldMesh); } void renderMesh(Camera pov, float ratio, Mesh* mesh) {
		meshShader.use();
		meshShader.setVec3("objectColor", vec3(1.0f, 1.0f, 1.0f));
		meshShader.setVec3("lightColor", vec3(1.0f, 1.0f, 0.9f));
		meshShader.setVec3("lightPos", vec3(-25.0f, 50.0f, -25.0f));
		meshShader.setMat4("view", pov.lookAt());
		meshShader.setMat4("projection", perspective(radians(pov.getFOV()), ratio, 0.1f, 10000.0f));
		meshShader.setMat4("model", mat4(1.0f));

		mesh->render();
	}
private:
	//** mesh var
	Mesh worldMesh;
	GRIDCELL* gridcellPtr = new GRIDCELL;

	//** gen var
	int seed = 69;
	PerlinNoise perlin;
	MarchingCube mCube;
	TRIANGLE emptyTriangle;
	float threshold = 0.50f;
	float x_offset = DIM / 2.0f, y_offset = 0.0f, z_offset = DIM / 2.0f;

	//** rendering var
	Shader shader;
	Shader pointShader;
	Shader meshShader;
};