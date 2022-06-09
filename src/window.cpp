#include "window.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

BOOL Window::init() {
    { //**Initialization of GLFW
        if (!glfwInit()) {
            cout << __FUNCTION__ << "->GLFW failed initialize." << endl;
            exit(EXIT_FAILURE);
        }
        else cout << __FUNCTION__ << "->GLFW initialized." << endl;

        //**Setting window
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        //**Creating the window
        wHandler = glfwCreateWindow(srcSize.x, srcSize.y, build.c_str(), NULL, NULL);
        if (wHandler == NULL) {
            cout << __FUNCTION__ << "->Failed to create GLFW window" << endl;
            glfwTerminate();
            exit(EXIT_FAILURE);
        }
        else cout << __FUNCTION__ << "->GLFW window created." << endl;
        glfwMakeContextCurrent(wHandler);

        //**Set callbacks
        glfwSetWindowUserPointer(wHandler, this);
        glfwSetKeyCallback(wHandler, keyCallback);
        glfwSetFramebufferSizeCallback(wHandler, framebuffer_size_callback);
        glfwSetCursorPosCallback(wHandler, mouse_callback_static);
        glfwSetMouseButtonCallback(wHandler, mouse_button_callback_static);

        //**Load glad
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            cout << __FUNCTION__ << "->Failed to initialize GLAD" << endl;
            exit(EXIT_FAILURE);
        }
        else   cout << __FUNCTION__ << "->GLAD initialized." << endl;

        //**OpenGL state
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        //**UI init
        ui.init(srcSize);
        ui.setLayouts(srcMidPoint);

        //**
        engine.initShaders();
        engine.genSurroundingChunks();
        player.position = vec3(0.0f, 0.0f, 0.0f);
    }
    //________________________________________________________________________________________________________________________________________
    //**TESTING**
    //________________________________________________________________________________________________________________________________________
    //**Depth Texture
    glGenFramebuffers(1, &depthMapFBO);

    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

BOOL Window::loop() {
    while (!glfwWindowShouldClose(wHandler)) {
        //**Process timing
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //**Clearing buffers
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        //**Finite-state machine calls
        F();
        M();
        G();

        //**Data rendering
        glDisable(GL_DEPTH_TEST);
        ui.renderText(build + " ms=" + to_string((int)(1000*deltaTime)), 10.0f, 10.0f, 0.25f, textColor);
        glEnable(GL_DEPTH_TEST);

        //**Refresh buffers and polling
        glfwSwapBuffers(wHandler);
        glfwPollEvents();
    }
    glfwTerminate();
    return true;
}

void Window::F() {
    switch (actualState) {
    case State::mainMenu:
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        layoutPtr = ui.getLayoutPtr("mainMenu");
        if (layoutPtr->getButtonPtr("Launch")->clicked) {
            nextState = State::inGame;
            layoutPtr->reset();
            glfwSetInputMode(wHandler, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            player.pov.setFirstMouse(true);
        }
        else if (layoutPtr->getButtonPtr("Exit")->clicked)
            glfwSetWindowShouldClose(wHandler, true);
        break;

    case State::inGame:
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        processKeyInputs();
        player.updatePosition(deltaTime);

        if (glfwGetKey(wHandler, 256) == GLFW_PRESS) {
            nextState = State::mainMenu;
            ui.getLayoutPtr("mainMenu")->setActive(true);
            glfwSetInputMode(wHandler, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            glfwSetCursorPos(wHandler, srcSize.x / 2.0f, srcSize.y / 2.0f);
            cursorPosX = srcSize.x / 2.0f;
            cursorPosY = srcSize.y / 2.0f;
        }
        break;
    }
}

void Window::M() {
    actualState = nextState;
}

void Window::G() {
    switch (actualState) {
    case State::mainMenu: {
        ui.renderLayout();
        ui.checkLayouts(vec2(cursorPosX, cursorPosY));
        break;
    }

    case State::inGame:
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        //1.Depth map rendering
        //glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        //glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        //glClear(GL_DEPTH_BUFFER_BIT);

        //engine.renderMeshForDepth(vec3(-25.0f, 50.0f, -25.0f), player.position, player.FOV, (float)SHADOW_WIDTH / SHADOW_HEIGHT);

        //glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //2.Normal render
        //glViewport(0, 0, srcSize.x, srcSize.y);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //glBindTexture(GL_TEXTURE_2D, depthMap);
        //engine.renderMesh(player, (float)srcSize.x / srcSize.y);

        engine.updateSurrounding(player);
        engine.renderActiveMeshs(player, (float)srcSize.x / srcSize.y);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        ui.renderText("Press escape to return to main menu", 10.0f, (srcMidPoint.y * 2) - 25.0f, 0.5f, textColor);
        ui.renderText("position.x = " + to_string(player.position.x), 10.0f, 40.0f, 0.3f, textColor);
        ui.renderText("position.y = " + to_string(player.position.y), 10.0f, 30.0f, 0.3f, textColor);
        ui.renderText("position.z = " + to_string(player.position.z), 10.0f, 20.0f, 0.3f, textColor);
        ui.renderText("front.x = " + to_string(player.front.x), 10.0f, 70.0f, 0.3f, textColor);
        ui.renderText("front.y = " + to_string(player.front.y), 10.0f, 60.0f, 0.3f, textColor);
        ui.renderText("front.z = " + to_string(player.front.z), 10.0f, 50.0f, 0.3f, textColor);
        break;
    }
}

void Window::processKeyInputs() {
    //**Camera interaction (set speeds vector)
    if (glfwGetKey(wHandler, 87) == GLFW_PRESS)
        player.keyEvent(87);
    if (glfwGetKey(wHandler, 83) == GLFW_PRESS)
        player.keyEvent(83);
    if (glfwGetKey(wHandler, 65) == GLFW_PRESS)
        player.keyEvent(65);
    if (glfwGetKey(wHandler, 68) == GLFW_PRESS)
        player.keyEvent(68);
    if (glfwGetKey(wHandler, 32) == GLFW_PRESS)
        player.keyEvent(32);
    if (glfwGetKey(wHandler, 340) == GLFW_PRESS)
        player.keyEvent(340);
    if (glfwGetKey(wHandler, 82) == GLFW_PRESS)
        player.setDefault();
}

void Window::keyCallback(GLFWwindow* aWHandler, int key, int scancode, int action, int mods) {
    //cout << __FUNCTION__ << "->" << key << " is " << action << endl;
    Window* windowPtr = (Window*)glfwGetWindowUserPointer(aWHandler);
}

void Window::framebuffer_size_callback(GLFWwindow* aWHandler, int width, int height) {
    Window* windowPtr = (Window*)glfwGetWindowUserPointer(aWHandler);
    windowPtr->srcSize.x = width;
    windowPtr->srcSize.y = height;
    glViewport(0, 0, width, height);
}

void Window::mouse_button_callback(GLFWwindow* aWHandler, int button, int action, int mods) {
    Window* windowPtr = (Window*)glfwGetWindowUserPointer(aWHandler);
    Layout* activeLayoutPtr = windowPtr->ui.getActiveLayoutPtr();
    if (activeLayoutPtr == nullptr)
        return;
    Button* activeButtonPtr = activeLayoutPtr->getActiveButton();
    if (action == GLFW_PRESS) {
        if (activeButtonPtr)
            activeButtonPtr->clicked = true;
    }
}

void Window::mouse_callback(GLFWwindow* aWHandler, double xpos, double ypos) {
    Window* windowPtr = (Window*)glfwGetWindowUserPointer(aWHandler);
    int width = windowPtr->srcSize.x;
    int height = windowPtr->srcSize.y;
    windowPtr->cursorPosX = 2.0f * (xpos - (width / 2.0)) / width;
    windowPtr->cursorPosY = -2.0f * (ypos - (height / 2.0)) / height;
    if (windowPtr->actualState == State::inGame)
        windowPtr->player.mouseMotion(vec2(xpos, ypos));
}