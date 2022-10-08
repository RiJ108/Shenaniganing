#include <engine.hpp>

void Engine::updateSurrounding(Entity entity) {
	vec3 newCP;
	for (int i = 0; i < 3; i++) {
		if (entity.position[i] > 0) newCP[i] = (int)(entity.position[i] + (DIM - 1) / 2) / (DIM - 1);
		else newCP[i] = (int)(entity.position[i] - (DIM - 1) / 2) / (DIM - 1);
	}
	if (newCP != oldCP) {
		oldCP = newCP;
		updateNeeded = true;
	}
};
void Engine::bufferSwapLoop() {
	cout << __FUNCTION__ << "->Launched" << endl;
	while (true) {
		for (const auto m : activeWorldMesh) {
			if (m->needSwap) {
				m->swapBuffers();
				m->needSwap = false;
				m->busy = false;
			}
		}
	}
};
void Engine::surroundingGenLoop() {
	cout << __FUNCTION__ << "->Launched" << endl;
	int index;
	while (true) {
		if (updateNeeded == 1) {
			generateSurroundingChunks();
		}
	}
};
void Engine::generateSurroundingChunksIC() {
	clock_t stt = clock(); Mesh* tmp; vector<float> data;
	for (int i = 0; i < kernelSize[0]; i++) {
		for (int j = 0; j < kernelSize[1]; j++) {
			for (int k = 0; k < kernelSize[2]; k++) {
				tmp = new Mesh;
				//**
				tmp->genBuffers();
				tmp->setBuffers();
				glBindBuffer(GL_ARRAY_BUFFER, tmp->VBO);
				tmp->bufferPtr = glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(float) * 6 * 3 * 5 * CUBE_LIMIT, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
				glBindBuffer(GL_ARRAY_BUFFER, tmp->VBOb);
				tmp->bufferPtrb = glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(float) * 6 * 3 * 5 * CUBE_LIMIT, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
				//**
				tmp->fillPrimaireBufferData(getData(getTriangles(vec3(i, j, k) - offsets + oldCP)));
				//**
				activeWorldMesh.push_back(tmp);
			}
		}
	}
	cout << __FUNCTION__ << "->Finished in " << difftime(clock(), stt) << "ms." << endl;
};
void Engine::generateSurroundingChunks() {
	clock_t stt = clock(); int index = 0; Mesh* tmp;
	for (int i = 0; i < kernelSize[0]; i++) {
		for (int j = 0; j < kernelSize[1]; j++) {
			for (int k = 0; k < kernelSize[2]; k++) {
				tmp = activeWorldMesh.at(index);
				//**
				tmp->busy = true;
				tmp->fillSecondBufferData(getData(getTriangles(vec3(i, j, k) - offsets + oldCP)));
				tmp->needSwap = true;
				//**
				index++;
			}
		}
	}
	updateNeeded = false;
	cout << __FUNCTION__ << "->Finished in " << difftime(clock(), stt) << "ms." << endl;
}
vector<float> Engine::getData(vector<TRIANGLE> triangles) {
	vector<float> data;
	for (unsigned int i = 0; i < triangles.size(); i++) {
		for (int j = 0; j < 3; j++) {
			data.push_back(triangles.at(i).points[j].x);
			data.push_back(triangles.at(i).points[j].y);
			data.push_back(triangles.at(i).points[j].z);
			data.push_back(triangles.at(i).norm.x);
			data.push_back(triangles.at(i).norm.y);
			data.push_back(triangles.at(i).norm.z);
		}
	}
	return data;
};
void Engine::setGC(GRIDCELL* aGc, vec3 aPos) {
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
};
vector<TRIANGLE> Engine::getTriangles(vec3 aPosition) {
	vec3 position; vector<TRIANGLE> triangles;
	for (unsigned int x = 0; x < DIM_AUG - 1; x++) {
		for (unsigned int y = 0; y < HEIGHT_AUG - 1; y++) {
			for (unsigned int z = 0; z < DIM_AUG - 1; z++) {
				position = vec3(x * STEP - x_offset, y * STEP - y_offset, z * STEP - z_offset);
				position += aPosition * vec3(DIM - 1);
				setGC(gridcellPtr, position);
				mCube.polygonise(*gridcellPtr, 0.5f, &triangles);
				for (int i = 0; i < 5 - mCube.nbrTriangle[mCube.cubeindex]; i++)
					triangles.push_back(emptyTriangle);
			}
		}
	}
	return triangles;
};
void Engine::initShaders() {
	pointShader = Shader("resources/shaders/vShaderSourcePoint.glsl", "resources/shaders/fShaderSourcePoint.glsl");
	meshShader = Shader("resources/shaders/vShaderSource3D.glsl", "resources/shaders/fShaderSource3D.glsl");
	shadowShader = Shader("resources/shaders/vShaderSourceShadow.glsl", "resources/shaders/fShaderSourceShadow.glsl");
};
void Engine::renderMeshes(Entity entity, float ratio, vector<Mesh*> meshes) {
	for (int i = 0; i < meshes.size(); i++) {
		if (!meshes.at(i)->busy) {
			float aColor = (float)(i + 1) / meshes.size();
			renderMesh(entity, ratio, meshes.at(i), vec3(aColor, 1 - aColor, -aColor));
		}
	}
};
void Engine::renderMesh(Camera pov, float ratio, Mesh* mesh) {
	meshShader.use();
	meshShader.setVec3("objectColor", vec3(1.0f, 1.0f, 1.0f));
	meshShader.setVec3("lightColor", vec3(1.0f, 1.0f, 0.9f));
	meshShader.setVec3("lightPos", vec3(-25.0f, 50.0f, -25.0f));
	meshShader.setMat4("view", pov.lookAt());
	meshShader.setMat4("projection", perspective(radians(pov.getFOV()), ratio, 0.1f, 10000.0f));
	meshShader.setMat4("model", mat4(1.0f));

	mesh->render();
};
void Engine::renderMesh(Entity entity, float ratio, Mesh* mesh) { renderMesh(entity, ratio, mesh, vec3(1.0f)); }
void Engine::renderMesh(Entity entity, float ratio, Mesh* mesh, vec3 aObjectColor) {
	meshShader.use();
	meshShader.setVec3("objectColor", aObjectColor);
	meshShader.setVec3("lightColor", vec3(1.0f, 1.0f, 0.9f));
	meshShader.setVec3("lightPos", lightPos);
	meshShader.setMat4("view", entity.lookAt());
	meshShader.setMat4("projection", perspective(radians(entity.FOV), ratio, 0.1f, 10000.0f));
	meshShader.setMat4("model", mat4(1.0f));

	mesh->render();
};