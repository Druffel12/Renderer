#version 430

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 normal;
layout (location = 2) in vec2 uv;

layout (location = 0) uniform mat4 proj;
layout (location = 1) uniform mat4 view;
layout (location = 2) uniform mat4 model;

out vec2 vUV;
out vec3 vNormal;

void main()
{
	gl_Position = proj * view * model * position;
	vUV = uv;
	vNormal = normalize(model * normal).xyz;
};