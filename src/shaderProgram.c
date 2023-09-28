#include "shaderProgram.h"

#include <glad/glad.h>

#include <stdio.h>
#include <stdlib.h>

struct ShaderProgram {
    unsigned int pId;
};

const char *readShaderFromFile(const char *file_name);

ShaderProgram *shaderProgram_create() {
    ShaderProgram *result = malloc(sizeof(struct ShaderProgram));
    result->pId = glCreateProgram();
    return result;
}

void shaderProgram_destroy(ShaderProgram *program) {
    glDeleteProgram(program->pId);
}

void shaderProgram_link(ShaderProgram *this) {
    glLinkProgram(this->pId);

    int isLinked;
    glGetShaderiv(this->pId, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE) {
        char logMsg[512];
        glGetProgramInfoLog(this->pId, 512, 0, logMsg);
        fprintf(stderr, "Program Linking error: \n%s\n", logMsg);
    }
}

void shaderProgram_use(ShaderProgram *this) {
    glUseProgram(this->pId);
}

void shaderProgram_attach(ShaderProgram *this, const char *file_name, unsigned int shader_type) {
    unsigned int shaderId = glCreateShader(shader_type);

    const char *src = readShaderFromFile(file_name);

    glShaderSource(shaderId, 1, &src, 0);
    glCompileShader(shaderId);

    int isCompiled;
    glGetShaderiv(this->pId, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        char logMsg[512];
        glGetProgramInfoLog(this->pId, 512, 0, logMsg);
        fprintf(stderr, "Error Compiling Shader: \nShaderType: %d\n%s\n", shader_type, logMsg);
    }

    glAttachShader(this->pId, shaderId);

    glDeleteShader(shaderId);
}

const char *readShaderFromFile(const char *file_name) {
    FILE *f = fopen(file_name, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET); /* same as rewind(f); */

    char *string = malloc(fsize + 1);
    fread(string, fsize, 1, f);
    fclose(f);

    string[fsize] = 0;
    return string;
}
