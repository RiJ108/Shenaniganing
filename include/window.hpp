#pragma once WINDOW_HPP

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include <iostream>
#include <map>
#include <string>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "ft2build.h"
#include FT_FREETYPE_H

#include "shader.hpp"
#include "ui.hpp"
#include "layout.hpp"
#include "objImporter.hpp"
#include "engine.hpp"
#include "mobject.hpp"
#include "constant.hpp"
#include "camera.hpp"
#include "marchingCube.hpp"
#include "entity.hpp"

using namespace std;
using namespace glm;

class Window {
public:
    BOOL init();
    BOOL loop();

    ~Window() {
        if (!layoutPtr)free(layoutPtr);
        if (!wHandler)free(wHandler);
    }

private:
    //**FSM section
    State actualState = State::mainMenu;
    State nextState = State::mainMenu;
    Layout* layoutPtr;
    void F();
    void M();
    void G();

    //**3D section
    unsigned int depthMapFBO;
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMap;

    Camera lightSourcePov;
    Engine engine;
    Entity player;
    void processKeyInputs();

    //**Window section
    GLFWwindow* wHandler;
    GLFWmonitor* monitor;
    const GLFWvidmode* mode;
    string build = "Shenaniganing build 0.22 dev-" + to_string(time(0));
    vec2 srcSize = vec2(1280, 720);
    vec2 srcMidPoint = srcSize * 0.5f;
    int fpsCap = 60;
    float deltaTime = 0.0f, lastFrame = 0.0f, currentFrame = 0.0f;		// Time management
    float cursorPosX = -1.0f, cursorPosY = -1.0f;
    static void keyCallback(GLFWwindow* aWHandler, int key, int scancode, int action, int mods);
    static void framebuffer_size_callback(GLFWwindow* aWHandler, int width, int height);
    static Window& getInstance() { static Window instance; return instance; }
    void mouse_callback(GLFWwindow* aWHandler, double xpos, double ypos);
    static void mouse_callback_static(GLFWwindow* aWHandler, double xpos, double ypos) { getInstance().mouse_callback(aWHandler, xpos, ypos); }
    void mouse_button_callback(GLFWwindow* aWHandler, int button, int action, int mods);
    static void mouse_button_callback_static(GLFWwindow* aWHandler, int button, int action, int mods) { getInstance().mouse_button_callback(aWHandler, button, action, mods); }

    //**Ui section
    UI ui;
    vec3 textColor = vec3(1.0f);
};