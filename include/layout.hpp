#pragma once
#include <map>
#include "shader.hpp"
#include <button.hpp>

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
	void addButton(vec2 aPos, vec2 aSize, vec3 aOnColor, vec3 aOffColor, string aName, string aText);
	// Fill the "data" variable (float*) containing the informations to pass to the shader for rendering the buttons of the layout
	void allocData();
	// Return the pointer of the "button" which as is active parameter set to true
	Button* getActiveButton();
	// Return the pointer of the "button" named "aButtonName"
	Button* getButtonPtr(string aButtonName);
	// Return as an integer the size of the "buttons" vector (Button) of the layout
	int getButtonsSize();
	// Return the ptr to the "data" variable (float*) containing the informations to pass to the shader for rendering the buttons of the layout
	float* getData();
	// Return the value of the "active" parameter (bool) of the layout
	bool isActive();
	// Set the "active" parameter (bool) of the layout to the given value of the parameter of the function
	void setActive(bool aState);
	// Set the "indice" parameter (int) of the layout to the given value of the parameter of the function
	void setIndice(int aIndice);
	// Return as an integer the value of the "indice" parameter of the layout
	int getIndice();
	// Return as a GLu integer the value of the vertex array object of the layout to render the buttons
	GLuint getVAO();
	// Return as a GLu integer the value of the vertex buffer object of the layout to render the buttons
	GLuint getVBO();
	// Generate, set and fill the vertex array object and vertex buffer object with the data variable
	void setAndFillBuffers();
	// Set the value of the "name" parameter (string) of the layout to the given value of the parameter of the function
	void setName(string aName);
	// Return as a string the value of the "name" parameter of the layout
	string getName();
	// Function that reset/set to false the "active" and "clicked" parameters of all the Button present in the "buttons" vector (vector<Button>)
	void resetButtonsStates();
	// Function that update the color values of the given button in the buffer
	void updateBufferButtonColor(Button* aButtonPtr);
	~Layout();

	vector <Button> buttons;
	string name = "layoutDefaultName";
	bool active = false;
	Button* tmp;
	float* data;
	int indice;
	GLuint VAO, VBO;

};