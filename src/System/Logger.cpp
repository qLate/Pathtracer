#include "Logger.h"

#include <iostream>

#include "BVH.h"
#include "Scene.h"
#include "SDLHandler.h"
#include "Utils.h"

void Logger::updatePrintFPS()
{
	uint32_t currTime = SDL_GetTicks();
	if (currTime >= lastFPSPrintTime + 200)
	{
		lastFPSPrintTime = currTime;

		std::cout
			<< "FPS: " << Utils::round(SDLHandler::io->Framerate, 1)
			<< " Triangles: " << Scene::triangles.size()
			<< " Bounding Boxes: " << BVHBuilder::nodes.size() << '\n';
	}
}
