#include "layout.hpp"

void Layout::renderText(Shader shader_TXT, GLuint VAO, GLuint VBO, map<GLchar, Character> Characters, string text, float x, float y, float scale, vec3 color) {
	// activate corresponding render state
	glUniform3f(glGetUniformLocation(shader_TXT.id, "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	// iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		float xpos = x + ch.Bearing.x * scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;
		// update VBO for each character
		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};
		// render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
};
void Layout::render(Shader shader_UI, Shader shader_TXT, GLuint VAO_TXT, GLuint VBO_TXT, map<GLchar, Character> Characters) {
	// Geometry rendering
	shader_UI.use();
	glBindVertexArray(getVAO());
	glDrawArrays(GL_POINTS, 0, getButtonsSize());
	glBindVertexArray(0);

	// Text rendering
	shader_TXT.use();
	glDisable(GL_DEPTH_TEST);
	for (Button button : buttons)
		renderText(shader_TXT, VAO_TXT, VBO_TXT, Characters, button.text, button.textPosition.x, button.textPosition.y, button.textScale, vec3(1));
	glEnable(GL_DEPTH_TEST);
};
vector<Button> Layout::getButtons() {
	return buttons;
};
void Layout::reset() {
	active = false;
	for (auto it = begin(buttons); it != end(buttons); it++) {
		it->active = false;
		it->clicked = false;
		updateBufferButtonColor(&*it);
	}
}
Button* Layout::getButtonPtr(string aButtonName) {
	for (auto it = begin(buttons); it != end(buttons); it++) {
		if(it->name == aButtonName){
			return &*it;
		}
	}
	return nullptr;
}
void Layout::addButton(vec2 aPos, vec2 aSize, vec3 aOnColor, vec3 aOffColor, string aName, string aText) {
	tmp = new Button;
	tmp->position = aPos;
	tmp->size = aSize;
	tmp->onColor = aOnColor;
	tmp->offColor = aOffColor;
	tmp->name = aName;
	tmp->text = aText;
	tmp->setBoundary();
	buttons.push_back(*tmp);
};
void Layout::updateBufferButtonColor(Button* aButtonPtr) {
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	if (aButtonPtr->active) glBufferSubData(GL_ARRAY_BUFFER, ((BUTTON_NUMBER_OF_FLOAT * (float)aButtonPtr->index) + 2) * sizeof(float), sizeof(float) * 3, &aButtonPtr->onColor[0]);
	else glBufferSubData(GL_ARRAY_BUFFER, ((BUTTON_NUMBER_OF_FLOAT * (float)aButtonPtr->index) + 2) * sizeof(float), sizeof(float) * 3, &aButtonPtr->offColor[0]);
	glUnmapBuffer(GL_ARRAY_BUFFER);
}
void Layout::allocData() {
	if (buttons.size()) {
		for (Button button : buttons) {
			dataVec.push_back(button.position[0]);
			dataVec.push_back(button.position[1]);
			dataVec.push_back(button.offColor[0]);
			dataVec.push_back(button.offColor[1]);
			dataVec.push_back(button.offColor[2]);
			dataVec.push_back(button.size[0]);
			dataVec.push_back(button.size[1]);
		}
		cout << __FUNCTION__ << "->Allocation done, " << buttons.size() << " button(s) allocated." << endl;
	}
	else
		cout << __FUNCTION__ << "->###!! Allocation failed. No buttons in the layout. !!###" << endl;
}
void Layout::setName(string aName) {
	name = aName;
}
string Layout::getName() {
	return name;
}
Button* Layout::getActiveButton() {
	for (unsigned int i = 0; i < buttons.size(); i++) {
		if (buttons.at(i).active)
			return &buttons.at(i);
	}
	return nullptr;
}
int Layout::getButtonsSize() {
	return buttons.size();
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * dataVec.size(), &dataVec[0], GL_DYNAMIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
Layout::~Layout() {
	cout << __FUNCTION__ << "\t" << name << endl;
	free(tmp);
	dataVec.clear();
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}