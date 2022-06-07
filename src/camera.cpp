#include "camera.hpp"
#include "displays.hpp"

Camera::Camera() {
    setDefault();
}

void Camera::setDefault() {
    position = vec3(0.0f, 10.0f, 3.0f);
    front = normalize(-position);
    up = vec3(0.0f, 1.0f, 0.0f);
    FOV = 100.0f;
    speeds = vec3(0.0f);
    //speedsCap = 20.0f * vec3(1.0f, 1.0f, 0.75f);
    accelarations = vec3(0.0f);
    modelMatrix = mat4(1.0f);
    lastCursorPosition = vec2(0.0f);
    firstMouse = true;
    pitch = asin(front.y);
    yaw = acos(front.x) / pitch;
}

vec3 Camera::mouseMotion(vec2 value) {
    if (firstMouse) {
        lastCursorPosition.x = value.x;
        lastCursorPosition.y = value.y;
        firstMouse = false;
    }
    mouseOffsets.x = value.x - lastCursorPosition.x;
    mouseOffsets.y = lastCursorPosition.y - value.y;
    lastCursorPosition.x = value.x;
    lastCursorPosition.y = value.y;
    mouseOffsets *= 0.2f;
    yaw += mouseOffsets.x;
    pitch += mouseOffsets.y;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
    if (yaw > 360.0f) yaw -= 360.0f;
    if (yaw < -360.0f) yaw += 360.0f;

    //vec3 direction = vec3(cos(radians(yaw)) * cos(radians(pitch)), sin(radians(pitch)), sin(radians(yaw)) * cos(radians(pitch)));
    this->front = normalize(vec3(cos(radians(yaw)) * cos(radians(pitch)), sin(radians(pitch)), sin(radians(yaw)) * cos(radians(pitch))));
    return this->front;
}

void Camera::setLastCursorPosition(vec2 aValue) {
    lastCursorPosition = aValue;
}

void Camera::setFirstMouse(bool aValue) {
    firstMouse = aValue;
}

void Camera::key(int aKey) {
    switch (aKey) {
    case 87:
        speeds.x = speedsCap.x;
        //accelarations.x = 5.0f;
        break;
    case 83:
        speeds.x = -speedsCap.x;
        //accelarations.x = -5.0f;
        break;
    case 65:
        speeds.z = -speedsCap.z;
        //accelarations.z = -5.0f;
        break;
    case 68:
        speeds.z = speedsCap.z;
        //accelarations.z = 5.0f;
        break;
    case 32:
        speeds.y = speedsCap.y;
        //accelarations.y = 5.0f;
        break;
    case 340:
        speeds.y = -speedsCap.y;
        //accelarations.y = -5.0f;
        break;
    default:
        cout << __FUNCTION__ << "False call." << endl;
        break;
    }
}

vec3 Camera::getFuturPos(float deltaTime) {
    return position + deltaTime * rotate(speeds, orientedAngle(vec3(1.0f, 0.0f, 0.0f), front, up), up);
}

vec3 Camera::getDirection() {
    return rotate(speeds, orientedAngle(vec3(1.0f, 0.0f, 0.0f), front, up), up);
}

void Camera::updatePosition(float deltaTime) {
    position += deltaTime * rotate(speeds, orientedAngle(vec3(1.0f, 0.0f, 0.0f), front, up), up);
    speeds = vec3(0.0f);
}

mat4 Camera::lookAt() {
	return glm::lookAt(position, position + front, up);
}

vec3 Camera::getPosition() {
	return position;
}

vec3 Camera::getUp() {
	return up;
}

vec3 Camera::getFront() {
	return front;
}

float Camera::getFOV() {
	return FOV;
}
