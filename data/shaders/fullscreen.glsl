#vertex
#version 450 core
layout (location = 0) in vec3 position;
layout (location = 2) in vec2 uv;

out VS_OUT
{
	vec2 uv;
} vs_out;

void main()
{
	gl_Position = vec4(position, 1.0);
	vs_out.uv = uv;
}

#fragment
#version 450 core
out vec4 fragColor;

in VS_OUT
{
	vec2 uv;
} fs_in;

uniform sampler2D DiffuseTexture;

void main()
{
	fragColor = texture(DiffuseTexture, fs_in.uv);
}