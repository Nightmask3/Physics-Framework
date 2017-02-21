#include "Debug.h"

Debug::Debug(GLuint VAO, GLuint VBO) : Primitive(PrimitiveType::DEBUG, VAO, VBO)
{
	// Debug components do not need debug 
	RenderDebug = false;
}

Component::ComponentType Debug::GetComponentID()
{
	return ComponentType::PRIMITIVE;
}
