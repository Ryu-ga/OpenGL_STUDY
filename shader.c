#include "shader.h"

char* read_file(const char* file_path) {
    FILE* fp = fopen(file_path, "r");
    if (!fp) {
        fprintf(stderr, "Failed to Read a File\n");
        return NULL;
    }
    
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);
    
    char* buffer = (char* )malloc(sizeof(char)*(size + 1));
    fread(buffer, sizeof(char), size, fp);
    fclose(fp);
    buffer[size] = 0;
    
    return buffer;
}

GLint TestShader(GLuint shader) {
    GLint result = GL_FALSE;
    int logLength;
    char* err_msg;
    
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
    
    if (logLength > 0) {
        err_msg = (char* )malloc(sizeof(char)*(logLength + 1));
        glGetShaderInfoLog(shader, logLength, NULL, err_msg);
        fprintf(stderr, "Failed to Compile Shader: %s\n", err_msg);
        free(err_msg);
    }
    return result;
}

GLint TestProgram(GLuint program) {
    GLint result = GL_FALSE;
    int logLength;
    char* err_msg;
    
    glGetProgramiv(program, GL_LINK_STATUS, &result);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        err_msg = (char* )malloc(sizeof(char)*(logLength + 1));
        glGetProgramInfoLog(program, logLength, NULL, err_msg);
        fprintf(stderr, "Failed to Compile Program: %s\n", err_msg);
        free(err_msg);
    }
    
    return result;
}

GLuint NewShaderFromFile(GLenum type, const char* path) {
	GLuint shader = glCreateShader(type);
	const char* shader_chr = read_file(path);
	glShaderSource(shader, 1, &shader_chr, NULL);

	glCompileShader(shader);
	if (TestShader(shader) == GL_FALSE) {
		glDeleteShader(shader);
		return 0;
	}

	free((void*)shader_chr);
	return shader;
}

GLuint NewProgramFromFile(const char* vertex_path, const char* fragment_path) {
	GLuint program = glCreateProgram();
	GLuint vertex = NewShaderFromFile(GL_VERTEX_SHADER, vertex_path);
	GLuint fragment = NewShaderFromFile(GL_FRAGMENT_SHADER, fragment_path);
	if (vertex)
		glAttachShader(program, vertex);
	if (fragment)
		glAttachShader(program, fragment);
	glLinkProgram(program);
	if (vertex)
		glDeleteShader(vertex);
	if (fragment)
		glDeleteShader(fragment);
	if (TestProgram(program) == GL_FALSE) {
		glDeleteProgram(program);
		return 0;
	}

	return program;
}
