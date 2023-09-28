#ifndef SNAKE_SHADERPROGRAM_H
#define SNAKE_SHADERPROGRAM_H

typedef struct ShaderProgram ShaderProgram;

ShaderProgram *shaderProgram_create();
void shaderProgram_destroy(ShaderProgram *program);

void shaderProgram_attach(ShaderProgram *this, const char *file_name, unsigned int shader_type);

void shaderProgram_link(ShaderProgram *this);

void shaderProgram_use(ShaderProgram *this);

#endif//SNAKE_SHADERPROGRAM_H
