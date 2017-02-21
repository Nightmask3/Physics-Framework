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

	// Creates, links and validates the Debug - Wireframes, normals - Shader Program 
	bool CreateDebugNormalsShaderProgram();

	// Creates, links and validates the Debug - Lines - Shader Program 
	bool CreateDebugLineShaderProgram();

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
	
	// Debug wireframe, face and vertex normal rendering shader programs
	bool CreateDebugNormalsVertexShader();
	bool CreateDebugNormalsFragmentShader();
	bool CreateDebugNormalsGeometryShader();

	// Debug lines shader program
	bool CreateDebugLineVertexShader();
	bool CreateDebugLineFragmentShader();

};
