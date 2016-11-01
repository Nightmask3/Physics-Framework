#include "Primitive.h"

void Primitive::ApplyTexture(unsigned int aTextureID)
{
	PrimitiveEvent TextureEvent;
	TextureEvent.EventID = PrimitiveEvent::TEXTURE_REQUEST;
	TextureEvent.TextureID = aTextureID;
	TextureRequest.Notify(this, &TextureEvent);
}

void Primitive::Update()
{
}
