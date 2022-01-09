#pragma once
#include <map>
#include "shader.hpp"
#include "constant.hpp"

using namespace std;
using namespace glm;

class Layout {
public:
	// Add a button to the layout
	// parameter 1: the position of the center of the button(vec2)
	// parameter 2: the size of the button (vec2)
	// parameter 3: the on color of the button (vec3)
	// parameter 4: the off color of the button (vec3)
	// parameter 5: the name/text of the button (string)
	// return void
	void addButton(vec2 aPos, vec2 aSize, vec3 aOnColor, vec3 aOffColor, string aName);
	// Fill the data variable (float*) containing the informations to pass to the shader for rendering the buttons of the layout
	void allocData();
	// Return the ptr of the button which as is active parameter set to true
	Button* getActiveButton();
	// Return as an integer the size of the buttons vector (Button) of the layout
	int getButtonsSize();
	// Return the ptr to the data variable (float*) containing the informations to pass to the shader for rendering the buttons of the layout
	float* getData();
	// Return the value if the parameter of the layout, active (bool)
	bool isActive();
	// Set the active parameter (bool) of the layout to the given value of the parameter of the function
	void setActive(bool aState);
	// Set the indice parameter (int) of the layout to the given value of the parameter of the function
	void setIndice(int aIndice);
	// Return as an integer the value of the parameter indice of the layout
	int getIndice();
	// Return as a GLu integer the value of the vertex array object of the layout to render the buttons
	GLuint getVAO();
	// Return as a GLu integer the value of the vertex buffer object of the layout to render the buttons
	GLuint getVBO();
	// Generate, set and fill the vertex array object and vertex buffer object with the data variable
	void setAndFillBuffers();
	~Layout();
	vector <Button> buttons;

private:
	string name = "layoutDefaultName";
	bool active = false;
	Button* tmp;
	float* data;
	int indice;
	GLuint VAO, VBO;
};