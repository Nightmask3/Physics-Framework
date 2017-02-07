#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "Renderer.h"

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

bool Renderer::Render()
{
	// Update camera values before constructing view matrix
	glm::vec3 cameraPosition = pActiveCamera->GetCameraPosition();
	glm::vec3 cameraTarget = pActiveCamera->GetCameraLookDirection();
	glm::vec3 upVector = pActiveCamera->GetCameraUpDirection();

	/*-------------------------------- VIEW MATRIX -------------------------------*/
	glm::mat4 view;
	view = glm::lookAt(
		cameraPosition, 
		cameraTarget,   
		upVector    
	);

	// Update field of view before constructing projection matrix
	FieldOfView += InputManager::GetScrollDelta().y * pActiveCamera->GetSensitivity();

	/*-------------------------------- PROJECTION MATRIX -------------------------------*/
	// Creates the projection matrix
	glm::mat4 projection;
 	projection = glm::perspective(
		FieldOfView, // The horizontal Field of View, in degrees : the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
		4.0f / 3.0f, // Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960, sounds familiar ?
		0.1f,        // Near clipping plane. Keep as big as possible, or you'll get precision issues.
		100.0f       // Far clipping plane. Keep as little as possible.
	);

	// Sets background color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Clear color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/*-------------------------------- REGULAR MESH RENDER-------------------------------*/
	DefaultShader.Use();

	// Get the Model Matrix
	GLint glModel = glGetUniformLocation(DefaultShader.GetShaderProgram(), "model");
	// Get the View Matrix
	GLint glView = glGetUniformLocation(DefaultShader.GetShaderProgram(), "view");
	// Get the Projection Matrix
	GLint glProjection = glGetUniformLocation(DefaultShader.GetShaderProgram(), "projection");

	// Set view and projection matrix
	glUniformMatrix4fv(glView, 1, GL_FALSE, &(view[0][0]));
	glUniformMatrix4fv(glProjection, 1, GL_FALSE, &(projection[0][0]));

	for (int i = 0; i < RenderList.size(); ++i)
	{
		Transform * transform = nullptr;
		Primitive * primitive = RenderList[i];
						
		GameObject const * renderObject = primitive->GetConstOwner(); 
		transform = renderObject->GetComponent<Transform>();
		glm::mat4 model;
		glm::mat4 translate, scale;
		translate = glm::translate(transform->GetPosition());
		scale = glm::scale(transform->GetScale());
		model = translate * scale;
		// Uniform matrices ARE supplied in Row Major order hence set to GL_TRUE
		glUniformMatrix4fv(glModel, 1, GL_FALSE, &model[0][0]);
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

	/*-------------------------------- DEBUG MESH RENDER-------------------------------*/
	if (EngineHandle.GetInputManager().isKeyPressed(GLFW_KEY_TAB))
	{
		DebugShader.Use();

		// Get the Model Matrix
		GLint glModel = glGetUniformLocation(DebugShader.GetShaderProgram(), "model");
		// Get the View Matrix
		GLint glView = glGetUniformLocation(DebugShader.GetShaderProgram(), "view");
		// Get the Projection Matrix
		GLint glProjection = glGetUniformLocation(DebugShader.GetShaderProgram(), "projection");

		// Set view and projection matrix
		glUniformMatrix4fv(glView, 1, GL_FALSE, &(view[0][0]));
		glUniformMatrix4fv(glProjection, 1, GL_FALSE, &(projection[0][0]));

		for (int i = 0; i < RenderList.size(); ++i)
		{
			Transform * transform = nullptr;
			Primitive * primitive = RenderList[i];

			GameObject const * renderObject = primitive->GetConstOwner();
			transform = renderObject->GetComponent<Transform>();
			glm::mat4 model;
			glm::mat4 translate, scale;
			translate = glm::translate(transform->GetPosition());
			scale = glm::scale(transform->GetScale() * 1.25f);
			model = translate * scale;
			// Uniform matrices ARE supplied in Row Major order hence set to GL_TRUE
			glUniformMatrix4fv(glModel, 1, GL_FALSE, &model[0][0]);
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
	return true;
}

void Renderer::OnNotify(Object * object, Event * event)
{
	// Check if this is an Engine event
	EngineEvent * engineEvent = nullptr;
	engineEvent = dynamic_cast<EngineEvent *>(event);
	
	if (engineEvent)
	{
		switch (engineEvent->EventID)
		{
			case EngineEvent::EventList::ENGINE_INIT:
			{
				InititalizeRenderer();
			}
			break;
			case EngineEvent::EventList::ENGINE_TICK:
			{
				Render();
			}
			break;
		}
	}

	PrimitiveEvent * primitiveEvent = nullptr;
	primitiveEvent = dynamic_cast<PrimitiveEvent *>(event);

	if(primitiveEvent)
	{
		switch (primitiveEvent->EventID)
		{
			case PrimitiveEvent::EventList::TEXTURE_REQUEST:
			{
				// Already bound textures are not rebound
				if (primitiveEvent->TextureID >= TextureCount)
					BindTexture(static_cast<Primitive *>(object), primitiveEvent->TextureID);
			}
		}
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
