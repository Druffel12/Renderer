#include "Context.h"
#include "Render.h"
#include "Timer.h"

#include "glm/ext.hpp"

int main()
{
	context game;
	game.init(800, 600, "Banana");
	game.enableVSync(true);

	Timer time;
	
	geometry triangle = loadGeometry("res\\mdl\\tri.obj");
	geometry soulspear = loadGeometry("res\\mdl\\soulspear.obj");
	geometry cube = loadGeometry("res\\mdl\\cube.obj");

	shader stdShad = loadShader("res\\shad\\mvp.vert", "res\\shad\\mvp.frag");

	texture soulDiffuse = loadTexture("res\\img\\Cole Squint.png");
	texture soulNormal = loadTexture("res\\img\\soulspear_normal.tga");
	//texture soulSpecular = loadTexture("res\\img\\soulspear_specular.tga");

	glm::mat4 cam_proj = glm::perspective(glm::radians(45.f), 800.0f / 600.0f, 0.1f, 1000.0f);
	glm::mat4 cam_view = glm::lookAt(glm::vec3(0, 1, 2), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 triangle_model = glm::identity<glm::mat4>();

	light sun;
	sun.direction = glm::vec3(-1, 0, 0);


	while (!game.shouldClose())
	{
		// services
		time.Tick();
		game.Tick();
		game.clear();

		// game logic
		triangle_model = glm::rotate(triangle_model, glm::radians(90.f) * time.deltaTime(), glm::vec3(0, 1, 0));

		// draw logic
		setUniform(stdShad, 0, cam_proj);
		setUniform(stdShad, 1, cam_view);
		setUniform(stdShad, 2, triangle_model);

		setUniform(stdShad, 3, soulDiffuse, 0);
		setUniform(stdShad, 4, sun.direction);

		draw(stdShad, cube);
	}

	freeGeometry(triangle);
	freeGeometry(cube);
	freeGeometry(soulspear);

	freeShader(stdShad);

	freeTexture(soulDiffuse);
	freeTexture(soulNormal);
	//freeTexture(soulSpecular);

	game.term();

	return 0;
}