#ifndef __SHADER_H
#define __SHADER_H

#include "common.h"
#include <string.h>

char* read_file(const char* file_path);

GLint TestShader(GLuint shader);
GLint TestProgram(GLuint program);

GLuint NewShaderFromChar(GLenum type, const char* shader);
GLuint NewShaderFromFile(GLenum type, const char* path);

GLuint NewProgramFromChar(const char* vertex, const char* fragment);
GLuint NewProgramFromFile(const char* vertex_path, const char* fragment_path);

#endif
