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
	glUseProgram(ShaderProgramID);
}

ShaderProgram::~ShaderProgram()
{
	// SHADER DELETION
	glDeleteShader(VertexShader);
	glDeleteShader(FragmentShader);
	glDeleteProgram(ShaderProgramID);
}
/*--------------------------- DEFAULT SHADERS --------------------------------*/
bool ShaderProgram::CreateDefaultVertexShader()
{
	
	std::cout << "/*--------------------------- DEFAULT SHADERS --------------------------------*/";
	Engine const & engineRef = GetRenderer().GetEngine();
	/*--------------------------- VERTEX SHADER --------------------------------*/
	// Reads the content of vertex shader code into a string
	std::string vertexSource = std::string(engineRef.GetResourceManager().LoadTextFile("DefaultVertexShader.glsl", READ).pData);
	VertexShader = glCreateShader(GL_VERTEX_SHADER);	
	vertexFile[0] = { vertexSource.c_str() };
	glShaderSource(VertexShader, 1, vertexFile, NULL);	
	glCompileShader(VertexShader);
	/*--------------------------- VERTEX SHADER DEBUG --------------------------------*/
	GLint status;
	glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &status);
	if (status == GL_TRUE)
	{
		std::cout << "\nVertex shader compiled successfully.";
		return true;
	}
	else
	{
		std::cout << "\nVertex shader failed to compile.";
		char buffer[512];
		glGetShaderInfoLog(VertexShader, 512, NULL, buffer);
		std::cout << buffer;
		return false;
	}
}

bool ShaderProgram::CreateDefaultFragmentShader()
{
	Engine const & engineRef = GetRenderer().GetEngine();
	/*--------------------------- FRAGMENT SHADER --------------------------------*/
    // Reads the content of fragment shader code into a string
	std::string fragmentSource = std::string(engineRef.GetResourceManager().LoadTextFile("DefaultFragmentShader.glsl" , READ).pData);	
	FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);	
	fragmentFile[0] = { fragmentSource.c_str() };		
	glShaderSource(FragmentShader, 1, fragmentFile, NULL);
	glCompileShader(FragmentShader);
	/*--------------------------- FRAGMENT SHADER DEBUG --------------------------------*/
	GLint status;
	glGetShaderiv(FragmentShader, GL_COMPILE_STATUS, &status);
	if (status == GL_TRUE)
	{
		std::cout << "\nFragment shader compiled successfully.";
		return true;
	}
	else
	{
		std::cout << "\nFragment shader failed to compile.";
		char buffer[512];
		glGetShaderInfoLog(FragmentShader, 512, NULL, buffer);
		std::cout << buffer;
		return false;
	}
}
bool ShaderProgram::CreateDefaultGeometryShader()
{
	Engine const & engineRef = GetRenderer().GetEngine();
	/*--------------------------- GEOMETRY SHADER --------------------------------*/
	// Reads the content of geometry shader code into a string
	std::string geometrySource = std::string(engineRef.GetResourceManager().LoadTextFile("DefaultGeometryShader.glsl", READ).pData);
	GeometryShader = glCreateShader(GL_GEOMETRY_SHADER);
	geometryFile[0] = { geometrySource.c_str() };
	glShaderSource(GeometryShader, 1, geometryFile, NULL);
	glCompileShader(GeometryShader);
	/*--------------------------- GEOMETRY SHADER DEBUG --------------------------------*/
	GLint status;
	glGetShaderiv(GeometryShader, GL_COMPILE_STATUS, &status);
	if (status == GL_TRUE)
	{
		std::cout << "\nGeometry shader compiled successfully.";
		return true;
	}
	else
	{
		std::cout << "\nGeometry shader failed to compile.";
		char buffer[512];
		glGetShaderInfoLog(GeometryShader, 512, NULL, buffer);
		std::cout << buffer;
		return false;
	}
}
bool ShaderProgram::CreateDefaultShaderProgram()
{
	/*--------------------------- SHADER CREATION --------------------------------*/
	CreateDefaultVertexShader();
	CreateDefaultGeometryShader();
	CreateDefaultFragmentShader();
	ShaderProgramID = glCreateProgram();
	// SHADER ATTACHMENT
	glAttachShader(ShaderProgramID, VertexShader);
	glAttachShader(ShaderProgramID, GeometryShader);
	glAttachShader(ShaderProgramID, FragmentShader);
	//glBindFragDataLocation(shaderProgram, 0, "outColor"); // Not necessary, but when using multiple buffers, binds the fragment data output to the right buffer

	// LINKING
	glLinkProgram(ShaderProgramID);					
	// VALIDATION
	glValidateProgram(ShaderProgramID);
	/*--------------------------- SHADER PROGRAM DEBUG --------------------------------*/
	GLint linked;
	glGetProgramiv(ShaderProgramID, GL_LINK_STATUS, &linked);
	if (linked == GL_TRUE)
	{
		std::cout << "\nShader program compiled successfully." << std::endl;
		return true;
	}
	else
	{
		std::cout << "\nShader program failed to compile.";
		char buffer[512];
		glGetShaderInfoLog(ShaderProgramID, 512, NULL, buffer);
		std::cout << buffer << std::endl;
		return false;
	}
}
/*--------------------------- DEBUG SHADERS --------------------------------*/
bool ShaderProgram::CreateDebugVertexShader()
{
	std::cout << "/*--------------------------- DEBUG SHADERS --------------------------------*/";
	Engine const & engineRef = GetRenderer().GetEngine();
	/*--------------------------- VERTEX SHADER --------------------------------*/
	// Reads the content of vertex shader code into a string
	std::string vertexSource = std::string(engineRef.GetResourceManager().LoadTextFile("DebugVertexShader.glsl", READ).pData);
	VertexShader = glCreateShader(GL_VERTEX_SHADER);
	vertexFile[0] = { vertexSource.c_str() };
	glShaderSource(VertexShader, 1, vertexFile, NULL);
	glCompileShader(VertexShader);
	/*--------------------------- VERTEX SHADER DEBUG --------------------------------*/
	GLint status;
	glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &status);
	if (status == GL_TRUE)
	{
		std::cout << "\nVertex shader compiled successfully.";
		return true;
	}
	else
	{
		std::cout << "\nVertex shader failed to compile.";
		char buffer[512];
		glGetShaderInfoLog(VertexShader, 512, NULL, buffer);
		std::cout << buffer;
		return false;
	}
}

bool ShaderProgram::CreateDebugFragmentShader()
{
	Engine const & engineRef = GetRenderer().GetEngine();
	/*--------------------------- FRAGMENT SHADER --------------------------------*/
	// Reads the content of fragment shader code into a string
	std::string fragmentSource = std::string(engineRef.GetResourceManager().LoadTextFile("DebugFragmentShader.glsl", READ).pData);
	FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	fragmentFile[0] = { fragmentSource.c_str() };
	glShaderSource(FragmentShader, 1, fragmentFile, NULL);
	glCompileShader(FragmentShader);
	/*--------------------------- FRAGMENT SHADER DEBUG --------------------------------*/
	GLint status;
	glGetShaderiv(FragmentShader, GL_COMPILE_STATUS, &status);
	if (status == GL_TRUE)
	{
		std::cout << "\nFragment shader compiled successfully.";
		return true;
	}
	else
	{
		std::cout << "\nFragment shader failed to compile.";
		char buffer[512];
		glGetShaderInfoLog(FragmentShader, 512, NULL, buffer);
		std::cout << buffer;
		return false;
	}
}

bool ShaderProgram::CreateDebugGeometryShader()
{
	Engine const & engineRef = GetRenderer().GetEngine();
	/*--------------------------- GEOMETRY SHADER --------------------------------*/
	// Reads the content of geometry shader code into a string
	std::string geometrySource = std::string(engineRef.GetResourceManager().LoadTextFile("DebugGeometryShader.glsl", READ).pData);
	GeometryShader = glCreateShader(GL_GEOMETRY_SHADER);
	geometryFile[0] = { geometrySource.c_str() };
	glShaderSource(GeometryShader, 1, geometryFile, NULL);
	glCompileShader(GeometryShader);
	/*--------------------------- GEOMETRY SHADER DEBUG --------------------------------*/
	GLint status;
	glGetShaderiv(GeometryShader, GL_COMPILE_STATUS, &status);
	if (status == GL_TRUE)
	{
		std::cout << "\nGeometry shader compiled successfully.";
		return true;
	}
	else
	{
		std::cout << "\nGeometry shader failed to compile.";
		char buffer[512];
		glGetShaderInfoLog(GeometryShader, 512, NULL, buffer);
		std::cout << buffer;
		return false;
	}
}

bool ShaderProgram::CreateDebugShaderProgram()
{
	/*--------------------------- SHADER CREATION --------------------------------*/
	CreateDebugVertexShader();
	CreateDebugGeometryShader();
	CreateDebugFragmentShader();
	ShaderProgramID = glCreateProgram();
	// SHADER ATTACHMENT
	glAttachShader(ShaderProgramID, VertexShader);
	glAttachShader(ShaderProgramID, GeometryShader);
	glAttachShader(ShaderProgramID, FragmentShader);
	//glBindFragDataLocation(shaderProgram, 0, "outColor"); // Not necessary, but when using multiple buffers, binds the fragment data output to the right buffer

	// LINKING
	glLinkProgram(ShaderProgramID);
	// VALIDATION
	glValidateProgram(ShaderProgramID);
	/*--------------------------- SHADER PROGRAM DEBUG --------------------------------*/
	GLint linked;
	glGetProgramiv(ShaderProgramID, GL_LINK_STATUS, &linked);
	if (linked == GL_TRUE)
	{
		std::cout << "\nShader program compiled successfully." << std::endl;
		return true;
	}
	else
	{
		std::cout << "\nShader program failed to compile.";
		char buffer[512];
		glGetShaderInfoLog(ShaderProgramID, 512, NULL, buffer);
		std::cout << buffer << std::endl;
		return false;
	}
}
