#pragma once

#define BUTTON_NUMBER_OF_FLOAT 7
#define BUTTON_SIZEOF BUTTON_NUMBER_OF_FLOAT*4
#define NUMBER_OF_LAYOUTS 1

class Window;

enum class State {mainMenu, inGame};

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