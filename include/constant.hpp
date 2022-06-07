#pragma once

#define DIM 50
#define HEIGHT 25
#define RES 2
#define STEP 1.0f/RES

#define DIM_AUG DIM * RES
#define HEIGHT_AUG HEIGHT * RES
#define CUBE_LIMIT (DIM_AUG - 1) * (HEIGHT_AUG - 1) * (DIM_AUG - 1)

#define BUTTON_NUMBER_OF_FLOAT 7
#define BUTTON_SIZEOF BUTTON_NUMBER_OF_FLOAT*4
#define NUMBER_OF_LAYOUTS 1
#define RADIAN_TO_DEGREE (180.0 / 3.141592653589793238463)

#include <map>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_angle.hpp>

using namespace std;
using namespace glm;

class Window;

enum class State { mainMenu, inGame };

//__Custom

typedef struct {
	vec3 coordinates;
	double value;
} C_POINT;

typedef struct {
	C_POINT apex[8];		//Coord, value
	int cubeIndex;			//Store the cube index of the latest marchingCube passage in the gridcell
} GRIDCELL;

typedef struct {
	vec3 points[3];
	vec3 norm;
}TRIANGLE;

//Holds all state information relevant to a character as loaded using FreeType
typedef struct Character {
	unsigned int TextureID; // ID handle of the glyph texture
	ivec2   Size;      // Size of glyph
	ivec2   Bearing;   // Offset from baseline to left/top of glyph
	unsigned int Advance;   // Horizontal offset to advance to next glyph
}Character;