#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "Renderer.h"
#include "GameObjectFactory.h"
#include "GameObject.h"
#include "UtilityFunctions.h"

void Renderer::RegisterPrimitive(Primitive * aNewPrimitive)
{
	*DefaultVAOList[RenderList.size()] = *DefaultVBOList[RenderList.size()] = (GLuint)(RenderList.size() + 1);
	RenderList.push_back(aNewPrimitive);
	aNewPrimitive->TextureRequest.AddObserver(this);
}

void Renderer::CreateDebugLinePrimitive()
{
	DebugLinePrimitive = EngineHandle.GetGameObjectFactory().SpawnComponent<Debug>();
	Line newLine(glm::vec3(0), glm::vec3(1));
	newLine.VAO = DebugLinePrimitive->GetVAO();
	newLine.VBO = DebugLinePrimitive->GetVBO();
	newLine.BindVertexData();
}

void Renderer::CreateDebugQuadPrimitive()
{
	DebugQuadPrimitive = EngineHandle.GetGameObjectFactory().SpawnComponent<Debug>();
	Quad newQuad(glm::vec3(0));
	newQuad.VAO = DebugQuadPrimitive->GetVAO();
	newQuad.VBO = DebugQuadPrimitive->GetVBO();
	newQuad.BindVertexData();
}

void Renderer::RegisterDebugLine(Line & aLine)
{
	DebugLinesStack.push_back(aLine);
}

void Renderer::InititalizeRenderer()
{
	/*--------------------------- GLEW INIT -------------------------------*/
	glewExperimental = GL_TRUE;
	if (glewInit())
	{
		int x;
		std::cout << "GLEW failed to initialize!\n";
		std::cin >> x;
		exit(1); // or handle the error in a nicer way, TODO : print to log or something
	}
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	std::cout << "Renderer: " << renderer << std::endl;
	std::cout << "OpenGL version:" << version << std::endl;
	/*---------- BUFFER ALLOCATION ----------*/
	for (int i = 0; i < MAXIMUM_RENDER_OBJECTS; i++)
	{
		// Allocates the vertex data objects
		DefaultVAOList[i] = new GLuint;
		glGenVertexArrays(1, DefaultVAOList[i]);
		DefaultVBOList[i] = new GLuint;
		glGenBuffers(1, DefaultVBOList[i]);
		EABList[i] = new GLuint;
		glGenBuffers(1, EABList[i]);
		TBOList[i] = new GLuint;
		glGenTextures(1, TBOList[i]);
	}

	/*---------- OPEN GL SETTINGS ----------*/
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);		// Depth test
	glEnable(GL_BLEND);				// Alpha Blending
	glDisable(GL_CULL_FACE);		// Backface Culling

	/*---------- SHADER CREATION ----------*/
	// Create and use default shader program
	DefaultShader.CreateDefaultShaderProgram();
	DefaultShader.Use();
	// Create debug normals shader program, reserve for later
	DebugNormalsShader.CreateDebugNormalsShaderProgram();
	// Create debug lines shader program, reserve for later
	DebugLinesShader.CreateDebugLineShaderProgram();
	// Create debug quads shader program, reserve for later
	DebugQuadsShader.CreateDebugQuadShaderProgram();

	/*---------- PRIMITIVE CREATION ----------*/
	CreateDebugLinePrimitive();
	CreateDebugQuadPrimitive();
}

void Renderer::Render()
{
	// Update camera values before constructing view matrix
	glm::vec3 cameraPosition = pActiveCamera->GetCameraPosition();
	glm::vec3 cameraTarget = pActiveCamera->GetCameraLookDirection();
	glm::vec3 upVector = pActiveCamera->GetCameraUpDirection();

	/*-------------------------------- VIEW MATRIX -------------------------------*/
	View = glm::lookAt(
		cameraPosition,
		cameraTarget,
		upVector
	);

	// Update field of view before constructing projection matrix
	FieldOfView += InputManager::GetScrollDelta().y * pActiveCamera->GetSensitivity();

	/*-------------------------------- PROJECTION MATRIX -------------------------------*/
	Projection = glm::perspective(
		FieldOfView, // The horizontal Field of View, in degrees : the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
		4.0f / 3.0f, // Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960, sounds familiar ?
		0.1f,        // Near clipping plane. Keep as big as possible, or you'll get precision issues.
		100.0f       // Far clipping plane. Keep as little as possible.
	);

	// Sets background color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Clear color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw all gameobjects
	MainRenderPass();

	if(EngineHandle.GetInputManager().isKeyPressed(GLFW_KEY_TAB))
		// Draw wireframes, face & vertex normals, debug lines
		DebugRenderPass();
}

void Renderer::MainRenderPass()
{
	/*-------------------------------- REGULAR MESH RENDER-------------------------------*/
	DefaultShader.Use();

	// Get the MVP Matrix id
	GLint glModelViewProjection = glGetUniformLocation(DefaultShader.GetShaderProgram(), "ModelViewProjectionMatrix");
	if (EngineHandle.GetInputManager().isKeyPressed(GLFW_KEY_CAPS_LOCK))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	//// Render Minkowski Difference
	//Mesh * shape1 = static_cast<Mesh *>(RenderList[3]);
	//Mesh * shape2 = static_cast<Mesh *>(RenderList[4]);
	//std::vector<Vertex> MinkowskiDifferenceVertices;
	//Utility::CalculateMinkowskiDifference(MinkowskiDifferenceVertices, shape1, shape2);
	//MinkowskiDifference->GetComponent<Mesh>()->BindVertexData(MinkowskiDifferenceVertices);

	for (int i = 0; i < RenderList.size(); ++i)
	{
		Transform * transform = nullptr;
		Primitive * primitive = RenderList[i];

		// Skip any debug objects
		if (primitive->GetPrimitiveType() == Primitive::DEBUG)
			continue;

		GameObject const * renderObject = primitive->GetConstOwner(); 
		transform = renderObject->GetComponent<Transform>();
		// Calculate the MVP matrix and set the matrix uniform
		glm::mat4 mvp;
		glm::mat4 model;
		glm::mat4 translate = glm::translate(transform->GetPosition());
		glm::mat4 rotate = glm::mat4_cast(transform->GetRotation());
		glm::mat4 scale = glm::scale(transform->GetScale());
		model = translate * rotate * scale;
		mvp = Projection * View * model;
		glEnableClientState(GL_VERTEX_ARRAY);
		// Uniform matrices ARE supplied in Row Major order hence set to GL_TRUE
		glUniformMatrix4fv(glModelViewProjection, 1, GL_FALSE, &mvp[0][0]);
		check_gl_error_render();
		// Bind TBO
		glBindTexture(GL_TEXTURE_2D, primitive->GetTBO());
		// Bind VAO
		glBindVertexArray(*DefaultVAOList[i]);
		check_gl_error_render();
		glDrawArrays(GL_TRIANGLES, 0, primitive->GetPrimitiveSize()/sizeof(Vertex));
		check_gl_error_render();
		assert(glGetError() == GL_NO_ERROR);
		// Unbind VAO when done
		glBindVertexArray(0);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
	DefaultShader.Unuse();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Render debug lines
	DebugLinesShader.Use();
	glModelViewProjection = glGetUniformLocation(DebugLinesShader.GetShaderProgram(), "ModelViewProjectionMatrix");
	RenderDebugLines(glModelViewProjection);
	DebugLinesShader.Unuse();

	// Render debug quads
	DebugQuadsShader.Use();
	GLint glModelMatrixAttributeIndex, glViewMatrixAttributeIndex, glProjectionMatrixAttributeIndex, glBillboardModeAttributeIndex;
	glModelMatrixAttributeIndex = glGetUniformLocation(DebugQuadsShader.GetShaderProgram(), "ModelMatrix");
	glViewMatrixAttributeIndex = glGetUniformLocation(DebugQuadsShader.GetShaderProgram(), "ViewMatrix");
	glProjectionMatrixAttributeIndex = glGetUniformLocation(DebugQuadsShader.GetShaderProgram(), "ProjectionMatrix");
	glBillboardModeAttributeIndex = glGetUniformLocation(DebugQuadsShader.GetShaderProgram(), "BillboardMode");
	RenderDebugQuads(glModelMatrixAttributeIndex, glViewMatrixAttributeIndex, glProjectionMatrixAttributeIndex, glBillboardModeAttributeIndex);
	DebugQuadsShader.Unuse();
}

void Renderer::DebugRenderPass()
{
	GLint glModelViewProjectionAttribute;
	/*-------------------------------- DEBUG MESH RENDER-------------------------------*/
	// Render wireframes
	DefaultShader.Use();
	glModelViewProjectionAttribute = glGetUniformLocation(DefaultShader.GetShaderProgram(), "ModelViewProjectionMatrix");
	RenderDebugWireframes(glModelViewProjectionAttribute);
	DefaultShader.Unuse();

	// Render normals
	DebugNormalsShader.Use();
	glModelViewProjectionAttribute = glGetUniformLocation(DebugNormalsShader.GetShaderProgram(), "ModelViewProjectionMatrix");
	RenderDebugNormals(glModelViewProjectionAttribute);
	DebugNormalsShader.Unuse();
}

void Renderer::RenderDebugWireframes(GLint aMVPAttributeIndex)
{
	// Render all objects, scaled up and in wireframe mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	for (int i = 0; i < RenderList.size(); ++i)
	{
		Transform * transform = nullptr;
		Primitive * primitive = RenderList[i];

		if (primitive->RenderDebug)
		{
			GameObject const * renderObject = primitive->GetConstOwner();
			transform = renderObject->GetComponent<Transform>();
			// Calculate the MVP matrix and set the matrix uniform
			glm::mat4 mvp;
			glm::mat4 model;
			glm::mat4 translate = glm::translate(transform->GetPosition());
			glm::mat4 rotate = glm::mat4_cast(transform->GetRotation());
			glm::mat4 scale = glm::scale(transform->GetScale() * 1.25f);
			model = translate * rotate * scale;
			mvp = Projection * View * model;
			glEnableClientState(GL_VERTEX_ARRAY);
			// Uniform matrices ARE supplied in Row Major order hence set to GL_TRUE
			glUniformMatrix4fv(aMVPAttributeIndex, 1, GL_FALSE, &mvp[0][0]);
			check_gl_error_render();
			// Bind VAO
			glBindVertexArray(*DefaultVAOList[i]);
			check_gl_error_render();
			glDrawArrays(GL_TRIANGLES, 0, primitive->GetPrimitiveSize() / sizeof(Vertex));
			check_gl_error_render();
			assert(glGetError() == GL_NO_ERROR);
			// Unbind VAO when done
			glBindVertexArray(0);
			glDisableClientState(GL_VERTEX_ARRAY);
		}
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Renderer::RenderDebugNormals(GLint aMVPAttributeIndex)
{
	// Render Normals
	for (int i = 0; i < RenderList.size(); ++i)
	{
		Transform * transform = nullptr;
		Primitive * primitive = RenderList[i];

		if (primitive->RenderDebug)
		{
			GameObject const * renderObject = primitive->GetConstOwner();
			transform = renderObject->GetComponent<Transform>();
			// Calculate the MVP matrix and set the matrix uniform
			glm::mat4 mvp;
			glm::mat4 model;
			glm::mat4 translate = glm::translate(transform->GetPosition());
			glm::mat4 rotate = glm::mat4_cast(transform->GetRotation());
			glm::mat4 scale = glm::scale(transform->GetScale() * 1.25f);
			model = translate * rotate * scale;
			mvp = Projection * View * model;
			glEnableClientState(GL_VERTEX_ARRAY);
			// Uniform matrices ARE supplied in Row Major order hence set to GL_TRUE
			glUniformMatrix4fv(aMVPAttributeIndex, 1, GL_FALSE, &mvp[0][0]);
			check_gl_error_render();
			// Bind VAO
			glBindVertexArray(*DefaultVAOList[i]);
			check_gl_error_render();
			glDrawArrays(GL_TRIANGLES, 0, primitive->GetPrimitiveSize() / sizeof(Vertex));
			check_gl_error_render();
			assert(glGetError() == GL_NO_ERROR);
			// Unbind VAO when done
			glBindVertexArray(0);
			glDisableClientState(GL_VERTEX_ARRAY);
		}
	}
}

void Renderer::RenderDebugLines(GLint aMVPAttributeIndex)
{
	glm::mat4 projectionView = Projection * View;

	// Draw all Lines that have been registered 
	for (int i = 0; i < DebugLinesStack.size(); ++i)
	{
		glm::mat4 model;
		glm::mat4 translate = glm::translate(DebugLinesStack[i].PointA);
		// Create rotation matrix using the direction the line points in
		glm::vec3 normal = DebugLinesStack[i].PointA - DebugLinesStack[i].PointB;
		normal = glm::normalize(normal);
		glm::mat4 rotate = glm::orientation(normal, glm::vec3(1, 0, 0));
		glm::mat4 scale = glm::scale(glm::vec3(DebugLinesStack[i].Scale));
		model = translate * rotate * scale;
		projectionView = projectionView * model;
		glEnableClientState(GL_VERTEX_ARRAY);
		glUniformMatrix4fv(aMVPAttributeIndex, 1, GL_FALSE, &projectionView[0][0]);
		check_gl_error_render();
		glBindVertexArray(DebugLinePrimitive->GetVAO());
		check_gl_error_render();
		glDrawArrays(GL_TRIANGLES, 0, DebugLinesStack[i].VertexCount);
		check_gl_error_render();
		glBindVertexArray(0);
		glDisableClientState(GL_VERTEX_ARRAY);

	}
	DebugLinesStack.clear();
}

void Renderer::RenderDebugQuads(GLint aModelAttributeIndex, GLint aViewAttributeIndex, GLint aProjectionAttributeIndex, GLint aBillboardModeAttributeIndex)
{
	// Set projection and view matrices
	glUniformMatrix4fv(aProjectionAttributeIndex, 1, GL_FALSE, &Projection[0][0]);
	glUniformMatrix4fv(aViewAttributeIndex, 1, GL_FALSE, &View[0][0]);
	// Use cylindrical billboarding
	glUniform1i(aBillboardModeAttributeIndex, 0);

	// Draw all Quads that have been registered 
	for (int i = 0; i < DebugQuadsStack.size(); ++i)
	{
		glm::mat4 model;
		glm::mat4 translate = glm::translate(DebugQuadsStack[i].WorldPosition);
		glm::mat4 scale = glm::scale(glm::vec3(DebugQuadsStack[i].Scale));
		model = translate * scale;
		glEnableClientState(GL_VERTEX_ARRAY);
		glUniformMatrix4fv(aModelAttributeIndex, 1, GL_FALSE, &model[0][0]);
		check_gl_error_render();
		glBindVertexArray(DebugQuadPrimitive->GetVAO());
		check_gl_error_render();
		glDrawArrays(GL_TRIANGLES, 0, DebugQuadsStack[i].VertexCount);
		check_gl_error_render();
		glBindVertexArray(0);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
	DebugQuadsStack.clear();
}

void Renderer::OnNotify(Object * object, Event * event)
{
	// Check if this is an Engine event
	EngineEvent * engineEvent = nullptr;
	engineEvent = dynamic_cast<EngineEvent *>(event);
	
	if (engineEvent)
	{
		if(engineEvent->EventID == EngineEvent::EventList::ENGINE_INIT)
		{
			InititalizeRenderer();
			
			/*---------- MINKOWSKI DIFFERENCE INIT ----------*/
			MinkowskiDifference = EngineHandle.GetGameObjectFactory().SpawnGameObject();
			std::vector<Vertex> MinkowskiDifferenceVertices;
			Mesh * minkowskiMesh = EngineHandle.GetGameObjectFactory().SpawnComponent<Mesh>();
			minkowskiMesh->SetOwner(MinkowskiDifference);
			MinkowskiDifference->AddComponent(minkowskiMesh);

		}
		else if (engineEvent->EventID == EngineEvent::EventList::ENGINE_TICK)		
		{
			Render();
		}
		return;
	}

	PrimitiveEvent * primitiveEvent = nullptr;
	primitiveEvent = dynamic_cast<PrimitiveEvent *>(event);

	if(primitiveEvent)
	{
		if (engineEvent->EventID == PrimitiveEvent::EventList::TEXTURE_REQUEST)
		{
			// Already bound textures are not rebound
			if (primitiveEvent->TextureID >= TextureCount)
				BindTexture(static_cast<Primitive *>(object), primitiveEvent->TextureID);
		}
		return;
	}
}

void Renderer::RegisterDebugQuad(Quad & aQuad)
{
	DebugQuadsStack.push_back(aQuad);
}

// aPrimitive : Primitive the texture is being assigned to
// aTextureID : ID of the texture in the resource manager
// Binds a texture to the image data
bool Renderer::BindTexture(Primitive * aPrimitive, int aTextureID)
{
	Texture * assignedTexture = EngineHandle.GetResourceManager().GetTexture(aTextureID);
	
	GLsizei width = assignedTexture->GetWidth();
	GLsizei height = assignedTexture->GetHeight();

	// Sets the active texture unit using the enum values
	glActiveTexture(GL_TEXTURE0 + TextureCount);

	// Sets what type of texture it is and the texture buffer it is bound to
	glBindTexture(GL_TEXTURE_2D, *TBOList[TextureCount]);

	aPrimitive->SetTBO(*TBOList[TextureCount]);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, assignedTexture->GetPixels());
	glGenerateMipmap(GL_TEXTURE_2D);

	glUniform1i(glGetUniformLocation(DefaultShader.GetShaderProgram(), "Texture"), aPrimitive->GetTBO());
	// Sets the texture uniform in the shader
	check_gl_error_render();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	TextureCount++;

	return true;
}
