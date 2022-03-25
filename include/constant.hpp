#pragma once

#define LENGTH 50
#define WIDTH 50
#define HEIGHT 25
#define GRID_RES 2
#define CUBE_LIMIT ((LENGTH * GRID_RES) - 1) * ((HEIGHT * GRID_RES) - 1) * ((WIDTH * GRID_RES) - 1)

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
	vec3 coord;
	vec3 norm;
	double val;
} C_MIDPOINT;

typedef struct {
	vec3 coord;
	double val;
} C_POINT;

typedef struct {
	C_POINT points[8];		//Coord, value
	//C_MIDPOINT* midPoints[8];	//Coord, norm, value(count the number of normal calculated at this point for smoothing)
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

typedef struct Button {
	vec2 position = vec2(0.0f);
	vec2 size = vec2(0.0f);
	vec3 onColor = vec3(0.0f);
	vec3 offColor = vec3(0.0f);

	bool active = false;
	bool clicked = false;
	int sizeOfData = 2 + 2 + 3;
	string name = "buttonDefaultName";
	float nameLengthPxl = 0.0f;
	vec4 boundaries = vec4(0.0f);
	int index = 0;
	float buttonWidthPxl = 0.0f;
	float scale;

	void (*functionPtr)(Window* aWindowPtr);

	void setButtonWidthPxl(float aSrcWidth) {
		buttonWidthPxl = size.x * aSrcWidth * 0.5f;
	};

	void setScale() {
		scale = (0.9f * buttonWidthPxl) / nameLengthPxl;
	};

	void setStringLengthPxl(map<GLchar, Character> aCharMap) {
		nameLengthPxl = 0.0f;
		string::const_iterator c;
		for (c = name.begin(); c != name.end(); c++) {
			Character ch = aCharMap[*c];
			nameLengthPxl += (ch.Advance * (1.0f / 64.0f));
		}
	}

	void setBoundary() {
		boundaries.x = position[0] - (size[0] / 2.0f);
		boundaries.y = position[0] + (size[0] / 2.0f);
		boundaries.z = position[1] - (size[1] / 2.0f);
		boundaries.w = position[1] + (size[1] / 2.0f);
	}

	bool isCursorPosIn(float xPos, float yPos) {
		if (xPos >= boundaries.x && xPos <= boundaries.y) {
			if (yPos >= boundaries.z && yPos <= boundaries.w)
				return true;
			else
				return false;
		}
		else
			return false;
	}
}Button;