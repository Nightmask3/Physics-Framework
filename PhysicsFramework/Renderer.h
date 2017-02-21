#pragma once
// C++ header files
#include <iostream>
#include <fstream>
#include <list>
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
#include "Camera.h"
//Component Files
#include "Component.h"
#include "Transform.h"
#include "Primitive.h"
// Render utilities
#include "ShaderProgram.h"
#include "Line.h"

class Renderer : public Observer
{
	/*----------MEMBER VARIABLES----------*/
private:
	/*--------------------------- CONSTANTS --------------------------------*/
	const static int MAXIMUM_RENDER_OBJECTS = 4096;
	/*--------------------------- SHADER PROGRAMS --------------------------------*/
	ShaderProgram DefaultShader;
	ShaderProgram DebugNormalsShader;
	ShaderProgram DebugLinesShader;
	/*--------------------------- VERTEX ARRAY OBJECTS --------------------------------*/
	GLuint * DefaultVAOList[MAXIMUM_RENDER_OBJECTS];
	/*--------------------------- VERTEX BUFFER OBJECTS --------------------------------*/
	GLuint * DefaultVBOList[MAXIMUM_RENDER_OBJECTS];
	/*--------------------------- ELEMENT BUFFER OBJECTS --------------------------------*/
	GLuint * EABList[MAXIMUM_RENDER_OBJECTS];
	/*--------------------------- TEXTURE BUFFER OBJECTS --------------------------------*/
	GLuint * TBOList[MAXIMUM_RENDER_OBJECTS];
	/*------------------------------- ENGINE REFERENCE -------------------------------*/
	Engine const & EngineHandle;
	/*--------------------------- MATRICES --------------------------------*/
	glm::mat4 Projection;
	glm::mat4 View;

	// The horizontal Field of View, in degrees : the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
	float FieldOfView = 45.0f;

	GameObject * MinkowskiDifference;
	std::vector<Line> DebugLinesStack;
	int DebugLinesCounter;
	Primitive * DebugLinePrimitive;
public:
	// List of render components
	std::vector<Primitive *> RenderList;
	// Holds the number of currently active/bound textures
	GLuint TextureCount;

	// Later on use an array of unique ptrs to cameras owned by renderer, 
	// active camera at any time is pointed to by this pointer
	Camera * pActiveCamera;
	/*----------MEMBER FUNCTIONS----------*/
public:
	Renderer(Engine const & aEngine) :EngineHandle(aEngine),
		DefaultShader(*this), 
		DebugNormalsShader(*this),
		DebugLinesShader(*this),
		TextureCount(0)
	{}

	virtual ~Renderer()
	{
		for (int i = 0; i < MAXIMUM_RENDER_OBJECTS; i++)
		{
			// BUFFER DELETION
			glDeleteBuffers(1, DefaultVBOList[i]);
			glDeleteBuffers(1, EABList[i]);
			// TEXTURE DELETION
			glDeleteTextures(1, TBOList[i]);
			// VERTEX ARRAY DELETION
			glDeleteVertexArrays(1, DefaultVAOList[i]);
		}
	}

	// Getters
	inline GLuint const & GetActiveShaderProgram() { return DefaultShader.GetShaderProgram(); }
	inline int GetRenderListSize() { return (int)RenderList.size(); }
	inline int GetTextureCount() { return TextureCount; }
	
	Engine const & GetEngine() { return EngineHandle; }
	Engine const & GetEngine() const { return EngineHandle; }

	// Setters
	inline void SetActiveCamera(Camera * aCameraPtr) { pActiveCamera = aCameraPtr; }

	void InititalizeRenderer();
	// Gives a primitive a VAO, VBO to use, adds to list of render objects
	void RegisterPrimitive(Primitive * aNewPrimitive);
	// Create the debug line primitive and save it for later
	void CreateDebugLinePrimitive();
	// Pushes debug line onto a stack of lines to be drawn
	void RegisterDebugLine(Line & aLine);
	bool BindTexture(Primitive * aPrimitive, int aTextureID);
	
	void Render();
	// Main pass render functions
	void MainRenderPass();

	// Debug pass render function
	void DebugRenderPass();
	void RenderDebugWireframes(GLint aMVPAttributeIndex);
	void RenderDebugNormals(GLint aMVPAttributeIndex);
	void RenderDebugLines(GLint aMVPAttributeIndex);

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