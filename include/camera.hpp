#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_angle.hpp>
#include "constant.hpp"

using namespace std;
using namespace glm;

class Camera {
public:
	Camera();
	void setDefault();
	mat4 lookAt();
	vec3 getPosition();
	vec3 getDirection();
	vec3 getUp();
	vec3 getFront();
	float getFOV();
	void key(int aKey);
	void updatePosition(float deltaTime);
	vec3 getFuturPos(float deltaTime);
	void setLastCursorPosition(vec2 aValue);
	void setFirstMouse(bool aValue);
	vec3 mouseMotion(vec2 value);

	vec3 position;
	vec3 speeds;
	vec3 accelarations;

private:
	vec3 up;
	vec3 front;
	vec3 speedsCap = vec3(10.0f);
	mat4 modelMatrix;
	vec2 lastCursorPosition;
	vec2 mouseOffsets;
	double yaw;
	double pitch;
	bool firstMouse;
	float FOV;
};