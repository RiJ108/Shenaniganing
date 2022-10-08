#pragma once

#include <vector>
#include <memory>
#include "shader.hpp"
#include "constant.hpp"

using namespace std;

class Mesh {
public:
    void *bufferPtr, *bufferPtrb;
    unsigned int VAO, VBO, VAOb, VBOb;
    int nbrTriangles = 0, nbrTrianglesb = 0;
    bool busy = false, needSwap = false;
    ~Mesh() {
        glDeleteBuffers(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &VAOb);
        glDeleteBuffers(1, &VBOb);
    }

    void genBuffers() {
        glGenVertexArrays(1, &VAO);
        glGenVertexArrays(1, &VAOb);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &VBOb);
    }

    void fillPrimaireBufferData(vector<float> data) {
        memcpy(bufferPtr, &data[0], sizeof(float) * data.size());
        nbrTriangles = data.size() / 6;
    }

    void fillSecondBufferData(vector<float> data) {
        memcpy(bufferPtrb, &data[0], sizeof(float) * data.size());
        nbrTrianglesb = data.size() / 6;
    }

    void swapBuffers() {
        int tmp = VAO;
        VAO = VAOb;
        VAOb = tmp;

        tmp = VBO;
        VBO = VBOb;
        VBOb = tmp;

        void* tmpb = bufferPtr;
        bufferPtr = bufferPtrb;
        bufferPtrb = tmpb;

        nbrTriangles = nbrTrianglesb;
    }

    void setBuffers() {
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferStorage(GL_ARRAY_BUFFER, sizeof(float) * 6 * 3 * 5 * CUBE_LIMIT, 0, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glBindVertexArray(0);

        glBindVertexArray(VAOb);
        glBindBuffer(GL_ARRAY_BUFFER, VBOb);
        glBufferStorage(GL_ARRAY_BUFFER, sizeof(float) * 6 * 3 * 5 * CUBE_LIMIT, 0, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glBindVertexArray(0);
    }

    void fillBuffers(vector<float> data) {
        memcpy(glMapNamedBuffer(VBO, GL_WRITE_ONLY), &data[0], sizeof(float) * data.size());
        glUnmapNamedBuffer(VBO);
    }

    void clearBuffers() {
        glBindVertexArray(VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 6 * 3 * 5 * CUBE_LIMIT, NULL);
        glBindVertexArray(0);
    }

    void render() {
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, nbrTriangles);
        glBindVertexArray(0);
    }
};