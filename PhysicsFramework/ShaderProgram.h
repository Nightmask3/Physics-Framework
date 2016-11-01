#pragma once

class Renderer;

class ShaderProgram
{
private:
	/*--------------------------- SHADER FILE NAME LIST --------------------------------*/
	const char * vertexFile[1];
	const char * fragmentFile[1];
	/*--------------------------- VERTEX SHADER --------------------------------*/
	GLuint vertexShader;
	std::string vertexSource;
	/*--------------------------- FRAGMENT SHADER --------------------------------*/
	GLuint fragmentShader;
	std::string fragmentSource;
	/*--------------------------- SHADER PROGRAM --------------------------------*/
	GLuint shaderProgram;
	/*--------------------------- RENDERER REFERENCE --------------------------------*/
	Renderer const & RendererReference;

public:
	ShaderProgram(Renderer const & render) : RendererReference(render) {}
	virtual ~ShaderProgram();


    inline Renderer const & GetRenderer() { return RendererReference; };
	bool CreateVertexShader();
	bool CreateFragmentShader();
	// Creates, links and validates the Final Shader Program (FISP)
	bool CreateShaderProgram();
	GLuint GetShaderProgram() { return shaderProgram; }
	void Use();
	void Unuse();

};
