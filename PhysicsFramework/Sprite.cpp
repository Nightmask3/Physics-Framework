#include <gl/glew.h>
#include "Sprite.h"
void check_gl_error_render()
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
Sprite::Sprite(GLuint VAO, GLuint VBO) : Primitive(PrimitiveType::SPRITE, VAO, VBO)
{
	BindVertexData(std::vector<Vertex>());
}

void Sprite::BindVertexData(std::vector<Vertex> & aVertexData)
{
	GLfloat vertices[] = {
		//  Position      Color         Texcoords
		-0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,	// Top-left
		0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,	// Top-right
		0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,	// Bottom-right

		0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,	// Bottom-right
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // Bottom-left
		-0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f	// Top-left
	};
	SetPrimitiveSize(sizeof(vertices));
	/*--------------------------- VERTEX ARRAY OBJECT --------------------------------*/
	glBindVertexArray(GetVAO());
	/*--------------------------- VERTEX BUFFER OBJECT --------------------------------*/
	glBindBuffer(GL_ARRAY_BUFFER, GetVBO());
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	/*--------------------------- VERTEX ATTRIBUTE POINTERS --------------------------------*/
	// Position
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(GLvoid*)0
	);
	glEnableVertexAttribArray(0);

	// Color
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(GLvoid*)(3 * sizeof(GLfloat))
	);
	glEnableVertexAttribArray(1);
	// Texture Coordinates
	glVertexAttribPointer(
		2,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(GLvoid*)(6 * sizeof(GLfloat))
	);
	glEnableVertexAttribArray(2);
	// Unbind VAO
	glBindVertexArray(0);
	
}
