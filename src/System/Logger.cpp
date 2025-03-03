#include "Logger.h"

#include <iostream>

#include "BVH.h"
#include "Scene.h"
#include "SDLHandler.h"

void Logger::updateFPSCounter()
{
	uint32_t currTime = SDL_GetTicks();
	if (currTime >= lastFrameTime + 200)
	{
		fps = (int)((float)frameCount / (float)(currTime - lastFrameTime) * 1000.0f);
		frameCount = 0;
		lastFrameTime = currTime;

		std::cout
			<< "FPS: " << fps
			<< " Triangles: " << Scene::triangles.size()
			<< " Graphical Objects: " << Scene::graphicals.size()
			<< " Bounding Boxes: " << BVHBuilder::nodes.size() << '\n';
	}
	frameCount++;
}
