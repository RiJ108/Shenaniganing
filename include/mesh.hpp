#pragma once

#include <vector>
#include <memory>
#include "shader.hpp"
#include "constant.hpp"

using namespace std;

class Mesh {
public:
    void* bufferPtr;
    unsigned int VAO, VBO;
    int nbrTriangles = 0;
    bool busy = false;
    vector<float> data;
    vector<TRIANGLE> triangles;
    ~Mesh() {
        data.clear();
        triangles.clear();
        glDeleteBuffers(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }

    void loadDataFromTriangles() {
        for (unsigned int i = 0; i < triangles.size(); i++) {
            for (int j = 0; j < 3; j++) {
                data.push_back(triangles.at(i).points[j].x);
                data.push_back(triangles.at(i).points[j].y);
                data.push_back(triangles.at(i).points[j].z);
                data.push_back(triangles.at(i).norm.x);
                data.push_back(triangles.at(i).norm.y);
                data.push_back(triangles.at(i).norm.z);
            }
        }
        nbrTriangles = data.size() / 6;
    }

    void genBuffers() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
    }

    void refreshBufferData() {
        memcpy(bufferPtr, &data[0], sizeof(float) * data.size());
    }

    void setBuffers() {
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferStorage(GL_ARRAY_BUFFER, sizeof(float) * 6 * 3 * 5 * CUBE_LIMIT, 0, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 3 * 5 * CUBE_LIMIT, NULL, GL_DYNAMIC_DRAW);
        //cout << glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(float) * 6 * 3 * 5 * CUBE_LIMIT, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT) << endl;

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glBindVertexArray(0);
    }

    void refillBuffers() {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 6 * 3 * 5 * CUBE_LIMIT, NULL);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * data.size(), &data[0]);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void fillBuffers(vector<float> passedData) {
        memcpy(glMapNamedBuffer(VBO, GL_WRITE_ONLY), &passedData[0], sizeof(float) * passedData.size());
        glUnmapNamedBuffer(VBO);
    }

    void fillBuffers() {
        memcpy(glMapNamedBuffer(VBO, GL_WRITE_ONLY), &data[0], sizeof(float) * data.size());
        glUnmapNamedBuffer(VBO);
    }

    void clearBuffers() {
        glBindVertexArray(VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 6 * 3 * 5 * CUBE_LIMIT, NULL);
        glBindVertexArray(0);
    }

    void clears() {
        data.clear();
        triangles.clear();
    }

    void render() {
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, nbrTriangles);
        glBindVertexArray(0);
    }
};