#pragma once

#include <ctime>
#include <cmath>
#include <future>

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
	void launchAndForget() {
		thread thb = thread(&Engine::surroundingGenLoop, this);
		thb.detach();

		thread th = thread(&Engine::bufferSwapLoop, this);
		th.detach();
	};
	void bufferSwapLoop();
	void genSur();
	void surroundingGenLoop();
	void updateSurrounding(Entity entity);
	void generateSurroundingChunks();
	void generateSurroundingChunksIC();
	Mesh* genChunk(vec3 position);
	void setGC(GRIDCELL* aGc, vec3 aPos);
	vector<TRIANGLE> getTriangles(vec3 aPosition);
	vector<float> getData(vector<TRIANGLE> triangles);
	void setMesh(Mesh* mesh);
	//______________________________________________________________________BUFFERS/SHADERS SETTING
	void initShaders();
	//______________________________________________________________________RENDERS
	void renderMeshes(Entity entity, float ratio, vector<Mesh*> meshes);
	void renderMesh(Camera pov, float ratio, Mesh* mesh);
	void renderMesh(Entity entity, float ratio, Mesh* mesh);
	void renderMesh(Entity entity, float ratio, Mesh* mesh, vec3 aObjectColor);

	//** mesh var
	vector<Mesh*> activeWorldMesh;
	GRIDCELL* gridcellPtr = new GRIDCELL;
	//** gen var
	bool updateNeeded = false;
	vec3 moveVec;
	bool genFlag = false;

	int seed = time(nullptr);
	PerlinNoise perlin = PerlinNoise(seed);
	MarchingCube mCube;
	TRIANGLE emptyTriangle;
	float threshold = 0.50f;
	float x_offset = DIM / 2.0f, y_offset = DIM / 2.0f, z_offset = DIM / 2.0f;
	vec3 oldCP = vec3(0.0f);
	vec3 kernelSize = vec3(5.0f);
	vec3 offsets = 0.5f * (kernelSize - vec3(1));
	//** rendering var
	Shader shader;
	Shader pointShader;
	Shader meshShader;
	Shader shadowShader;
	vec3 lightPos = vec3(-25.0f, 50.0f, -25.0f);
};