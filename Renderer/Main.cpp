#include "Context.h"
#include "Render.h"
#include "glm/ext.hpp"

int main()
{
	context game;
	game.init(800, 600, "Banana");

	vertex triVerts[]= 
	{
		{{-.5f, -.5f, 0, 1}},
		{{.5f, -.5f, 0, 1}},
		{{0, .5f, 0, 1}}
	};
	vertex triVerts2[] =
	{
		{ { -.5f, .1f, 0, 1 } },
	{ { .5f, .1f, 0, 1 } },
	{ { 0, -.9f, 0, 1 } }
	};
	unsigned int triIndices[] = { 2, 1, 0 };
	
	//triangles
	geometry triangle = makeGeometry(triVerts, 3, triIndices, 3);
	geometry triangle2 = makeGeometry(triVerts2, 3, triIndices, 3);

	const char * basicVert =
		"#version 410\n"
		"layout (location = 0) in vec4 position;\n"
		"void main() { gl_Position = position; }";

	const char * mvpVert =
		"#version 430\n"
		"layout (location = 0) in vec4 position;\n"
		"layout (location = 0) uniform mat4 proj;\n"
		"layout (location = 1) uniform mat4 view;\n"
		"layout (location = 2) uniform mat4 model;\n"
		"void main() { gl_Position = proj * view * model * position; }";

	const char * basicFrag =
		"#version 330\n"
		"out vec4 vertColor;\n"
		"void main() { vertColor = vec4(1.0, 1.0, 0.0, 1.0); }";
									//Red, Green, Blue, Alpha.

	Shader basicShad = makeShader(basicVert, basicFrag);
	Shader mvpShad = makeShader(mvpVert, basicFrag);

	glm::mat4 cam_proj = glm::perspective(glm::radians(45.f), 800.0f / 600.0f, 0.1f, 1000.0f);
	glm::mat4 cam_view = glm::lookAt(glm::vec3(0, 0, -1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 triangle_model = glm::identity<glm::mat4>();


	while (!game.shouldClose())
	{
		game.tick();
		game.clear();

		triangle_model = glm::rotate(triangle_model, glm::radians(5.f), glm::vec3(0, 1, 0));

		setUniform(mvpShad, 0, cam_proj);
		setUniform(mvpShad, 1, cam_view);
		setUniform(mvpShad, 2, triangle_model);

		draw(mvpShad, triangle);
		draw(mvpShad, triangle2);
	}

	game.term();

	return 0;
}