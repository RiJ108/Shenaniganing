#ifndef SHADER_HPP
#define SHADER_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <vector>

using namespace std;
using namespace glm;

class Shader{
public:
    unsigned int id;
        Shader();
        Shader(const char* vertexPath, const char* fragmentPath);
        void use();
        void setBool(const string &name, bool value) const;
        void setInt(const string &name, int value) const;
        void setFloat(const string &name, float value) const;
        void setVec2(const string &name, const vec2 &value) const;
        void setVec2(const string &name, float x, float y) const;
        void setVec3(const string &name, const vec3 &value) const;
        void setVec3(const string &name, float x, float y, float z) const;
        void setVec4(const string &name, const vec4 &value) const;
        void setVec4(const string &name, float x, float y, float z, float w);
        void setMat2(const string &name, const mat2 &mat) const;
        void setMat3(const string &name, const mat3 &mat) const;
        void setMat4(const string &name, const mat4 &mat) const;
        virtual ~Shader();
protected:
private:
};

#endif 
