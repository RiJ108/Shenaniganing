#include "layout.hpp"

void Layout::addButton(vec2 aPos, vec2 aSize, vec3 aOnColor, vec3 aOffColor, string aName) {
	tmp = new Button;
	tmp->position = aPos;
	tmp->size = aSize;
	tmp->onColor = aOnColor;
	tmp->offColor = aOffColor;
	tmp->name = aName;
	tmp->setBoundary();
	buttons.push_back(*tmp);
};

void Layout::allocData() {
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
}

void Layout::resetButtonsStates() {
	for (unsigned int i = 0; i < buttons.size(); i++) {
		if (buttons.at(i).active = true) {
			buttons.at(i).active = false;
		}
		buttons.at(i).clicked = false;
	}
}

Button* Layout::getActiveButton() {
	for (unsigned int i = 0; i < buttons.size(); i++) {
		if (buttons.at(i).active)
			return &buttons.at(i);
	}
	return nullptr;
}

void Layout::updateBufferButtonColor(Button* aButtonPtr) {
	/*glBindBuffer(GL_ARRAY_BUFFER, activeLayoutPtr->getVBO());
                glBufferSubData(GL_ARRAY_BUFFER, ((7 * (float)buttonPtr->index) + 2) * sizeof(float), sizeof(float) * 3, &buttonPtr->onColor[0]);
                glUnmapBuffer(GL_ARRAY_BUFFER);*/
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	if (aButtonPtr->active) glBufferSubData(GL_ARRAY_BUFFER, ((7 * (float)aButtonPtr->index) + 2) * sizeof(float), sizeof(float) * 3, &aButtonPtr->onColor[0]);
	else glBufferSubData(GL_ARRAY_BUFFER, ((7 * (float)aButtonPtr->index) + 2) * sizeof(float), sizeof(float) * 3, &aButtonPtr->offColor[0]);
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

int Layout::getButtonsSize() {
	return buttons.size();
}

float* Layout::getData() {
	return data;
}

bool Layout::isActive() {
	return active;
}

void Layout::setActive(bool aState) {
	active = aState;
}

void Layout::setIndice(int aIndice) {
	indice = aIndice;
}

int Layout::getIndice() {
	return indice;
}

GLuint Layout::getVAO() {
	return VAO;
}

GLuint Layout::getVBO() {
	return VBO;
}

void Layout::setName(string aName) {
	name = aName;
} 

string Layout::getName() {
	return name;
}

void Layout::setAndFillBuffers() {
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

Layout::~Layout() {
	cout << __FUNCTION__ << endl;
	free(tmp);
	free(data);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}