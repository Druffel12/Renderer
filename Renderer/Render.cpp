#include "render.h"

#include "glm/gtc/type_ptr.hpp"

#define STB_IMAGE_IMPLEMENTATION 1
#include "stb/stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobjloader/tiny_obj_loader.h"

#include <vector>
#include <cstdio>
#include <cassert>

geometry loadGeometry(const char *filePath)
{
	using namespace tinyobj;

	// contains all data for all vertex attributes loaded from the file
	attrib_t vertexAttributes;

	// enumeration of all shapes in obj file
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string error;

	bool success = LoadObj(&vertexAttributes, &shapes, &materials, &error, filePath);

	if (!error.empty())
	{
		fprintf(stderr, "%s", error.c_str());
	}
	if (!success || shapes.size() < 1)
	{
		return {};
	}

	std::vector<vertex> vertices;
	std::vector<unsigned int> indices;

	// get mesh data
	size_t offset = 0;
	for (size_t i = 0; i < shapes[0].mesh.num_face_vertices.size(); ++i)
	{
		// number of vertices for current face
		unsigned char faceVertices = shapes[0].mesh.num_face_vertices[i];

		assert(faceVertices == 3);

		// iterate over vertices used to form current face
		for (unsigned char j = 0; j < faceVertices; ++j)
		{
			tinyobj::index_t idx = shapes[0].mesh.indices[offset + j];

			tinyobj::real_t vx = vertexAttributes.vertices[3 * idx.vertex_index + 0];
			tinyobj::real_t vy = vertexAttributes.vertices[3 * idx.vertex_index + 1];
			tinyobj::real_t vz = vertexAttributes.vertices[3 * idx.vertex_index + 2];

			tinyobj::real_t nx = vertexAttributes.normals[3 * idx.normal_index + 0];
			tinyobj::real_t ny = vertexAttributes.normals[3 * idx.normal_index + 1];
			tinyobj::real_t nz = vertexAttributes.normals[3 * idx.normal_index + 2];

			tinyobj::real_t tx = vertexAttributes.texcoords[2 * idx.texcoord_index + 0];
			tinyobj::real_t ty = vertexAttributes.texcoords[2 * idx.texcoord_index + 1];

			vertices.push_back(vertex{ { vx, vy, vz, 1 },{ nx, ny, nz, 1 },{ tx, ty } });
			indices.push_back(3 * i + j);
		}
		offset += faceVertices;
	}

	return makeGeometry(&vertices[0], vertices.size(), &indices[0], shapes[0].mesh.indices.size());
}

geometry makeGeometry(vertex *verts, size_t vertCount, unsigned int *indicies, size_t indxCount)
{
	// create an instance of geometry
	geometry newGeo = {};
	newGeo.size = indxCount;

	// generate buffers
	glGenVertexArrays(1, &newGeo.vao);
	glGenBuffers(1, &newGeo.vbo);
	glGenBuffers(1, &newGeo.ibo);

	// bind buffers
	glBindVertexArray(newGeo.vao);
	glBindBuffer(GL_ARRAY_BUFFER, newGeo.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, newGeo.ibo);

	// populate buffers
	glBufferData(GL_ARRAY_BUFFER, vertCount * sizeof(vertex), verts, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indxCount * sizeof(unsigned int), indicies, GL_STATIC_DRAW);

	// describe vertex data
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)16);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)32);

	// unbind buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// return geometry
	return newGeo;
}

void freeGeometry(geometry &geo)
{
	glDeleteBuffers(1, &geo.vbo);
	glDeleteBuffers(1, &geo.ibo);
	glDeleteVertexArrays(1, &geo.vao);

	geo = {};
}

bool checkShader(shader &shad)
{
	GLint status = GL_FALSE;
	glGetProgramiv(shad.program, GL_LINK_STATUS, &status);

	if (status != GL_TRUE)
	{
		GLint logLength = 0;
		glGetProgramiv(shad.program, GL_INFO_LOG_LENGTH, &logLength);
		GLchar *log = new GLchar[logLength];
		glGetProgramInfoLog(shad.program, logLength, 0, log);

		fprintf(stderr, "%s\n", log);

		delete[] log;
	}

	return status == GL_TRUE;
}

bool checkSubShader(GLuint subshader)
{
	GLint status = GL_FALSE;
	glGetShaderiv(subshader, GL_COMPILE_STATUS, &status);

	if (status != GL_TRUE)
	{
		GLint logLength = 0;
		glGetShaderiv(subshader, GL_INFO_LOG_LENGTH, &logLength);
		GLchar *log = new GLchar[logLength];
		glGetShaderInfoLog(subshader, logLength, 0, log);

		fprintf(stderr, "%s\n", log);

		delete[] log;
	}

	return status == GL_TRUE;
}

shader loadShader(const char *vertPath, const char *fragPath)
{
	const size_t FILE_COUNT = 2;

	const char *filePaths[FILE_COUNT];
	filePaths[0] = vertPath;
	filePaths[1] = fragPath;

	char *fileContents[FILE_COUNT];

	for (size_t i = 0; i < FILE_COUNT; ++i)
	{
		FILE *currentFile = nullptr;
		fopen_s(&currentFile, filePaths[i], "r");
		assert(currentFile != nullptr && "Path to file cannot be null!");

		fseek(currentFile, 0L, SEEK_END);
		long fileLength = ftell(currentFile);

		fileContents[i] = new char[fileLength + 1];
		fileContents[i][fileLength] = '\0';
		rewind(currentFile);

		char *writeHead = fileContents[i];
		while (feof(currentFile) == 0)
		{
			fgets(writeHead, fileLength, currentFile);
			writeHead = fileContents[i] + strlen(fileContents[i]);
		}

		fclose(currentFile);
	}

	// hard code compilation for now, but would like to allow for other types
	// of shaders in the future (i.e. geometry, tesselation, etc. shaders)
	shader newShad = makeShader(fileContents[0], fileContents[1]);

	for (size_t i = 0; i < FILE_COUNT; ++i)
	{
		delete fileContents[i];
	}

	return newShad;
}

shader makeShader(const char *vertSource, const char *fragSource)
{
	// make the shader object
	shader newShad = {};
	newShad.program = glCreateProgram();

	// create shaders
	GLuint vert = glCreateShader(GL_VERTEX_SHADER);
	GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);

	// compile shaders
	glShaderSource(vert, 1, &vertSource, 0);
	glShaderSource(frag, 1, &fragSource, 0);
	glCompileShader(vert);
	glCompileShader(frag);
	assert(checkSubShader(vert));
	assert(checkSubShader(frag));

	// attach shaders
	glAttachShader(newShad.program, vert);
	glAttachShader(newShad.program, frag);

	// link shaders
	glLinkProgram(newShad.program);

	assert(checkShader(newShad));

	// delete shaders
	glDeleteShader(vert);
	glDeleteShader(frag);

	// return the shader object
	return newShad;
}

void freeShader(shader &shad)
{
	glDeleteProgram(shad.program);
	shad = {};
}

texture makeTexture(unsigned width, unsigned height, unsigned channels, const unsigned char *pixels)
{
	GLenum oglFormat = GL_RGBA;
	switch (channels)
	{
	case 1:
		oglFormat = GL_RED;
		break;
	case 2:
		oglFormat = GL_RG;
		break;
	case 3:
		oglFormat = GL_RGB;
		break;
	case 4:
		oglFormat = GL_RGBA;
		break;
	}

	texture retval = { 0, width, height, channels };

	glGenTextures(1, &retval.handle);
	glBindTexture(GL_TEXTURE_2D, retval.handle);

	glTexImage2D(GL_TEXTURE_2D, 0, oglFormat, width, height, 0, oglFormat, GL_UNSIGNED_BYTE, pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	return retval;
}

void freeTexture(texture &tex)
{
	glDeleteTextures(1, &tex.handle);
	tex = {};
}

texture loadTexture(const char *imagePath)
{
	int imageWidth, imageHeight, imageFormat;
	unsigned char *rawPixelData = nullptr;

	texture newTexture = {};

	stbi_set_flip_vertically_on_load(true);
	rawPixelData = stbi_load(imagePath,
		&imageWidth,
		&imageHeight,
		&imageFormat,
		STBI_default);

	assert(rawPixelData != nullptr && "Image failed to load.");
	newTexture = makeTexture(imageWidth, imageHeight, imageFormat, rawPixelData);
	stbi_image_free(rawPixelData);

	return newTexture;
}

void draw(const shader &shad, const geometry &geo)
{
	glUseProgram(shad.program);
	glBindVertexArray(geo.vao);

	glDrawElements(GL_TRIANGLES, geo.size, GL_UNSIGNED_INT, 0);
}

void setUniform(const shader &shad, GLuint location, const glm::mat4 &value)
{
	glProgramUniformMatrix4fv(shad.program, location, 1, GL_FALSE, glm::value_ptr(value));
}

void setUniform(const shader &shad, GLuint location, const texture &value, int textureSlot)
{
	glActiveTexture(GL_TEXTURE0 + textureSlot);
	glBindTexture(GL_TEXTURE_2D, value.handle);
	glProgramUniform1i(shad.program, location, textureSlot);
}

void setUniform(const shader &shad, GLuint location, const glm::vec3 &value)
{
	glProgramUniform3fv(shad.program, location, 1, glm::value_ptr(value));
}