#pragma once

class Timer
{
	double lastTime;
	double currentTime;

	size_t frameCount;

public:

	Timer();

	void Tick();

	float Time();
	float deltaTime();

	unsigned int fps();

};