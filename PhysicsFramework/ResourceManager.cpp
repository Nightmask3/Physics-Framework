#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

// Simple OpenGL Image Loader (SOIL)
#include "SOIL.h"
#include "Texture.h"

#include "Engine.h"
#include "ResourceManager.h"
#include "Renderer.h"

std::string ResourceManager::ReadTextFile(const char * src) const
{
	std::stringstream content;
	std::ifstream file(src);

	if (!file.is_open())
	{
		std::cout << "Error opening file " << src << std::endl;
		return NULL;
	}

	content << file.rdbuf();
	return content.str();
}

Texture * ResourceManager::LoadTexture(int width, int height, char * filename)
{
	// Load texture
	unsigned char * pixelArray = SOIL_load_image(filename, &width, &height, 0, SOIL_LOAD_RGBA);
	// Create texture
	Texture * newTexture = new Texture(width, height, pixelArray, pRenderer->GetTextureCount());
	// Add to texture list
	TextureList.emplace_back(newTexture);

	return newTexture;
}

void ResourceManager::OnNotify(Object * object, Event * event)
{
	// Check if this is an Engine event
	EngineEvent * engineEvent = static_cast<EngineEvent *>(event);
	if (engineEvent)
	{
		switch (engineEvent->EventID)
		{
			case EngineEvent::EventList::ENGINE_LOAD:
			{
				LoadTexture(256, 256, "..\\Resources\\Flare.png");
				break;
			}
		}
	}
}
