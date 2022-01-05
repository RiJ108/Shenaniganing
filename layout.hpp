#pragma once

#include "shader.hpp"

#define BUTTON_NUMBER_OF_FLOAT 7
#define BUTTON_SIZEOF BUTTON_NUMBER_OF_FLOAT*4

using namespace std;
using namespace glm;

typedef struct Button {
	vec2 position = vec2(0.0f);
	vec2 size = vec2(0.0f);
	vec3 onColor = vec3(0.0f);
	vec3 offColor = vec3(0.0f);
	
	bool state = false;
	int sizeOfData = 2 + 2 + 3;
	string name = "buttonDefaultName";
	vec4 boundaries = vec4(0.0f);

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

class Layout {
public:
	// Add a button to the layout
	// parameter 1: the position of the center of the button(vec2)
	// parameter 2: the size of the button (vec2)
	// parameter 3: the on color of the button (vec3)
	// parameter 4: the off color of the button (vec3)
	// parameter 5: the name/text of the button (string)
	// return void
	void addButton(vec2 aPos, vec2 aSize, vec3 aOnColor, vec3 aOffColor, string aName) {
		tmp = new Button;
		tmp->position = aPos;
		tmp->size = aSize;
		tmp->onColor = aOnColor;
		tmp->offColor = aOffColor;
		tmp->name = aName;
		tmp->setBoundary();
		buttons.push_back(*tmp);
	};

	void allocData() {
		if (buttons.size()) {
			data = (float*)malloc(sizeof(float) * BUTTON_NUMBER_OF_FLOAT * buttons.size());
			unsigned int offset;
			for (int i = 0; i < buttons.size(); i++) {
				offset = i * BUTTON_NUMBER_OF_FLOAT;
				data[0 + offset] = buttons.at(i).position[0];
				data[1 + offset] = buttons.at(i).position[1];
				data[2 + offset] = buttons.at(i).offColor[0];
				data[3 + offset] = buttons.at(i).offColor[1];
				data[4 + offset] = buttons.at(i).offColor[2];
				data[5 + offset] = buttons.at(i).size[0];
				data[6 + offset] = buttons.at(i).size[1];
			}
			cout << __FUNCTION__ << " -> Allocation done, " << buttons.size() << " button(s) allocated." << endl;
		}
		else
			cout << __FUNCTION__ << " -> Allocation failed. No buttons in the layout." << endl;
	};

	// Return as an integer the number of buttons on the vector/the vector size
	int getButtonsSize() {
		return buttons.size();
	};

	float* getData() {
		return data;
	};

	bool isActive() {
		return active;
	};

	void setActive(bool aState) {
		active = aState;
	};

	void setIndice(int aIndice) {
		indice = aIndice;
	};

	int getIndice() {
		return indice;
	};

	GLuint getVAO() {
		return VAO;
	};

	GLuint getVBO() {
		return VBO;
	};

	void setAndFillBuffers() {
		allocData();

		glGenBuffers(1, &VBO);
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, BUTTON_SIZEOF, 0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, BUTTON_SIZEOF, (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, BUTTON_SIZEOF, (void*)(5 * sizeof(float)));

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, BUTTON_SIZEOF * buttons.size(), NULL, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, BUTTON_SIZEOF * buttons.size(), data);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	~Layout() {
		cout << __FUNCTION__ << endl;
		free(tmp);
		free(data);
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}
	
	vector <Button> buttons;

private:
	string name = "layoutDefaultName";
	bool active = false;
	Button* tmp;
	float* data;
	int indice;
	GLuint VAO, VBO;
};