#pragma once

#include <vector>
#include "shader.hpp"
#include "constant.hpp"

using namespace std;

class Mesh {
public:
    unsigned int VAO, VBO;
    int nbrTriangles = 0;
    vector<float> data;
    vector<TRIANGLE> triangles;
    ~Mesh() {
        //cout << __FUNCTION__ << " adress:" << this << endl;
        data.clear();
        triangles.clear();
        glDeleteBuffers(1, &VAO);
        glDeleteBuffers(1, &VBO);
    };
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
    };
    void setBuffers() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 3 * 5 * CUBE_LIMIT, NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * data.size(), &data[0]);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glBindVertexArray(0);

        data.clear();
        triangles.clear();
    };
    void render() {
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, nbrTriangles);
        glBindVertexArray(0);
    };
};