#version 430

layout (location = 3) uniform sampler2D albedo;
layout (location = 4) uniform vec3 lightDir;

in vec2 vUV;
in vec3 vNormal;

out vec4 outColor;

void main()
{
	
	float d = max(0, dot(vNormal, -lightDir));
	outColor = (texture(albedo, vUV));
	outColor = vec4(outColor.x * d, outColor.y * d, outColor.z * d, 1);
};