#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) out vec2 outPosition;
layout (location = 1) out vec2 outTexcoord;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main() 
{
	outTexcoord = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
    outPosition = vec2(outTexcoord * 2.0f - 1.0f);
	gl_Position = vec4(outPosition, 0.0f, 1.0f);
}
