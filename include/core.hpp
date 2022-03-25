#pragma once

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
#include "constant.hpp"
#include "layout.hpp"

using namespace std;
using namespace glm;

class Core {
public:
    void run();
    void initWindow();
    void runWindow();
private:
    //**Window section
    GLFWwindow* wHandler;
    GLFWmonitor* monitor;
    const GLFWvidmode* mode;
    string build = "Shenaniganing build 0.2 dev";
    ivec2 windowSizes = ivec2(1280, 720);

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