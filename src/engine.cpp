#include <engine.hpp>

void Engine::tFunction() {
	cout << __FUNCTION__ << endl;
	for (Mesh* p : activeWorldMesh) {
		p->busy = true;
		/*void* ptr = glMapNamedBuffer(p->VBO, GL_WRITE_ONLY);

		if(glUnmapNamedBuffer(p->VBO))*/
			p->busy = false;
	}
	genFlag = false;
};

void Engine::updateSurrounding(Entity entity, GLFWwindow* wHandler) {
	vec3 newCP;
	for (int i = 0; i < 3; i++) {
		if (entity.position[i] > 0) newCP[i] = (int)(entity.position[i] + (DIM - 1) / 2) / (DIM - 1);
		else newCP[i] = (int)(entity.position[i] - (DIM - 1) / 2) / (DIM - 1);
	}
	if (newCP != oldCP) {
		moveVec = newCP - oldCP;
		oldCP = newCP;
		cout << "\n__\n";
		async(std::launch::async, &Engine::generateSurroundingChunks, this, activeWorldMesh);
	}
	if (genFlag) {
		cout << __FUNCTION__ << " UPDATING aWM's buffers " << endl;
		clock_t stt = clock();
		//async(std::launch::async, &Engine::tFunction, this);
		for (Mesh* p : activeWorldMesh) {
			void* ptr = glMapNamedBuffer(p->VBO, GL_WRITE_ONLY);
			memcpy(ptr, &p->data[0], sizeof(float) * p->data.size());
			glUnmapNamedBuffer(p->VBO);
		}
		genFlag = false;
		cout << __FUNCTION__ << " UPDATE finished in " << difftime(clock(), stt) << "ms" << endl;
	}
}

void Engine::refreshAWMBuffers() {
	for (Mesh* p : activeWorldMesh) {
		glBindBuffer(GL_ARRAY_BUFFER, p->VBO);
		void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		cout << ptr << endl;
		memcpy(ptr,
			&p->data[0],
			sizeof(p->data));
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}
}

void Engine::generateSurroundingChunks(vector<Mesh*> aWM) {
		cout << __FUNCTION__ << " launched_" << endl;
		clock_t stt = clock(); int index = 0;
		for (int i = 0; i < kernelSize[0]; i++) {
			for (int j = 0; j < kernelSize[1]; j++) {
				for (int k = 0; k < kernelSize[2]; k++) {
					Mesh* tmp = activeWorldMesh.at(index);
					tmp->clears();
					generateMeshTriangles(tmp, vec3(i, j, k) - offsets + oldCP);
					tmp->loadDataFromTriangles();
					index++;
				}
			}
		}
		it = activeWorldMesh.begin();
		genFlag = true;
		cout << __FUNCTION__ << " finished in " << difftime(clock(), stt) << "ms." << endl;
	}

void Engine::genSurroundingChunks() {
		clock_t stt = clock();
		for (int i = 0; i < kernelSize[0]; i++) {
			for (int j = 0; j < kernelSize[1]; j++) {
				for (int k = 0; k < kernelSize[2]; k++) {
					activeWorldMesh.push_back(genChunk(vec3(i, j, k) - offsets + oldCP));
				}
			}
		}
		cout << __FUNCTION__ << " finished in " << difftime(clock(), stt) << "ms." << endl;
	}

Mesh* Engine::genChunk(vec3 position) {
		Mesh* meshPtr;
		meshPtr = new Mesh();
		generateMeshTriangles(meshPtr, position);
		setMesh(meshPtr);
		return meshPtr;
	}

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
	}

void Engine::generateMeshTriangles(Mesh* mesh) { generateMeshTriangles(mesh, vec3(0.0f)); };

void Engine::generateMeshTriangles(Mesh* mesh, vec3 chunkPosition) {
		vec3 position;
		for (unsigned int x = 0; x < DIM_AUG - 1; x++) {
			for (unsigned int y = 0; y < HEIGHT_AUG - 1; y++) {
				for (unsigned int z = 0; z < DIM_AUG - 1; z++) {
					position = vec3(x * STEP - x_offset, y * STEP - y_offset, z * STEP - z_offset);
					position += chunkPosition * vec3(DIM - 1);
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

void Engine::setMesh(Mesh* mesh) {
		mesh->loadDataFromTriangles();
		mesh->genBuffers();
		mesh->setBuffers();
		mesh->fillBuffers();
		mesh->clears();
	}

void Engine::initShaders() {
		pointShader = Shader("resources/shaders/vShaderSourcePoint.glsl", "resources/shaders/fShaderSourcePoint.glsl");
		meshShader = Shader("resources/shaders/vShaderSource3D.glsl", "resources/shaders/fShaderSource3D.glsl");
		shadowShader = Shader("resources/shaders/vShaderSourceShadow.glsl", "resources/shaders/fShaderSourceShadow.glsl");
	}

void Engine::renderMeshes(Entity entity, float ratio, vector<Mesh*> meshes) {
	for (int i = 0; i < meshes.size(); i++) {
		if (!meshes.at(i)->busy) {
			float aColor = (float)(i + 1) / meshes.size();
			renderMesh(entity, ratio, meshes.at(i), vec3(aColor, 1 - aColor, -aColor));
		}
	}
}

void Engine::renderMesh(Camera pov, float ratio, Mesh* mesh) {
		meshShader.use();
		meshShader.setVec3("objectColor", vec3(1.0f, 1.0f, 1.0f));
		meshShader.setVec3("lightColor", vec3(1.0f, 1.0f, 0.9f));
		meshShader.setVec3("lightPos", vec3(-25.0f, 50.0f, -25.0f));
		meshShader.setMat4("view", pov.lookAt());
		meshShader.setMat4("projection", perspective(radians(pov.getFOV()), ratio, 0.1f, 10000.0f));
		meshShader.setMat4("model", mat4(1.0f));

		mesh->render();
	}

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
	}