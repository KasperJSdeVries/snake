#include "shaderProgram.h"
#include "darray.h"

#include <glad/glad.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct uniform_var {
    char *var_name;
    int value;
};

struct ShaderProgram {
    unsigned int pId;
    struct uniform_var *uniform_vars;
};

const char *readShaderFromFile(const char *file_name);

ShaderProgram *shaderProgram_create() {
    ShaderProgram *result = malloc(sizeof(struct ShaderProgram));
    result->pId = glCreateProgram();
    result->uniform_vars = darray_create(struct uniform_var);
    return result;
}

void shaderProgram_destroy(ShaderProgram *program) {
    glDeleteProgram(program->pId);
    free(program);
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

void shaderProgram_add_uniform(ShaderProgram *this, const char *varName) {
    struct uniform_var new_uniform = {
            .var_name = strdup(varName),
            .value = glGetUniformLocation(this->pId, varName),
    };
    darray_push(this->uniform_vars, new_uniform);
}

void shaderProgram_set_float(ShaderProgram *this, const char *varName, float value) {
    for (int i = 0; i < darray_length(this->uniform_vars); ++i) {
        if (strcmp(this->uniform_vars[i].var_name, varName) == 0) {
            glUniform1f(this->uniform_vars[i].value, value);
            return;
        }
    }
}

void shaderProgram_set_vec3(ShaderProgram *this, const char *varName, vec3 value) {
    for (int i = 0; i < darray_length(this->uniform_vars); ++i) {
        if (strcmp(this->uniform_vars[i].var_name, varName) == 0) {
            glUniform3f(this->uniform_vars[i].value, value[0], value[1], value[2]);
            return;
        }
    }
}
void shaderProgram_set_vec4(ShaderProgram *this, const char *varName, vec4 value) {
    for (int i = 0; i < darray_length(this->uniform_vars); ++i) {
        if (strcmp(this->uniform_vars[i].var_name, varName) == 0) {
            glUniform4f(this->uniform_vars[i].value, value[0], value[1], value[2], value[3]);
            return;
        }
    }
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
