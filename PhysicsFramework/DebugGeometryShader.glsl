#version 330 compatibility
layout(triangles) in;
layout(line_strip, max_vertices=4) out;

in vec4 VertexColor[];
in vec2 VertexUV[];

out vec4 GeometryColor;
out vec2 GeometryUV;

void main()
{	
  GeometryColor = VertexColor[0].xyzw;
  GeometryUV = VertexUV[0].xy;

  for(int i=0; i<3; i++)
  {
    gl_Position = gl_in[i].gl_Position;
    EmitVertex();
  }

  EndPrimitive();
}  