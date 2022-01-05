#include "window.hpp"

vec2 CURSOR_POS = vec2(-1.0f);

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

    //**Creating the window
    wHandler = glfwCreateWindow(srcWidth, srcHeight, build.c_str(), NULL, NULL);
    if (wHandler == NULL) {
        cout << __FUNCTION__ << "->Failed to create GLFW window" << endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    else cout << __FUNCTION__ << "->GLFW window created." << endl;
    glfwSetWindowSizeLimits(wHandler, 990, 540, GLFW_DONT_CARE, GLFW_DONT_CARE);
    glfwMakeContextCurrent(wHandler);
    
    //**Set callbacks
    glfwSetWindowUserPointer(wHandler, this);
    glfwSetFramebufferSizeCallback(wHandler, framebuffer_size_callback);
    glfwSetCursorPosCallback(wHandler, mouse_callback_static);         // Mouse motion callback
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
 
    return true;
}

BOOL Window::loop() {
    initUI();
    setLayouts();
    Layout* activeLayoutPtr;
    while (!glfwWindowShouldClose(wHandler)) {
        //**Process timing
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //**Clearing buffers
        glClearColor(0.025f, 0.025f, 0.025f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        //**Data rendering
        renderText(shader_TXT, build + " ms=" + to_string((int)(deltaTime * 1000)), 10.0f, 10.0f, 0.25f, vec3(0.5, 0.8f, 0.2f));
        renderText(shader_TXT, " x = " + to_string(CURSOR_POS.x), 10.0f, 30.0f, 0.25f, vec3(0.5, 0.8f, 0.2f));
        renderText(shader_TXT, " y = " + to_string(CURSOR_POS.y), 10.0f, 20.0f, 0.25f, vec3(0.5, 0.8f, 0.2f));

        //**UI rendering
        activeLayoutPtr = getActiveLayoutPtr();
        if (activeLayoutPtr != nullptr) {
            shader_UI.use();
            glBindVertexArray(activeLayoutPtr->getVAO());
            glDrawArrays(GL_POINTS, 0, activeLayoutPtr->getButtonsSize());
            glBindVertexArray(0);
        }

        //**Inputs processing
        checkUI();

        //**Refresh buffers and polling
        glfwSwapBuffers(wHandler);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO_TXT);
    glDeleteBuffers(1, &VBO_TXT);

    glfwTerminate();
    return true;
}

void Window::checkUI() {
    Layout* layoutActivePtr;
    Button* buttonPtr;
    layoutActivePtr = getActiveLayoutPtr();
    for (unsigned int i = 0; i < layoutActivePtr->getButtonsSize(); i++) {
        buttonPtr = &layoutActivePtr->buttons.at(i);
        if (buttonPtr->isCursorPosIn(cursorPosX, cursorPosY)) {
            if (!buttonPtr->active) {
                cout << "Cursor in " << buttonPtr->name << " button." << endl;
                glBindBuffer(GL_ARRAY_BUFFER, layoutActivePtr->getVBO());
                glBufferSubData(GL_ARRAY_BUFFER, ((7 * (float)i) + 2) * sizeof(float), sizeof(float) * 3, &buttonPtr->onColor[0]);
                glUnmapBuffer(GL_ARRAY_BUFFER);
                buttonPtr->active = true;
            }
        }
        else {
            if (buttonPtr->active) {
                cout << "Cursor out " << buttonPtr->name << " button." << endl;
                glBindBuffer(GL_ARRAY_BUFFER, layoutActivePtr->getVBO());
                glBufferSubData(GL_ARRAY_BUFFER, ((7 * (float)i) + 2) * sizeof(float), sizeof(float) * 3, &buttonPtr->offColor[0]);
                glUnmapBuffer(GL_ARRAY_BUFFER);
                buttonPtr->active = false;
            }
        }
    }
}

void Window::setLayouts() {
    tmp = new Layout;
    tmp->addButton(vec2(0.0f, 0.15f),
        vec2(0.5f, 0.3f),
        vec3(0.2f, 1.0f, 0.2f),
        vec3(0.2f, 0.2f, 0.2f),
        "Play Button");
    tmp->addButton(vec2(0.0f, -0.1f),
        vec2(0.2f, 0.1f),
        vec3(1.0f, 0.2f, 0.2f),
        vec3(0.2f, 0.2f, 0.2f),
        "Exit Button");
    tmp->setActive(true);
    tmp->setIndice(layouts.size());
    tmp->setAndFillBuffers();

    //**
    tmp->buttons.at(0).functionPtr = &Window::functionPtrTest;
    tmp->buttons.at(1).functionPtr = &Window::exitCallBack;

    layouts.push_back(*tmp);
}

void Window::functionPtrTest(Window* aWindowPtr) {
    cout << __FUNCTION__ << endl;
}

void Window::exitCallBack(Window* aWindowPtr) {
    cout << __FUNCTION__ << endl;
    glfwSetWindowShouldClose(aWindowPtr->wHandler, true);

}

Layout* Window::getActiveLayoutPtr() {
    for (unsigned int i = 0; i < layouts.size(); i++) {
        if (layouts.at(i).isActive())
            return &layouts.at(i);
    }
    return nullptr;
}

void Window::processButtons(int button, int action, int mods) {
    cout << __FUNCTION__ << endl;
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
    Button* activeButtonPtr = activeLayoutPtr->getActiveButton();
    if (activeButtonPtr) {
        cout << __FUNCTION__ << endl;
        cout << "  " << activeButtonPtr->name << endl;
        activeButtonPtr->functionPtr(windowPtr);
    }
}

void Window::mouse_callback(GLFWwindow* aWHandler, double xpos, double ypos) {
    Window* windowPtr = (Window*)glfwGetWindowUserPointer(aWHandler);
    int width = windowPtr->srcWidth;
    int height = windowPtr->srcHeight;
    windowPtr->cursorPosX = 2.0f * (xpos - (width / 2.0)) / width;
    windowPtr->cursorPosY = -2.0f * (ypos - (height / 2.0)) / height;
}

void Window::initUI() {
    shader_UI = Shader("resources/shader/vShaderSourceUI.vs", "resources/shader/gShaderSourceUI.gs", "resources/shader/fShaderSourceUI.fs");
}

void Window::initFT() {
    //**Initiate 2D shader
    shader_TXT = Shader("resources/shader/vShaderSourceTXT.vs", "resources/shader/fShaderSourceTXT.fs");
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