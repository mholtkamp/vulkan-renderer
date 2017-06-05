#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform VSUniformBuffer 
{
    mat4 model;
    mat4 view;
    mat4 projection;
} ubo;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexcoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexcoord;

out gl_PerVertex 
{
    vec4 gl_Position;
};

void main()
{
    gl_Position = ubo.projection * ubo.view * ubo.model * vec4(inPosition, 0.0, 1.0);
    fragColor = inColor;
    fragTexcoord = inTexcoord;
}