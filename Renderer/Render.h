#pragma once

#include "Glew/glew.h"
#include "glm/glm.hpp"

struct vertex
{
	glm::vec4 pos;
};

struct geometry
{
	GLuint vao, vbo, ibo; //buffers
	GLuint size;		  //index count

};

struct Shader
{
	GLuint program;
};

geometry makeGeometry(vertex * verts, size_t vertCount,
	unsigned int * indices, size_t indexCount);

void freeGeometry(geometry &geo);

Shader makeShader(const char * vertSource,
	const char * fragSource);

void freeShader(Shader & shad);

void draw(const Shader & shad, const geometry & geo);

void setUniform(const Shader &shad, GLuint location, const glm::mat4 &value);

