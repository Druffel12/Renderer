#pragma once

#include "glew/glew.h"
#include "glm/glm.hpp"

struct vertex
{
	glm::vec4 pos;
	glm::vec4 norm;
	glm::vec2 uv1;
};

struct geometry
{
	GLuint vao, vbo, ibo; // buffers
	GLuint size;          // index count
};

struct shader
{
	GLuint program;
};

struct texture
{
	GLuint handle;
	unsigned width, height, channels;
};

struct light
{
	glm::vec3 direction;
};

geometry loadGeometry(const char *filePath);
geometry makeGeometry(vertex *verts, size_t vertCount,
	unsigned int *indices, size_t indxCount);
void freeGeometry(geometry &geo);

shader loadShader(const char *vertPath, const char *fragPath);
shader makeShader(const char *vertSource,
	const char *fragSource);

void freeShader(shader &shad);

texture makeTexture(unsigned width, unsigned height, unsigned channels, const unsigned char *pixels);

void freeTexture(texture &tex);

texture loadTexture(const char *imagePath);

void draw(const shader &shad, const geometry &geo);

void setUniform(const shader &shad, GLuint location, const glm::mat4 &value);
void setUniform(const shader &shad, GLuint location, const texture &value, int textureSlot);
void setUniform(const shader &shad, GLuint location, const glm::vec3 &value);