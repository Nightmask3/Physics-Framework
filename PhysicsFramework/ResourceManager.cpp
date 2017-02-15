#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

// Assimp headers
#include <assimp/Importer.hpp>       // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/mesh.h>            // Mesh data structure
#include <assimp/color4.h>			// Color data structure
#include <assimp/postprocess.h>     // Post processing flags

// Simple OpenGL Image Loader (SOIL) headers
#include "SOIL.h"
#include "Texture.h"

#include "Engine.h"
#include "ResourceManager.h"
#include "Renderer.h"
#include "GameObjectFactory.h"
#include "Mesh.h"

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

Mesh * ResourceManager::ImportMesh(std::string & aFilename)
{
	// Create an instance of the Importer class  
	Assimp::Importer importer;  
	
	// Load file with postprocessing tags
	const aiScene* scene = 
		importer.ReadFile(aFilename,
		//aiProcess_CalcTangentSpace|
		aiProcess_Triangulate |
		//aiProcess_JoinIdenticalVertices|
		aiProcess_SortByPType  
		);
	// If the import failed, report it  
	if( !scene)  
	{    
		std::cout << importer.GetErrorString();    
		return nullptr; 
	}

	if (scene->HasMeshes())
	{
		aiMesh * importedMesh = scene->mMeshes[0];
		Mesh * newMesh = EngineHandle.GetGameObjectFactory().SpawnComponent<Mesh>();
		
		std::vector<Vertex> importedVertexData;
		for (int i = 0; i < importedMesh->mNumVertices; ++i)
		{
			glm::vec3 newVertexPosition, newVertexNormal;
			glm::vec4 newVertexColor;
			glm::vec2 newVertexUV;

			// Positions
			newVertexPosition.x = importedMesh->mVertices->x;
			newVertexPosition.y = importedMesh->mVertices->y;
			newVertexPosition.z = importedMesh->mVertices->z;

			// Normals
			newVertexNormal.x = importedMesh->mNormals->x;
			newVertexNormal.y = importedMesh->mNormals->y;
			newVertexNormal.z = importedMesh->mNormals->z;

			// Colors - if a color channel exists
			if (importedMesh->GetNumColorChannels())
			{
				newVertexColor.x = importedMesh->mColors[0]->r;
				newVertexColor.y = importedMesh->mColors[0]->g;
				newVertexColor.z = importedMesh->mColors[0]->b;
				newVertexColor.w = importedMesh->mColors[0]->a;
			}
			else // default to white
				newVertexColor = glm::vec4(1);

			// UVs - if a UV channel exists
			if (importedMesh->GetNumUVChannels())
			{
				newVertexUV.x = importedMesh->mTextureCoords[0]->x;
				newVertexUV.y = importedMesh->mTextureCoords[0]->y;
			}

			// Create new vertex and add it to vertex list
			Vertex newVertex(newVertexPosition, newVertexNormal, newVertexColor, newVertexUV);
			importedVertexData.push_back(newVertex);
		}

		newMesh->SetVertices(importedVertexData);
		return newMesh;
	}
	else
	{
		std::cout << "No meshes in imported file: " << aFilename << std::endl;
		return nullptr;
	}

}

void ResourceManager::OnNotify(Object * object, Event * event)
{
	// Check if this is an Engine event
	EngineEvent * engineEvent = static_cast<EngineEvent *>(event);
	if (engineEvent)
	{
		if (engineEvent->EventID == EngineEvent::EventList::ENGINE_LOAD)
		{
			LoadTexture(256, 256, "..\\Resources\\Flare.png");
		}
	}
}
