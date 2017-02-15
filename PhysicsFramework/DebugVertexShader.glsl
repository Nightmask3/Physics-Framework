#version 450 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 in_color;
layout(location = 3) in vec2 texcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 VertexColor;
out vec3 VertexNormal;
out vec2 VertexUV;
void main()
{
	gl_Position = projection * view * model * vec4(position.x, position.y, position.z, 1.0); // Vertex information so it is definitely a point hence, w can always be 1
	VertexColor = in_color;
	VertexUV = texcoord;
}