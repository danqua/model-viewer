#vertex
#version 450 core
layout (location = 0) in vec3 VertPosition;
layout (location = 1) in vec3 VertNormal;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;

out vec3 Normal;

void main()
{
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(VertPosition, 1.0);
	Normal = VertNormal;
}

#fragment
#version 450 core
out vec4 FragColor;

in vec3 Normal;

void main()
{
	FragColor = vec4(normalize((Normal + 1.0) / 2.0), 1.0);
}