#pragma once

#include "context.h"

#include <cstdio>

#include "glew/glew.h"
#include "glfw/glfw3.h"

// Error callback called by OpenGL whenever a problem occurs (vendor-dependent)
void APIENTRY errorCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
	GLsizei length, const GLchar *message,
	const void *userParam)
{
	fprintf(stderr, "%s\n", message);
}

bool context::init(int width, int height, const char *title)
{
	// init glfw
	glfwInit();

	window = glfwCreateWindow(width, height, title,
		nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// init glew
	glewInit();

	// print out some diagnostics
	printf("OpenGL Version: %s\n", (const char *)glGetString(GL_VERSION));
	printf("Renderer: %s\n", (const char *)glGetString(GL_RENDERER));
	printf("Vendor: %s\n", (const char *)glGetString(GL_VENDOR));
	printf("GLSL: %s\n", (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION));

#ifdef _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

	glDebugMessageCallback(errorCallback, 0);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, true);
#endif

	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glDepthFunc(GL_LEQUAL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.25f, 0.25f, 0.25f, 1);

	enableVSync(true);

	return true;
}

void context::Tick()
{
	glfwPollEvents();
	glfwSwapBuffers(window);
}

void context::term()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

bool context::shouldClose() const
{
	return glfwWindowShouldClose(window);
}

void context::clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void context::enableVSync(bool vsync)
{
	vsyncEnabled = vsync;
	glfwSwapInterval(vsyncEnabled ? 1 : 0);
}