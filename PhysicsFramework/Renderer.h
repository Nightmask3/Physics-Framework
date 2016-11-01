#pragma once
// C++ header files
#include <iostream>
#include <fstream>
#include <vector>
// Math header files
#include "glm/vec2.hpp"
#include "glm/mat3x3.hpp"
// Manager header file
#include "WindowManager.h"
#include "InputManager.h"
#include "ResourceManager.h"
// Entity classes
#include "GameObject.h"
#include "Observer.h"
//Component Files
#include "Component.h"
#include "Transform.h"
#include "Primitive.h"
// Render utilities
#include "ShaderProgram.h"

class Renderer : public Observer
{
	/*----------MEMBER VARIABLES----------*/
private:
	/*--------------------------- CONSTANTS --------------------------------*/
	const static int MAXIMUM_SPRITES = 4096;
	/*--------------------------- SHADER PROGRAM --------------------------------*/
	ShaderProgram ActiveShaderProgram;
	/*--------------------------- VERTEX ARRAY OBJECTS --------------------------------*/
	GLuint * VAO[MAXIMUM_SPRITES];
	/*--------------------------- VERTEX BUFFER OBJECTS --------------------------------*/
	GLuint * VBO[MAXIMUM_SPRITES];
	/*--------------------------- ELEMENT BUFFER OBJECTS --------------------------------*/
	GLuint * EAB[MAXIMUM_SPRITES];
	/*--------------------------- TEXTURE BUFFER OBJECTS --------------------------------*/
	GLuint * TBO[MAXIMUM_SPRITES];
	/*------------------------------- MANAGER REFERENCES -------------------------------*/
	WindowManager const & WindowManagerReference;
	InputManager const & InputManagerReference;
	ResourceManager const & ResourceManagerReference;

public:
	// List of render components
	std::vector<Primitive *> RenderList;
	// Holds the number of currently active/bound textures
	GLuint TextureCount;

	/*----------MEMBER FUNCTIONS----------*/
public:
	Renderer(WindowManager const & win, InputManager const & in, ResourceManager const & res) : 
		WindowManagerReference(win), 
		InputManagerReference(in), 
		ResourceManagerReference(res), 
		ActiveShaderProgram(*this),
		TextureCount(0)
	{}

	virtual ~Renderer()
	{
		for (int i = 0; i < MAXIMUM_SPRITES; i++)
		{
			// BUFFER DELETION
			glDeleteBuffers(1, VBO[i]);
			glDeleteBuffers(1, EAB[i]);
			// TEXTURE DELETION
			glDeleteTextures(1, TBO[i]);
			// VERTEX ARRAY DELETION
			glDeleteVertexArrays(1, VAO[i]);
		}
	}

	inline ResourceManager const & GetResourceManager() const { return ResourceManagerReference; }
	inline GLuint const & GetActiveShaderProgram() { return ActiveShaderProgram.GetShaderProgram(); }
	inline int GetRenderListSize() { return (int)RenderList.size(); }
	inline int GetTextureCount() { return TextureCount; }

	void InititalizeRenderer();
	void RegisterPrimitive(Primitive * aNewPrimitive);
	bool BindTexture(Primitive * aPrimitive, int aTextureID);
	bool Render();

	static void check_gl_error_render()
	{
		GLenum err(glGetError());
		using namespace std;
		while (err != GL_NO_ERROR) {
			string error;

			switch (err) {
			case GL_INVALID_OPERATION:      error = "INVALID_OPERATION";      break;
			case GL_INVALID_ENUM:           error = "INVALID_ENUM";           break;
			case GL_INVALID_VALUE:          error = "INVALID_VALUE";          break;
			case GL_OUT_OF_MEMORY:          error = "OUT_OF_MEMORY";          break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";  break;
			}

			cerr << "GL_" << error.c_str() << endl;
			err = glGetError();
		}
	}

	virtual void OnNotify(Object * object, Event * event) override;
};