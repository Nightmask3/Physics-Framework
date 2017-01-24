#pragma once

#include <string>
#include <utility>
#include <vector>
#include <memory>

#include "Observer.h"
#include "Texture.h"
#include "Resource.h"
class Renderer;

enum AccessType
{
	READ,
	WRITE,
	APPEND,
	AccessTypeEnd
};
class ResourceManager : public Observer
{
private:
	Renderer * pRenderer;
	std::vector<std::unique_ptr<Texture>> TextureList;
public:
	ResourceManager() {};
	virtual ~ResourceManager() {};

	inline Texture * GetTexture(int aTextureID) const { return TextureList[aTextureID].get(); }
	inline void RegisterRenderer(Renderer * aRenderer) { pRenderer = aRenderer; }

	char * LoadTextFile(const char* aFileName, AccessType aAccessType) const;
	Texture * LoadTexture(int width, int height, char * filename);
	virtual void OnNotify(Object * object, Event * event) override;
};