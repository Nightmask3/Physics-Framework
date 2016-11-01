#pragma once

#include <string>
#include <utility>
#include <vector>
#include <memory>

#include "Observer.h"
#include "Texture.h"

class Renderer;

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

	std::string ReadContents(const char*) const;
	Texture * LoadTexture(int width, int height, char * filename);
	virtual void OnNotify(Object * object, Event * event) override;
};