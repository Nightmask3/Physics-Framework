#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "Renderer.h"
#include "GameObjectFactory.h"
#include "GameObject.h"
#include "UtilityFunctions.h"


void Renderer::RegisterPrimitive(Primitive * aNewPrimitive)
{
	*VAOList[RenderList.size()] = *VBOList[RenderList.size()] = (GLuint)(RenderList.size() + 1);
	RenderList.push_back(aNewPrimitive);
	aNewPrimitive->TextureRequest.AddObserver(this);
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
	for (int i = 0; i < MAXIMUM_SPRITES; i++)
	{
		// Allocates the vertex data objects
		VAOList[i] = new GLuint;
		glGenVertexArrays(1, VAOList[i]);
		VBOList[i] = new GLuint;
		glGenBuffers(1, VBOList[i]);
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
	// Create and use debug shader program
	DefaultShader.CreateDefaultShaderProgram();
	DefaultShader.Use();

	DebugShader.CreateDebugShaderProgram();
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
	
	for (int i = 0; i < RenderList.size(); ++i)
	{
		Transform * transform = nullptr;
		Primitive * primitive = RenderList[i];
						
		GameObject const * renderObject = primitive->GetConstOwner(); 
		transform = renderObject->GetComponent<Transform>();
		// Calculate the MVP matrix and set the matrix uniform
		glm::mat4 mvp;
		glm::mat4 model;
		glm::mat4 translate, scale;
		translate = glm::translate(transform->GetPosition());
		scale = glm::scale(transform->GetScale());
		model = translate * scale;
		mvp = Projection * View * model;
		// Uniform matrices ARE supplied in Row Major order hence set to GL_TRUE
		glUniformMatrix4fv(glModelViewProjection, 1, GL_FALSE, &mvp[0][0]);
		check_gl_error_render();
		// Bind TBO
		glBindTexture(GL_TEXTURE_2D, primitive->GetTBO());
		// Bind VAO
		glBindVertexArray(*VAOList[i]);
		check_gl_error_render();
		glDrawArrays(GL_TRIANGLES, 0, primitive->GetPrimitiveSize()/sizeof(Vertex));
		check_gl_error_render();
		assert(glGetError() == GL_NO_ERROR);
		// Unbind VAO when done
		glBindVertexArray(0);
	}
	return;
}

void Renderer::DebugRenderPass()
{
	
	/*-------------------------------- DEBUG MESH RENDER-------------------------------*/
	DebugShader.Use();
	
	// Get the MVP Matrix id
	GLint glModelViewProjection = glGetUniformLocation(DebugShader.GetShaderProgram(), "ModelViewProjectionMatrix");

	Mesh * shape1 = static_cast<Mesh *>(RenderList[2]);
	Mesh * shape2 = static_cast<Mesh *>(RenderList[3]);

	int size1 = shape1->Vertices.size();
	int size2 = shape2->Vertices.size();
	std::vector<Vertex> MinkowskiDifferenceVertices;

	for (int i = 0; i < size1; ++i)
	{
		// Calculate the model matrices and set the matrix uniform
		glm::mat4 model1, model2;
		glm::mat4 translate, scale;
		translate = glm::translate(shape1->GetOwner()->GetComponent<Transform>()->GetPosition());
		scale = glm::scale(shape1->GetOwner()->GetComponent<Transform>()->GetScale());
		model1 = translate * scale;

		translate = glm::translate(shape2->GetOwner()->GetComponent<Transform>()->GetPosition());
		scale = glm::scale(shape2->GetOwner()->GetComponent<Transform>()->GetScale());
		model2 = translate * scale;

		glm::vec4 position1 = model1 * glm::vec4(shape1->Vertices[i].Position, 1);
		glm::vec4 position2 = model2 * glm::vec4(shape2->Vertices[i].Position, 1);
		Vertex newVertex;
		newVertex.Position = glm::vec3(position2 - position1);
		MinkowskiDifferenceVertices.push_back(newVertex);
	}

	// Sort Minkowski Difference vertices
	// https://stackoverflow.com/questions/6880899/sort-a-set-of-3-d-points-in-clockwise-counter-clockwise-order
	glm::vec3 zAxis = glm::vec3(0, 0, 1);
	glm::vec3 xAxis = glm::vec3(1, 0, 0);

	std::vector<std::pair<int, float>> order(MinkowskiDifferenceVertices.size());

	for (int i = 0; i < MinkowskiDifferenceVertices.size(); ++i)
	{
		float zAngle = glm::dot(MinkowskiDifferenceVertices[i].Position, zAxis);
		float xAngle = glm::dot(MinkowskiDifferenceVertices[i].Position, xAxis);
		float angle = atan2(zAngle, xAngle);
		order[i] = std::make_pair(i, angle);
	}

	std::sort(order.begin(), order.end(), Utility::FloatOrdering());
	Utility::SortUsingOrderingPairs(MinkowskiDifferenceVertices, order);
	MinkowskiDifference->GetComponent<Mesh>()->SetVertices(MinkowskiDifferenceVertices);

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
			glm::mat4 translate, scale;
			translate = glm::translate(transform->GetPosition());
			scale = glm::scale(transform->GetScale() * 1.25f);
			model = translate * scale;
			mvp = Projection * View * model;
			// Uniform matrices ARE supplied in Row Major order hence set to GL_TRUE
			glUniformMatrix4fv(glModelViewProjection, 1, GL_FALSE, &mvp[0][0]);
			check_gl_error_render();
			// Bind VAO
			glBindVertexArray(*VAOList[i]);
			check_gl_error_render();
			glDrawArrays(GL_TRIANGLES, 0, primitive->GetPrimitiveSize() / sizeof(Vertex));
			check_gl_error_render();
			assert(glGetError() == GL_NO_ERROR);
			// Unbind VAO when done
			glBindVertexArray(0);
		}
	}

	//RenderDebugLines();
}

void Renderer::RenderDebugLines()
{
	glLineWidth(2.5);
	glColor3f(1.0, 0.0, 0.0);
	// Draw lines between all points that have been registered 
	for (int i = 0; i < DebugLinePointList.size(); ++i)
	{
		glm::vec3 startingPoint = DebugLinePointList[i].first;
		glm::vec3 endingPoint = DebugLinePointList[i].second;

		glBegin(GL_LINES);
		glVertex3f(startingPoint.x, startingPoint.y, startingPoint.z);
		glVertex3f(endingPoint.x, endingPoint.y, endingPoint.z);
		glEnd();
	}

	if (EngineHandle.GetInputManager().isKeyPressed(GLFW_KEY_TAB) == false)
	{
		DebugLinePointList.clear();
	}
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
