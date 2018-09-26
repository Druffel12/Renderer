#include "Timer.h"

#include "Glfw/glfw3.h"

Timer::Timer()
{
	lastTime = currentTime = 0.0;
	frameCount = 0;
}

void Timer::Tick()
{
	frameCount++;
	lastTime = currentTime;
	currentTime = static_cast<float>(glfwGetTime());
}

float Timer::Time()
{
	return static_cast<float>(glfwGetTime());
}

float Timer::deltaTime()
{
	return static_cast<float>(currentTime - lastTime);
}

unsigned int Timer::fps()
{
	return static_cast<int>(1 / deltaTime());
}
