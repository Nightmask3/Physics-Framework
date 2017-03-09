#pragma once
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

struct Line
{
	glm::vec3 PointA = glm::vec3(0);
	glm::vec3 PointB = glm::vec3(0);
	float Scale = 1.0f;
	static const int VertexCount = 9;
	glm::vec3 Color = glm::vec3(1, 0, 0);
	GLuint VAO;
	GLuint VBO;
	
	Line(glm::vec3 aPointA, glm::vec3 aPointB)
		: 
		PointA(aPointA),
		PointB(aPointB)
	{
		
	}

	inline void BindVertexData()
	{
		float ArrowVertices[] = 
		{
			// Arrow bar triangle 1
			0.f, -0.1f, 0.f, Color.x, Color.y, Color.z, 1.f,
			0.75f, -0.1f, 0.f, Color.x, Color.y, Color.z, 1.f,
			0.f, 0.1f, 0.f, Color.x, Color.y, Color.z, 1.f,
			// Arrow bar triangle 2
			0.75f, -0.1f, 0.f, Color.x, Color.y, Color.z, 1.f,
			0.75f, 0.1f, 0.f, Color.x, Color.y, Color.z, 1.f,
			0.f, 0.1f, 0.f, Color.x, Color.y, Color.z, 1.f,
			// Arrow tip triangle
			0.75f, -0.2f, 0.f, Color.x, Color.y, Color.z, 1.f,
			1.f, 0.f, 0.f, Color.x, Color.y, Color.z, 1.f,
			0.75f, 0.2f, 0.f, Color.x, Color.y, Color.z, 1.f,
		};

		GLsizei stride = 7 * sizeof(GLfloat);
		Scale = glm::distance(PointA, PointB);
		int primitiveSize = stride * VertexCount;
		/*--------------------------- VERTEX ARRAY OBJECT --------------------------------*/
		glBindVertexArray(VAO);
		/*--------------------------- VERTEX BUFFER OBJECT --------------------------------*/
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, primitiveSize, &ArrowVertices[0], GL_STATIC_DRAW);
		/*--------------------------- VERTEX ATTRIBUTE POINTERS --------------------------------*/
		// Position
		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			stride,
			(GLvoid*)0
		);
		glEnableVertexAttribArray(0);

		// Color
		glVertexAttribPointer(
			1,
			4,
			GL_FLOAT,
			GL_FALSE,
			stride,
			(GLvoid*)(3 * sizeof(GLfloat))
		);
		glEnableVertexAttribArray(1);

	}
};