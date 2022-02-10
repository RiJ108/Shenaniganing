#pragma once

#include "displays.hpp"
#include "shader.hpp"

#define WORLD_SIZE 1
#define GRID_SIZE 50
#define GRID_RESOLUTION 5
#define IN_CHUNK_VERTICES_OFFSET GRID_SIZE/2.0
#define GRID_SUB_SIZE GRID_SIZE * GRID_RESOLUTION

typedef struct grid {
	//float vertices[6 * (GRID_SUB_SIZE + 1) * (GRID_SUB_SIZE + 1)];
	//unsigned int indices[6 * (GRID_SUB_SIZE) * (GRID_SUB_SIZE)];
	GLuint VAO, VBO, EBO;
}grid;

class Terrain {
public:
	grid active;
private:
};