#pragma once

#include <layout.hpp>

class UI {
public:
	Shader shader_UI;
	vector<Layout> layouts;
	Layout* layoutPtr;

	Layout* getLayoutPtr(string aLayoutName) {
		for (auto it = begin(layouts); it != end(layouts); it++) {
			if (it->name == aLayoutName) {
				return &*it;
			}
		}
		return nullptr;
	}
	Layout* getActiveLayoutPtr() {
		for (auto it = begin(layouts); it != end(layouts); it++) {
			if (it->active) {
				return &*it;
			}
		}
		return nullptr;
	}
	void init(ivec2 WINDOW_SIZE) {
		shader_UI = Shader("resources/shaders/vShaderSourceUI.glsl", "resources/shaders/gShaderSourceUI.glsl", "resources/shaders/fShaderSourceUI.glsl");
	}
	void setLayouts(map<GLchar, Character> Characters, vec2 srcMidPoint) {
		//layoutPtr->buttons.at(0).functionPtr = &Window::functionPtrTest;

		layoutPtr = new Layout;
		layoutPtr->addButton(vec2(0.0f, 0.15f),
			vec2(0.5f, 0.25f),
			vec3(0.2f, 1.0f, 0.2f),
			vec3(0.2f, 0.2f, 0.2f),
			"Launch", "Launch");

		layoutPtr->addButton(vec2(0.0f, -0.1f),
			vec2(0.2f, 0.175f),
			vec3(1.0f, 0.2f, 0.2f),
			vec3(0.2f, 0.2f, 0.2f),
			"Exit", "Exit");

		for (unsigned int i = 0; i < layoutPtr->buttons.size(); i++) {
			layoutPtr->buttons.at(i).index = i;
			layoutPtr->buttons.at(i).setTextPixelLength(Characters);
			layoutPtr->buttons.at(i).setTextPixelWidth(Characters);
			layoutPtr->buttons.at(i).setTextPosition(srcMidPoint, layoutPtr->buttons.at(i).position);
		}

		layoutPtr->setActive(true);
		layoutPtr->setIndice(layouts.size());
		layoutPtr->setAndFillBuffers();
		layoutPtr->setName("mainMenu");

		layouts.push_back(*layoutPtr);
	}
};