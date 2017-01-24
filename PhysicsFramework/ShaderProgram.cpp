// C++ header files
#include <iostream>
// GLEW
#include <GL/glew.h>

#include "ShaderProgram.h"
#include "Renderer.h"
// explanation here: https://youtu.be/6ByZWqPzI70?list=PLRwVmtr-pp06qT6ckboaOhnm9FxmzHpbY
bool checkStatus(GLuint objectID, PFNGLGETSHADERIVPROC objPropGetterFunc, PFNGLGETSHADERINFOLOGPROC getInfoLogFunc, GLenum statusType)
{
	GLint status;
	objPropGetterFunc(objectID, statusType, &status);
	if (status != GL_TRUE)
	{
		GLint infoLogLength;
		objPropGetterFunc(objectID, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* buffer = new GLchar[infoLogLength];

		GLsizei bufferSize;
		getInfoLogFunc(objectID, infoLogLength, &bufferSize, buffer);
		std::cout << buffer << std::endl;
		delete[] buffer;
		return false;
	}

	return true;
}

bool checkShaderStatus(GLuint shaderID)
{
	return checkStatus(shaderID, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}
bool checkProgramStatus(GLuint programID)
{
	return checkStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
}

void ShaderProgram::Use()
{
	glUseProgram(shaderProgram);
}

ShaderProgram::~ShaderProgram()
{
	// SHADER DELETION
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteProgram(shaderProgram);
}

bool ShaderProgram::CreateVertexShader()
{
	/*--------------------------- VERTEX SHADER --------------------------------*/
	// Reads the content of vertex shader code into a string
	std::string vertexSource = std::string(GetRenderer().GetResourceManager().LoadTextFile("VertexShader.glsl", READ));
	vertexShader = glCreateShader(GL_VERTEX_SHADER);	
	vertexFile[0] = { vertexSource.c_str() };
	glShaderSource(vertexShader, 1, vertexFile, NULL);	
	glCompileShader(vertexShader);
	/*--------------------------- VERTEX SHADER DEBUG --------------------------------*/
	GLint status;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
	if (status == GL_TRUE)
	{
		std::cout << "\nVertex shader compiled successfully.";
		return true;
	}
	else
	{
		std::cout << "\nVertex shader failed to compile.";
		char buffer[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
		std::cout << buffer;
		return false;
	}
}

bool ShaderProgram::CreateFragmentShader()
{
	/*--------------------------- FRAGMENT SHADER --------------------------------*/
    // Reads the content of fragment shader code into a string
	std::string fragmentSource = std::string(GetRenderer().GetResourceManager().LoadTextFile("FragmentShader.glsl" , READ));	
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);	
	fragmentFile[0] = { fragmentSource.c_str() };		
	glShaderSource(fragmentShader, 1, fragmentFile, NULL);
	glCompileShader(fragmentShader);
	/*--------------------------- FRAGMENT SHADER DEBUG --------------------------------*/
	GLint status;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
	if (status == GL_TRUE)
	{
		std::cout << "\nFragment shader compiled successfully.";
		return true;
	}
	else
	{
		std::cout << "\nFragment shader failed to compile.";
		char buffer[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, buffer);
		std::cout << buffer;
		return false;
	}
}
bool ShaderProgram::CreateShaderProgram()
{
	/*--------------------------- SHADER CREATION --------------------------------*/
	CreateVertexShader();
	CreateFragmentShader();
	shaderProgram = glCreateProgram();
	// SHADER ATTACHMENT
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	//glBindFragDataLocation(shaderProgram, 0, "outColor"); // Not necessary, but when using multiple buffers, binds the fragment data output to the right buffer

	// LINKING
	glLinkProgram(shaderProgram);					
	// VALIDATION
	glValidateProgram(shaderProgram);
	/*--------------------------- SHADER PROGRAM DEBUG --------------------------------*/
	GLint linked;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linked);
	if (linked == GL_TRUE)
	{
		std::cout << "\nShader program compiled successfully." << std::endl;
		return true;
	}
	else
	{
		std::cout << "\nShader program failed to compile.";
		char buffer[512];
		glGetShaderInfoLog(shaderProgram, 512, NULL, buffer);
		std::cout << buffer << std::endl;
		return false;
	}
}