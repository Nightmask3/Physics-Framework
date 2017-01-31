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


TextFileData & ResourceManager::LoadTextFile(const char* aFileName, AccessType aAccessType) const
{
	FILE * fp = nullptr;
	switch (aAccessType)
	{
	case READ:
		fp = fopen(aFileName, "r");
		break;
	case WRITE:
		fp = fopen(aFileName, "w");
		break;
	case APPEND:
		fp = fopen(aFileName, "a");
		break;
	default:
		std::cout << "Incorrect File Access Mode!\n";
		break;
	}

	if (fp)
	{
		std::fseek(fp, 0, SEEK_END);
		long fileSize = std::ftell(fp); // Finds the size of the file

		char * fileContents = new char[fileSize];
		// Null out character array
		memset(fileContents, '\0', sizeof(char) * fileSize);
		
		std::rewind(fp);				// Resets the file pointer
		std::fread(fileContents, 1, fileSize, fp); // Reads from file into char array
		std::fclose(fp);				// Closes file

		// File loaded, return pointer
		TextFileData * fileData = new TextFileData;
		fileData->pData = fileContents;
		fileData->Size = fileSize;
		return *fileData;
	}
	else
	{
		std::cout << "Loading file failed!\n";
		return TextFileData();
	}
}

Texture * ResourceManager::LoadTexture(int width, int height, char * filename)
{
	// Load texture
	unsigned char * pixelArray = SOIL_load_image(filename, &width, &height, 0, SOIL_LOAD_RGBA);
	// Create texture
	Texture * newTexture = new Texture(width, height, pixelArray, EngineHandle.GetRenderer().GetTextureCount());
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
