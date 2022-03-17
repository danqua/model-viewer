#vertex
#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out VS_OUT
{
	vec3 normal;
} vs_out;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0);
	vs_out.normal = normal;
}

#fragment
#version 450 core
out vec4 fragColor;

in VS_OUT
{
	vec3 normal;
} fs_in;

void main()
{
	fragColor = vec4(normalize((fs_in.normal + 1.0) / 2.0), 1.0);
}