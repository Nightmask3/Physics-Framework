#pragma once

class Renderer;

class ShaderProgram
{
	/*-----------------MEMBER VARIABLES------------------*/
private:
	/*--------------------------- SHADER FILE NAME LIST --------------------------------*/
	const char * vertexFile[1];
	const char * fragmentFile[1];
	const char * geometryFile[1];
	/*--------------------------- VERTEX SHADER --------------------------------*/
	GLuint VertexShader;
	std::string VertexSource;
	/*--------------------------- FRAGMENT SHADER --------------------------------*/
	GLuint FragmentShader;
	std::string FragmentSource;
	/*--------------------------- GEOMETRY SHADER --------------------------------*/
	GLuint GeometryShader;
	std::string GeometrySource;
	/*--------------------------- SHADER PROGRAM --------------------------------*/
	GLuint ShaderProgramID;
	/*--------------------------- RENDERER REFERENCE --------------------------------*/
	Renderer const & RendererReference;

	/*-----------------MEMBER FUNCTIONS------------------*/
public:
	// Creates, links and validates the Default Shader Program 
	bool CreateDefaultShaderProgram();

	// Creates, links and validates the Default Shader Program 
	bool CreateDebugShaderProgram();


	ShaderProgram(Renderer const & render) : RendererReference(render) {}
	virtual ~ShaderProgram();


    inline Renderer const & GetRenderer() { return RendererReference; };
	
	GLuint GetShaderProgram() { return ShaderProgramID; }
	
	void Use();
	void Unuse();

private:
	// Default shader programs
	bool CreateDefaultVertexShader();
	bool CreateDefaultFragmentShader();
	bool CreateDefaultGeometryShader();
	
	// Debug shader programs
	bool CreateDebugVertexShader();
	bool CreateDebugFragmentShader();
	bool CreateDebugGeometryShader();
};
