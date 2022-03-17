#vertex
#version 450 core
layout (location = 0) in vec3 VertPosition;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
out vec3 FragPosition;

void main()
{
	FragPosition = VertPosition;
	gl_Position = ProjectionMatrix * mat4(mat3(ViewMatrix)) * vec4(VertPosition, 1.0);
}

#fragment
#version 450 core

out vec4 FragColor;

in vec3 FragPosition;

uniform samplerCube Cubemap;

void main()
{
	FragColor = texture(Cubemap, FragPosition);
}