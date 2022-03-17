#vertex
#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out VS_OUT {
	vec3 normal;
	vec2 uv;
} vs_out;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0);
	vs_out.normal = normal;
	vs_out.uv = uv;
}

#fragment
#version 450 core
out vec4 fragColor;

uniform sampler2D diffuseTexture;

in VS_OUT {
	vec3 normal;
	vec2 uv;
} fs_in;

void main()
{
	float gamma = 2.2;
	fragColor = texture(diffuseTexture, fs_in.uv);
	fragColor.rgb = pow(fragColor.rgb, vec3(1.0 / gamma));
}