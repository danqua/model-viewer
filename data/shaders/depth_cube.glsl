#vertex
#version 450 core
layout (location = 0) in vec3 VertPosition;
layout (location = 1) in vec3 VertNormal;

uniform mat4 ModelMatrix;

out vec3 Normal;

void main()
{
	gl_Position = ModelMatrix * vec4(VertPosition, 1.0);
	Normal = VertNormal;
}

#geometry
#version 450 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 ShadowMatrices[6];

out vec4 FragPos; // FragPos from GS (output per emitvertex)
out vec3 Normal;

void main()
{
	for (int face = 0; face < 6; ++face)
	{
		gl_Layer = face; // built-in variable that specifies to which face we render
		for (int i = 0; i < 3; ++i)
		{
			FragPos = gl_in[i].gl_Position;
			gl_Position = ShadowMatrices[face] * FragPos;
			EmitVertex();
		}
		EndPrimitive();
	}
}

#fragment
#version 450 core
out vec4 FragColor;
in vec4 FragPos;
in vec3 Normal;

uniform vec3 LightPosition;
uniform float FarPlane;

void main()
{
	float lightDistance = length(FragPos.xyz - LightPosition);

	// map to [0:1] range by dividing by far_plane
	lightDistance = lightDistance / FarPlane;

	FragColor = vec4(normalize((Normal + 1.0) / 2.0), 1.0);
	gl_FragDepth = lightDistance;
}