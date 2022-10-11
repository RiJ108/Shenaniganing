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
	void launchAndForget();
	bool inKernel(ivec3 aCoord);
	void uFunction();
	void bufferSwapLoop();
	vector<float> getTrianglesData(vec3 aPosition);
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
	bool tFlag = false;
	vec3 moveVec;
	bool genFlag = false;

	vector<ivec3> kernelCoords;
	int seed = time(nullptr);
	PerlinNoise perlin = PerlinNoise(69);
	MarchingCube mCube;
	TRIANGLE emptyTriangle;
	float threshold = 0.50f;
	float x_offset = DIM / 2.0f, y_offset = DIM / 2.0f, z_offset = DIM / 2.0f;
	vec3 oldCP = vec3(0.0f);
	vec3 kernelSize = vec3(9.0f);
	vec3 offsets = 0.5f * (kernelSize - vec3(1));
	vec3 coordOffset[8] = { vec3(0.0f),
						vec3(1.0f, 0.0f, 0.0f),
						vec3(1.0f, 0.0f, 1.0f),
						vec3(0.0f, 0.0f, 1.0f),
						vec3(0.0f, 1.0f, 0.0f),
						vec3(1.0f, 1.0f, 0.0f),
						vec3(1.0f, 1.0f, 1.0f),
						vec3(0.0f, 1.0f, 1.0f) };

	//** rendering var
	Shader shader;
	Shader pointShader;
	Shader meshShader;
	Shader shadowShader;
	vec3 lightPos = vec3(-25.0f, 50.0f, -25.0f);
};