#include "shader.hpp"

Shader::Shader(){
}

Shader::Shader(const char* vertexPath, const char* geometryPath, const char* fragmentPath){
    cout << __FUNCTION__ << "->vertex+geometry+fragment" << endl;
    cout << "  Sources files;\n\t" << vertexPath << "\n\t" << geometryPath << "\n\t" << fragmentPath << endl;
    string vertexCode;
    string geometryCode;
    string fragmentCode;
    ifstream vShaderFile;
    ifstream gShaderFile;
    ifstream fShaderFile;

    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions (ifstream::failbit | ifstream::badbit);
    gShaderFile.exceptions (ifstream::failbit | ifstream::badbit);
    fShaderFile.exceptions (ifstream::failbit | ifstream::badbit);
    try {
        // open files
        vShaderFile.open(vertexPath);
        gShaderFile.open(geometryPath);
        fShaderFile.open(fragmentPath);
        //cout << vertexPath << endl << geometryPath << endl << fragmentPath << endl;
        stringstream vShaderStream, gShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        gShaderStream << gShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();		  
        // close file handlers
        vShaderFile.close();
        gShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        geometryCode = gShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch(ifstream::failure e) {
        cout << __FUNCTION__ << "->ERROR::Unable to read the source files." << endl;
        exit(EXIT_FAILURE);
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* gShaderCode = geometryCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    GLuint vertex, geometry, fragment;
    GLint success;
    GLchar infoLog[512];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        cout << __FUNCTION__ << "->ERROR::Vertex shader compilation failed.\n\t" << infoLog << endl;
        exit(EXIT_FAILURE);
    }else cout << __FUNCTION__ << "->Vertex shader compilated." << endl;

    geometry = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometry, 1, &gShaderCode, NULL);
    glCompileShader(geometry);
    glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(geometry, 512, NULL, infoLog);
        cout << __FUNCTION__ << "->ERROR::Geometry shader compilation failed.\n\t" << infoLog << endl;
        exit(EXIT_FAILURE);
    }
    else cout << __FUNCTION__ << "->Geometry shader compiled." << endl;

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        cout << __FUNCTION__ << "->ERROR::Fragment shader compilation failed.\n\t" << infoLog << endl;
        exit(EXIT_FAILURE);
    }else cout << __FUNCTION__ << "->Fragment shader compiled." << endl;

    id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, geometry);
    glAttachShader(id, fragment);
    glLinkProgram(id);
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if(!success){
        glGetProgramInfoLog(id, 512, NULL, infoLog);
        cout << __FUNCTION__ << "->ERROR::Program linking failed.\n\t" << infoLog << endl;
        exit(EXIT_FAILURE);
    }else cout << __FUNCTION__ << "->Program linked." << endl;
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::Shader(const char* vertexPath, const char* fragmentPath){
    cout << __FUNCTION__ << "->vertex+fragment" << endl;
    cout << "  Sources files;\n\t" << vertexPath << "\n\t" << fragmentPath << endl;
    string vertexCode;
    string fragmentCode;
    ifstream vShaderFile;
    ifstream fShaderFile;

    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions (ifstream::failbit | ifstream::badbit);
    fShaderFile.exceptions (ifstream::failbit | ifstream::badbit);
    try {
        // open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        //cout << vertexPath << endl << fragmentPath << endl;
        stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();		  
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode   = vShaderStream.str();
        fragmentCode = fShaderStream.str();		
    }
    catch(ifstream::failure e) {
        cout << __FUNCTION__ << "->ERROR::Unable to read the source files." << endl;
        exit(EXIT_FAILURE);
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    GLuint vertex, fragment;
    GLint success;
    GLchar infoLog[512];
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        cout << __FUNCTION__ << "->ERROR::Vertex shader compilation failed.\n\t" << infoLog << endl;
        exit(EXIT_FAILURE);
    }else cout << __FUNCTION__ << "->Vertex shader compilated." << endl;
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        cout << __FUNCTION__ << "->ERROR::Fragment shader compilation failed.\n\t" << infoLog << endl;
        exit(EXIT_FAILURE);
    }else cout << __FUNCTION__ << "->Fragment shader compiled." << endl;
    id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glLinkProgram(id);
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if(!success){
        glGetProgramInfoLog(id, 512, NULL, infoLog);
        cout << __FUNCTION__ << "->ERROR::Program linking failed.\n\t" << infoLog << endl;
        exit(EXIT_FAILURE);
    }else cout << __FUNCTION__ << "->Program linked." << endl;
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

// activate the shader
// ------------------------------------------------------------------------
void Shader::use(){
    glUseProgram(id);
}
// utility uniform functions
// ------------------------------------------------------------------------
void Shader::setBool(const string &name, bool value) const{
    glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}
// ------------------------------------------------------------------------
void Shader::setInt(const string &name, int value) const{
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::setFloat(const string &name, float value) const{
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::setVec2(const string &name, const vec2 &value) const{
    glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}
void Shader::setVec2(const string &name, float x, float y) const{
    glUniform2f(glGetUniformLocation(id, name.c_str()), x, y);
}
// ------------------------------------------------------------------------
void Shader::setVec3(const string &name, const vec3 &value) const{
    glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}
void Shader::setVec3(const string &name, float x, float y, float z) const{
    glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
}
// ------------------------------------------------------------------------
void Shader::setVec4(const string &name, const vec4 &value) const{
    glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}
void Shader::setVec4(const string &name, float x, float y, float z, float w){
    glUniform4f(glGetUniformLocation(id, name.c_str()), x, y, z, w);
}
// ------------------------------------------------------------------------
void Shader::setMat2(const string &name, const mat2 &mat) const{
    glUniformMatrix2fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat3(const string &name, const mat3 &mat) const{
    glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat4(const string &name, const mat4 &mat) const{
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
Shader::~Shader(){

}