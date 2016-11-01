#version 450 core
in vec4 ex_color;
in vec2 Texcoord;
out vec4 out_color;
uniform sampler2D Texture;
void main()
{
	out_color = /*texture(Texture, Texcoord) **/ vec4(ex_color.x, ex_color.y, ex_color.z, 1.0);
}