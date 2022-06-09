#pragma once

#include <layout.hpp>

class UI {
public:
	Shader shader_UI;
	Shader shader_TXT;
	GLuint VAO_TXT, VBO_TXT;
	FT_Library ft;
	FT_Face face;
	map<GLchar, Character> Characters;
	vector<Layout> layouts;
	Layout* layoutPtr;
	Button* buttonPtr;
	void renderLayout() {
		shader_UI.use();
		layoutPtr = getActiveLayoutPtr();
		if (layoutPtr != nullptr) {
			glBindVertexArray(layoutPtr->getVAO());
			glDrawArrays(GL_POINTS, 0, layoutPtr->getButtonsSize());
			glBindVertexArray(0);
			glDisable(GL_DEPTH_TEST);
			for (unsigned int i = 0; i < layoutPtr->buttons.size(); i++) {
				buttonPtr = &layoutPtr->buttons.at(i);
				renderText(buttonPtr->text, buttonPtr->textPosition.x, buttonPtr->textPosition.y, buttonPtr->textScale, vec3(1.0f));
			}
			glEnable(GL_DEPTH_TEST);
		}
	};
	void renderText(string text, float x, float y, float scale, vec3 color) {
		// activate corresponding render state	
		shader_TXT.use();
		glUniform3f(glGetUniformLocation(shader_TXT.id, "textColor"), color.x, color.y, color.z);
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
	};
	void checkLayouts(vec2 cursorPos) {
		layoutPtr = getActiveLayoutPtr();
		if (layoutPtr == nullptr) {
			cout << __FUNCTION__ << "->###!! activeLayoutPtr == nullptr !!###" << endl;
			return;
		}
		for (auto it = begin(layoutPtr->buttons); it != end(layoutPtr->buttons); it++) {
			if (it->isCursorPosIn(cursorPos.x, cursorPos.y)) {
				if (!it->active) {
					it->active = true;
					layoutPtr->updateBufferButtonColor(&*it);
				}
			}
			else {
				if (it->active) {
					it->active = false;
					layoutPtr->updateBufferButtonColor(&*it);
				}
			}
		}
	};
	Layout* getLayoutPtr(string aLayoutName) {
		for (auto it = begin(layouts); it != end(layouts); it++) {
			if (it->name == aLayoutName) {
				return &*it;
			}
		}
		return nullptr;
	}
	Layout* getActiveLayoutPtr() {
		for (auto it = begin(layouts); it != end(layouts); it++) {
			if (it->active) {
				return &*it;
			}
		}
		return nullptr;
	}
	void init(ivec2 WINDOW_SIZE) {
		shader_UI = Shader("resources/shaders/vShaderSourceUI.glsl", "resources/shaders/gShaderSourceUI.glsl", "resources/shaders/fShaderSourceUI.glsl");
		shader_TXT = Shader("resources/shaders/vShaderSourceTXT.glsl", "resources/shaders/fShaderSourceTXT.glsl");
		mat4 proj = ortho(0.0f, static_cast<float>(WINDOW_SIZE.x), 0.0f, static_cast<float>(WINDOW_SIZE.y));
		shader_TXT.use();
		glUniformMatrix4fv(glGetUniformLocation(shader_TXT.id, "projection"), 1, GL_FALSE, value_ptr(proj));
		initFT();
	}
	void initFT() {
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
	};
	void setLayouts(vec2 srcMidPoint) {
		//layoutPtr->buttons.at(0).functionPtr = &Window::functionPtrTest;

		layoutPtr = new Layout;
		layoutPtr->addButton(vec2(0.0f, 0.15f),
			vec2(0.5f, 0.25f),
			vec3(0.2f, 1.0f, 0.2f),
			vec3(0.2f, 0.2f, 0.2f),
			"Launch", "Launch");

		layoutPtr->addButton(vec2(0.0f, -0.1f),
			vec2(0.2f, 0.175f),
			vec3(1.0f, 0.2f, 0.2f),
			vec3(0.2f, 0.2f, 0.2f),
			"Exit", "Exit");

		int i = -1;
		for (auto it = begin(layoutPtr->buttons); it != end(layoutPtr->buttons); it++) {
			it->index = ++i;
			it->setTextPixelSizes(Characters);
			it->setTextPosition(srcMidPoint);
		}

		layoutPtr->setActive(true);
		layoutPtr->setIndice(layouts.size());
		layoutPtr->setAndFillBuffers();
		layoutPtr->setName("mainMenu");

		layouts.push_back(*layoutPtr);
	}
};