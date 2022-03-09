#include "camera.hpp"
#include "displays.hpp"

Camera::Camera() {
    setDefault();
}

void Camera::setDefault() {
    position = vec3(0.0f, 5.0f, 3.0f);
    front = normalize(-position);
    up = vec3(0.0f, 1.0f, 0.0f);
    FOV = 100.0f;
    speeds = vec3(0.0f);
    speedsCap = 20.0f * vec3(1.0f, 1.0f, 0.75f);
    accelarations = vec3(0.0f);
    modelMatrix = mat4(1.0f);
    lastCursorPosition = vec2(0.0f);
    firstMouse = true;
    yaw = orientedAngle(vec3(-1.0f, 0.0f, 0.0f),front, up) * RADIAN_TO_DEGREE;
    pitch = -orientedAngle(vec3(0.0f, 0.0f, -1.0f), front, normalize(cross(up, front))) * RADIAN_TO_DEGREE;
}

void Camera::mouseMotion(double xMotion, double yMotion) {
    if (firstMouse) {
        lastCursorPosition.x = xMotion;
        lastCursorPosition.y = yMotion;
        firstMouse = false;
    }
    mouseOffsets.x = xMotion - lastCursorPosition.x;
    mouseOffsets.y = lastCursorPosition.y - yMotion;
    lastCursorPosition.x = xMotion;
    lastCursorPosition.y = yMotion;
    mouseOffsets *= 0.2f;
    yaw += mouseOffsets.x;
    pitch += mouseOffsets.y;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
    if (yaw > 360.0f) yaw -= 360.0f;
    if (yaw < -360.0f) yaw += 360.0f;

    vec3 direction = vec3(cos(radians(yaw)) * cos(radians(pitch)), sin(radians(pitch)), sin(radians(yaw)) * cos(radians(pitch)));
    this->front = normalize(direction);
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
        break;
    case 83:
        speeds.x = -speedsCap.x;
        break;
    case 65:
        speeds.z = -speedsCap.z;
        break;
    case 68:
        speeds.z = speedsCap.z;
        break;
    case 32:
        speeds.y = speedsCap.y;
        break;
    case 340:
        speeds.y = -speedsCap.y;
        break;
    default:
        cout << __FUNCTION__ << "False call." << endl;
        break;
    }
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
