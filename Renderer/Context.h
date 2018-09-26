#pragma once

class context
{
	struct GLFWwindow *window;

	bool vsyncEnabled;

public:
	bool init(int width, int height, const char *title);
	void Tick();
	void term();
	void clear();

	void enableVSync(bool vsync);

	bool shouldClose() const;
};