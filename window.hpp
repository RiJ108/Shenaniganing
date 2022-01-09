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
#include "layout.hpp"
#include "objImporter.hpp"
#include "mobject.hpp"
#include "constant.hpp"

using namespace std;
using namespace glm;


class Window {
public:
    BOOL init();
    BOOL loop();

    static void functionPtrTest(Window* aWindowPtr);
    static void exitCallBack(Window* aWindowPtr);

private:
    //**FSM section
    State actualState = State::mainMenu;
    State nextState = State::mainMenu;
    Layout* layoutPtr;
    Button* buttonPtr;
    float textHeightPxl = 35.0f;
    void F();
    void M();
    void G();

    //**3D section
    Shader shader3D;
    GLuint VAO_OBJ, VBO_OBJ;
    ObjImporter objImporter;
    MObject testObj;
    void init3DShader();

    //**Window section
    GLFWwindow* wHandler;
    string build = "Shenaniganing build 0.1 dev";
    ivec2 WINDOW_SIZE = ivec2(1280, 720);
    int* scrWidthPtr, * scrHeightPtr;
    int srcWidth = 1280, srcHeight = 720;
    int fpsCap = 60;
    float deltaTime = 0.0f, lastFrame = 0.0f, currentFrame = 0.0f;		// Time management
    float* cursorPosXPtr, * cursorPosYPtr;
    float cursorPosX = -1.0f, cursorPosY = -1.0f;
    static void keyCallback(GLFWwindow* aWHandler, int key, int scancode, int action, int mods);
    static void framebuffer_size_callback(GLFWwindow* aWHandler, int width, int height);
    static Window& getInstance() { static Window instance; return instance; }
    void mouse_callback(GLFWwindow* aWHandler, double xpos, double ypos);
    static void mouse_callback_static(GLFWwindow* aWHandler, double xpos, double ypos) { getInstance().mouse_callback(aWHandler, xpos, ypos); }
    void mouse_button_callback(GLFWwindow* aWHandler, int button, int action, int mods);
    static void mouse_button_callback_static(GLFWwindow* aWHandler, int button, int action, int mods) { getInstance().mouse_button_callback(aWHandler, button, action, mods); }

    //**Text section
    Shader shader_TXT;
    GLuint VAO_TXT, VBO_TXT;
    FT_Library ft;
    FT_Face face;
    map<GLchar, Character> Characters;
    void initFT();
    void renderText(Shader& shader, string text, float x, float y, float scale, vec3 color);

    //**Layout section
    Shader shader_UI;
    GLuint VAO_UI[NUMBER_OF_LAYOUTS];
    GLuint VBO_UI[NUMBER_OF_LAYOUTS];
    vector<Layout> layouts;
    Layout* tmp;
    void initUI();
    void setLayouts();
    Layout* getActiveLayoutPtr();
    Layout* getLayoutPtr(string aName);
    void clearClicked();
    void resetActiveLayout();
    void checkUI();
};