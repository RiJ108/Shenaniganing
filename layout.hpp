#pragma once

#include <string>
#include <vector>
#include "glm/glm.hpp"

using namespace std;
using namespace glm;

typedef struct Button {
	vec2 position = vec2(0.0f);
	vec2 size = vec2(0.0f);
	vec3 color = vec3(0.0f);
	
	int sizeOfData = 2 + 2 + 3;
	bool state = false;
	string name = "";
	vec4 boundaries = vec4(0.0f);

	void setBoundary() {
		boundaries.x = position[0] - (size[0] / 2.0f);
		boundaries.y = position[0] + (size[0] / 2.0f);
		boundaries.z = position[1] - (size[1] / 2.0f);
		boundaries.w = position[1] + (size[1] / 2.0f);
	}
}Button;

class Layout {
public:
	// Add a button to the layout
	// parameter 1: the position of the center of the button(vec2)
	// parameter 2: the size of the button (vec2)
	// parameter 3: the color of the button (vec3)
	// parameter 4: the name/text of the button (string)
	// return void
	void addButton(vec2 aPos, vec2 aSize, vec3 aColor, string aName) {
		tmp = new Button;
		tmp->position = aPos;
		tmp->size = aSize;
		tmp->color = aColor;
		tmp->name = aName;
		tmp->setBoundary();
		buttons.push_back(*tmp);
	};

	// Return as an integer the number of buttons on the vector/the vector size
	int getButtonsSize() {
		return buttons.size();
	};

	void allocData() {
		if (buttons.size()) {
			data = (float*)malloc(sizeof(float) * 7 * buttons.size());
			unsigned int offset;
			for (int i = 0; i < buttons.size(); i++) {
				offset = i * 7;
				data[0 + offset] = buttons.at(i).position[0];
				data[1 + offset] = buttons.at(i).position[1];
				data[2 + offset] = buttons.at(i).color[0];
				data[3 + offset] = buttons.at(i).color[1];
				data[4 + offset] = buttons.at(i).color[2];
				data[5 + offset] = buttons.at(i).size[0];
				data[6 + offset] = buttons.at(i).size[1];
			}
			cout << __FUNCTION__ << " -> Allocation done, " << buttons.size() << " button(s) allocated." << endl;
		}
		else
			cout << __FUNCTION__ << " -> Allocation failed. No buttons in the layout." << endl;
	};

	float *data;
private:
	Button *tmp;
	vector <Button> buttons;
	

};