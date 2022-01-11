#include "window.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

BOOL Window::init() {
    //**Initialization of GLFW
    if (!glfwInit()) {
        cout << __FUNCTION__ << "->GLFW failed initialize." << endl;
        exit(EXIT_FAILURE);
    }
    else cout << __FUNCTION__ << "->GLFW initialized." << endl;

    //**Setting window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    //**Creating the window
    wHandler = glfwCreateWindow(srcWidth, srcHeight, build.c_str(), NULL, NULL);
    if (wHandler == NULL) {
        cout << __FUNCTION__ << "->Failed to create GLFW window" << endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    else cout << __FUNCTION__ << "->GLFW window created." << endl;
    //glfwSetWindowSizeLimits(wHandler, 990, 540, GLFW_DONT_CARE, GLFW_DONT_CARE);
    glfwMakeContextCurrent(wHandler);

    //**Set callbacks
    glfwSetWindowUserPointer(wHandler, this);
    //glfwSetKeyCallback(wHandler, keyCallback);
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

    //**Init and load FreeType (load glyphes)
    initFT();

    //**3D
    init3DShader();

    //--------------------------------------------------------------------------------------------------------
    initUI();
    setLayouts();

    /*objImporter.extract("resources/objects/Cube.obj");
    objImporter.loadMObject(&testObj);
    objImporter.loadMObject_(&testObj);
    testObj.initAndFillBuffers();
    testObj.initAndFillBuffers_();*/

    testBlock.setData();
    testBlock.gfsBuffers();

    return true;
}

BOOL Window::loop() {
        while (!glfwWindowShouldClose(wHandler)) {
        //**Process timing
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //**Clearing buffers
        glClearColor(0.025f, 0.025f, 0.025f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        //**Finite-state machine calls
        F();
        M();
        G();

        //**Data rendering
        glDisable(GL_DEPTH_TEST);
        renderText(shader_TXT, build + " ms=" + to_string((int)(deltaTime * 1000)), 10.0f, 10.0f, 0.25f, vec3(0.5, 0.8f, 0.2f));
        //renderText(shader_TXT, " x = " + to_string(cursorPosX), 10.0f, 30.0f, 0.25f, vec3(0.5, 0.8f, 0.2f));
        //renderText(shader_TXT, " y = " + to_string(cursorPosY), 10.0f, 20.0f, 0.25f, vec3(0.5, 0.8f, 0.2f));
        glEnable(GL_DEPTH_TEST);

        //**Refresh buffers and polling
        glfwSwapBuffers(wHandler);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO_TXT);
    glDeleteBuffers(1, &VBO_TXT);

    glfwTerminate();
    return true;
}

void Window::F() {
    switch (actualState) {
    case State::mainMenu:
        if (layouts.at(0).buttons.at(0).clicked) {
            nextState = State::inGame;
            getLayoutPtr("mainMenu")->setActive(false);
            getLayoutPtr("mainMenu")->resetButtonsStates();
            glfwSetInputMode(wHandler, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            pov.setLastCursorPosition(vec2(srcWidth/2.0f, srcHeight/2.0f));
            pov.setFirstMouse(true);
        }
        break;

    case State::inGame:
        processKeyInputs();
        if (glfwGetKey(wHandler, 256) == GLFW_PRESS) {
            nextState = State::mainMenu;
            getLayoutPtr("mainMenu")->setActive(true);
            glfwSetInputMode(wHandler, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        break;
    }
}

void Window::M() {
    actualState = nextState;
}

void Window::G() {
    switch (actualState) {
    case State::mainMenu:
        layoutPtr = getActiveLayoutPtr();
        if (layoutPtr == nullptr) {
            cout << __FUNCTION__ << "->###!! layoutPtr == nullptr !!###" << endl;
            return;
        }

        shader_UI.use();
        glBindVertexArray(layoutPtr->getVAO());
        glDrawArrays(GL_POINTS, 0, layoutPtr->getButtonsSize());
        glBindVertexArray(0);
        glDisable(GL_DEPTH_TEST);
        for (unsigned int i = 0; i < layoutPtr->buttons.size(); i++) {
            buttonPtr = &layoutPtr->buttons.at(i);
            renderText(shader_TXT, layoutPtr->buttons.at(i).name,
                (((buttonPtr->position.x + 1.0f) / 2.0f) * srcWidth) - (buttonPtr->nameLengthPxl * 0.5f * buttonPtr->scale),
                (((buttonPtr->position.y + 1.0f) / 2.0f) * srcHeight) - (textHeightPxl * 0.5f * buttonPtr->scale),
                buttonPtr->scale, vec3(1.0f));
        }
        glEnable(GL_DEPTH_TEST);

        //**Inputs processing
        checkUI();
        break;

    case State::inGame:
        //**Setting shader's uniform
        shader3D.use();
        shader3D.setVec3("lightPos", vec3(-2.0f * sin(currentFrame * 0.5f), 2.0f * cos(currentFrame * 0.05f), 2.0f));
        shader3D.setVec3("frontView", pov.getFront());

        shader3D.setMat4("view", pov.lookAt());
        shader3D.setVec3("viewPos", pov.getPosition());
        shader3D.setMat4("projection", perspective(radians(pov.getFOV()), (float)srcWidth / srcHeight, 0.1f, 10000.0f));
        shader3D.setMat4("model", mat4(1.0f));

        //**Render the MObject
        glBindVertexArray(testBlock.getVAO());
        glDrawArrays(GL_TRIANGLES, 0, testBlock.getNbrVertices());
        glBindVertexArray(0);

        renderText(shader_TXT, "Press escape to return to main menu", 10.0f, srcHeight - 25.0f, 0.5f, vec3(0.8f, 0.5f, 0.2f));
        renderText(shader_TXT, "Press R to reset pov", 10.0f, srcHeight - 45.0f, 0.3f, vec3(0.8f, 0.5f, 0.2f));
        break;
    }
}

void Window::processKeyInputs() {
    //**Camera interaction (set speeds vector)
    if (glfwGetKey(wHandler, 87) == GLFW_PRESS)
        pov.key(87);
    if (glfwGetKey(wHandler, 83) == GLFW_PRESS)
        pov.key(83);
    if (glfwGetKey(wHandler, 65) == GLFW_PRESS)
        pov.key(65);
    if (glfwGetKey(wHandler, 68) == GLFW_PRESS)
        pov.key(68);
    if (glfwGetKey(wHandler, 32) == GLFW_PRESS)
        pov.key(32);
    if (glfwGetKey(wHandler, 340) == GLFW_PRESS)
        pov.key(340);
    if (glfwGetKey(wHandler, 82) == GLFW_PRESS)
        pov.setDefault();

    //**Update camera position
    pov.updatePosition(deltaTime);
}

void Window::checkUI() {
    Layout* activeLayoutPtr;
    Button* buttonPtr;
    activeLayoutPtr = getActiveLayoutPtr();
    if (activeLayoutPtr == nullptr) {
        cout << __FUNCTION__ << "->###!! activeLayoutPtr == nullptr !!###" << endl;
        return;
    }
    for (unsigned int i = 0; i < activeLayoutPtr->getButtonsSize(); i++) {
        buttonPtr = &activeLayoutPtr->buttons.at(i);
        if (buttonPtr->isCursorPosIn(cursorPosX, cursorPosY)) {
            if (!buttonPtr->active) {
                //cout << "Cursor in " << buttonPtr->name << " button." << endl;
                buttonPtr->active = true;
                activeLayoutPtr->updateBufferButtonColor(buttonPtr);
            }
        }
        else {
            if (buttonPtr->active) {
                //cout << "Cursor out " << buttonPtr->name << " button." << endl;
                buttonPtr->active = false;
                activeLayoutPtr->updateBufferButtonColor(buttonPtr);
            }
        }
    }
}

void Window::setLayouts() {
    tmp = new Layout;
    tmp->addButton(vec2(0.0f, 0.15f),
        vec2(0.5f, 0.25f),
        vec3(0.2f, 1.0f, 0.2f),
        vec3(0.2f, 0.2f, 0.2f),
        "Launch");

    tmp->addButton(vec2(0.0f, -0.1f),
        vec2(0.2f, 0.175f),
        vec3(1.0f, 0.2f, 0.2f),
        vec3(0.2f, 0.2f, 0.2f),
        "Exit");

    for (unsigned int i = 0; i < tmp->buttons.size(); i++) {
        tmp->buttons.at(i).index = i;
        tmp->buttons.at(i).setStringLengthPxl(Characters);
        tmp->buttons.at(i).setButtonWidthPxl(srcWidth);
        tmp->buttons.at(i).setScale();
    }

    tmp->setActive(true);
    tmp->setIndice(layouts.size());
    tmp->setAndFillBuffers();
    tmp->setName("mainMenu");

    //**
    tmp->buttons.at(0).functionPtr = &Window::functionPtrTest;
    tmp->buttons.at(1).functionPtr = &Window::exitCallBack;

    layouts.push_back(*tmp);
}

Layout* Window::getLayoutPtr(string aName) {
    for (unsigned int i = 0; i < layouts.size(); i++) {
        if (layouts.at(i).getName() == aName)
            return &layouts.at(i);
    }
    return nullptr;
}

void Window::functionPtrTest(Window* aWindowPtr) {
    cout << __FUNCTION__ << endl;
}

void Window::exitCallBack(Window* aWindowPtr) {
    cout << __FUNCTION__ << endl;
    glfwSetWindowShouldClose(aWindowPtr->wHandler, true);

}

void Window::resetActiveLayout() {
    layoutPtr = getActiveLayoutPtr();
    for (unsigned int i = 0; i < layoutPtr->buttons.size(); i++) {
        layoutPtr->buttons.at(i).active = false;
        layoutPtr->buttons.at(i).clicked = false;
    }
}

Layout* Window::getActiveLayoutPtr() {
    for (unsigned int i = 0; i < layouts.size(); i++) {
        if (layouts.at(i).isActive())
            return &layouts.at(i);
    }
    return nullptr;
}

void Window::keyCallback(GLFWwindow* aWHandler, int key, int scancode, int action, int mods) {
    cout << __FUNCTION__ << "->" << key << " is " << action << endl;
}

void Window::framebuffer_size_callback(GLFWwindow* aWHandler, int width, int height) {
    Window* windowPtr = (Window*)glfwGetWindowUserPointer(aWHandler);
    windowPtr->srcWidth = width;
    windowPtr->srcHeight = height;
    glViewport(0, 0, width, height);
}

void Window::mouse_button_callback(GLFWwindow* aWHandler, int button, int action, int mods) {
    Window* windowPtr = (Window*)glfwGetWindowUserPointer(aWHandler);
    Layout* activeLayoutPtr = windowPtr->getActiveLayoutPtr();
    if (activeLayoutPtr == nullptr) {
        //cout << __FUNCTION__ << "->###!! activeLayoutPtr == nullptr !!###" << endl;
        return;
    }
    Button* activeButtonPtr = activeLayoutPtr->getActiveButton();
    if (action == GLFW_PRESS) {
        if (activeButtonPtr) {
            cout << __FUNCTION__ << " -> " << activeButtonPtr->name << " pressed." << endl;
            activeButtonPtr->functionPtr(windowPtr);
            activeButtonPtr->clicked = true;
        }
    }
    else if (action == GLFW_RELEASE) {

    }
}

void Window::clearClicked() {
    for (unsigned int i = 0; i < layouts.size(); i++) {
        layoutPtr = &layouts.at(i);
        for (unsigned int j = 0; j < layoutPtr->buttons.size(); j++) {
            layoutPtr->buttons.at(j).clicked = false;
        }
    }
}

void Window::mouse_callback(GLFWwindow* aWHandler, double xpos, double ypos) {
    Window* windowPtr = (Window*)glfwGetWindowUserPointer(aWHandler);
    int width = windowPtr->srcWidth;
    int height = windowPtr->srcHeight;
    windowPtr->cursorPosX = 2.0f * (xpos - (width / 2.0)) / width;
    windowPtr->cursorPosY = -2.0f * (ypos - (height / 2.0)) / height;
    if (windowPtr->actualState == State::inGame) {
        windowPtr->pov.mouseMotion(xpos, ypos);
    }
}

void Window::initUI() {
    shader_UI = Shader("resources/shaders/vShaderSourceUI.vs", "resources/shaders/gShaderSourceUI.gs", "resources/shaders/fShaderSourceUI.fs");
}

void Window::init3DShader() {
    //**Initiate 3D shader
    //shader3D = Shader("./resources/shaders/vShaderSource3D_OLD.vs", "resources/shaders/fShaderSource3D_OLD.fs");
    shader3D = Shader("./resources/shaders/vShaderSource3D.vs", "resources/shaders/fShaderSource3D.fs");
    shader3D.use();
    shader3D.setVec3("objectColor", vec3(1.0f, 1.0f, 1.0f));
    shader3D.setVec3("lightColor", vec3(1.0f, 1.0f, 0.9f));
    shader3D.setVec3("lightPos", vec3(-25.0f, 50.0f, -25.0f));
}

void Window::initFT() {
    //**Initiate 2D shader
    shader_TXT = Shader("resources/shaders/vShaderSourceTXT.vs", "resources/shaders/fShaderSourceTXT.fs");
    mat4 proj = ortho(0.0f, static_cast<float>(WINDOW_SIZE.x), 0.0f, static_cast<float>(WINDOW_SIZE.y));
    shader_TXT.use();
    glUniformMatrix4fv(glGetUniformLocation(shader_TXT.id, "projection"), 1, GL_FALSE, value_ptr(proj));

    if (FT_Init_FreeType(&ft)) {
        cout << __FUNCTION__ << "->ERROR::FREETYPE: Could not init FreeType Library" << endl;
        exit(EXIT_FAILURE);
    }
    if (FT_New_Face(ft, "resources/fonts/arial.ttf", 0, &face)) {
        cout << __FUNCTION__ << "->ERROR::FREETYPE: Failed to load font" << endl;
        exit(EXIT_FAILURE);
    }
    else {
        // set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, 48);

        // disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // load first 128 characters of ASCII set
        for (unsigned char c = 0; c < 128; c++) {
            // Load character glyph 
            if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
                cout << __FUNCTION__ << "->ERROR::FREETYTPE: Failed to load Glyph" << endl;
                continue;
            }
            // generate texture
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // now store character for later use
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)
            };
            Characters.insert(std::pair<char, Character>(c, character));
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    //**Destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    //**Configure VAO/VBO for texture quads
    glGenVertexArrays(1, &VAO_TXT);
    glGenBuffers(1, &VBO_TXT);
    glBindVertexArray(VAO_TXT);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TXT);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Window::renderText(Shader& shader, string text, float x, float y, float scale, vec3 color) {
    // activate corresponding render state	
    shader.use();
    glUniform3f(glGetUniformLocation(shader.id, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO_TXT);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO_TXT);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}