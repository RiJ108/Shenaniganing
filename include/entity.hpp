#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <displays.hpp>

using namespace glm;

class Entity {
public:
	mat4 lookAt() {
		return glm::lookAt(position, position + front, up);
	};

    void mouseMotion(vec2 cursorPos) {
        front = pov.mouseMotion(cursorPos);
    };

	void updatePosition(float deltaTime) {
		position += vel.x * deltaTime * front;
		vel = vec3(0.0f);
	};

	void keyEvent(int keyCode) {
        switch (keyCode) {
        case 87:
            vel.x = termV.x;
            break;
        case 83:
            vel.x = -termV.x;
            break;
        case 65:
            vel.z = -termV.z;
            break;
        case 68:
            vel.z = termV.z;
            break;
        case 32:
            vel.y = termV.y;
            break;
        case 340:
            vel.y = -termV.y;
            break;
        default:
            cout << __FUNCTION__ << "False call." << endl;
            break;
        }
	};

    void setDefault() {
        vec3 position = vec3(0.0f);
        vec3 front = vec3(1.0f, 0.0f, 0.0f);
    };

	vec3 position = vec3(0.0f);
	vec3 front = vec3(1.0f, 0.0f, 0.0f);
	vec3 up = vec3(0.0f, 1.0f, 0.0f);
	vec2 dim = vec2(2.0f, 0.5f);
	vec3 acc = vec3(0.0f);
	vec3 vel = vec3(0.0f);
	vec3 termV = vec3(10.0f);
    Camera pov;
    vec2 capsuleDim = vec2(1.0, 0.25f);

	float FOV = 100.0f;
private:
	
};