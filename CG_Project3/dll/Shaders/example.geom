#version 450

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;
// layout(binding = 1) uniform sampler2D NormalMap;

in  vec3 geom_position[3];
in  vec3 geom_normal[3];
in  vec2 geom_texcoord[3];
// in  vec4 geom_color[3];

out vec3 frag_position;
out vec2 frag_texcoord;
out vec3 frag_normal;
out vec3 Tangent;
out vec3 Bitangent;
// out mat3 TBN;

void Calcute_Tan_Bitan(){

	vec3 e1 = geom_position[1].xyz - geom_position[0].xyz;
	vec3 e2 = geom_position[2].xyz - geom_position[0].xyz;

	// vec3 e1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	// vec3 e2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;

	vec2 delta_uv1 = geom_texcoord[1] - geom_texcoord[0];
	vec2 delta_uv2 = geom_texcoord[2] - geom_texcoord[0];
	float c = 1.0 / ((delta_uv1.x * delta_uv2.y) - (delta_uv1.y * delta_uv2.x));

	// Tangent = c * ((delta_uv2.y * e1)  - (delta_uv1.y * e2));
	Tangent.x = c * (delta_uv2.y*e1.x - delta_uv1.y*e2.x);  // dv2*e1x - dv1*e2x
	Tangent.y = c * (delta_uv2.y*e1.y - delta_uv1.y*e2.y);
	Tangent.z = c * (delta_uv2.y*e1.z - delta_uv1.y*e2.z);
	
	// Tangent = normalize(Tangent);
	
	// Bitangent = c * ((-delta_uv2.x * e1) + (delta_uv1.x * e2));
	Bitangent.x = c * (-delta_uv2.x*e1.x + delta_uv1.x*e2.x);  // -du2*e1x + du1*e2x
	Bitangent.y = c * (-delta_uv2.x*e1.y + delta_uv1.x*e2.y);
	Bitangent.z = c * (-delta_uv2.x*e1.z + delta_uv1.x*e2.z);
	// Bitangent = normalize(Bitangent);
}


void main() {


	for(int i = 0; i < gl_in.length(); i++){
		gl_Position = gl_in[i].gl_Position;
		// gl_Position = vec4(geom_position[i], 1.0);

		frag_position = geom_position[i];
		frag_normal   = geom_normal[i];
		frag_texcoord = geom_texcoord[i];
		
		Calcute_Tan_Bitan();
		
		EmitVertex();
	}
	EndPrimitive();
}