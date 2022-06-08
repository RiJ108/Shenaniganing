#pragma once

#include <constant.hpp>

class Button {
public:
	vec2 position = vec2(0.0f);
	vec2 size = vec2(0.0f);
	vec3 onColor = vec3(0.0f);
	vec3 offColor = vec3(0.0f);

	bool active = false;
	bool clicked = false;
	bool locked = false;
	int sizeOfData = 2 + 2 + 3;
	string name = "defaultName";

	string text = "defaultText";
	vec2 textPixelSize = vec2(0.0f);
	vec2 textPosition = vec2(0.0f);
	float textScale = 1.0f;

	vec4 boundaries = vec4(0.0f);
	int index = 0;

	void (*functionPtr)(Window* aWindowPtr);

	void setTextPosition(vec2 srcMidPoint) {
		textPosition = srcMidPoint;
		textPosition -= textPixelSize * 0.5f * textScale;
		textPosition += position * srcMidPoint;
	}

	void setTextPixelSizes(map<GLchar, Character> aCharMap) {
		textPixelSize = vec2(0.0f);
		string::const_iterator c;
		for (c = text.begin(); c != text.end(); c++) {
			Character ch = aCharMap[*c];
			textPixelSize.x += (ch.Advance * (1.0f / 64.0f));
			if (textPixelSize.y < ch.Size.x)
				textPixelSize.y = ch.Size.x;
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
};