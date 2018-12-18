#version 450
layout(binding = 0) uniform sampler2D Texture;
layout(binding = 1) uniform sampler2D NormalMap;
layout(binding = 2) uniform sampler2D SpecularMap;

// in  vec4 frag_color;
in  vec3 frag_position;
in  vec3 frag_normal;
in  vec2 frag_texcoord;
in  vec3 Tangent;
in  vec3 Bitangent;

out vec4 outColor;

uniform float EarthDegree;
uniform float light_rev_degree;

void main(){

	vec4 TextureColor  = texture2D(Texture, frag_texcoord);
	vec3 SpecularColor = texture2D(SpecularMap, frag_texcoord).rgb;

	mat3 TBN = mat3(Tangent, Bitangent, frag_normal);
	// mat3 TBN = mat3(Tangent*(-5), Bitangent*(-5), frag_normal);
	vec3 normal = texture(NormalMap, frag_texcoord).rgb;
	normal = normalize(normal * 2.0 - 1.0);   
	normal = normalize(TBN * normal); 

	const float PI = 3.1415926;
	float coef_amb  = 0.1; 
	float coef_diff = 1.0; 
	float coef_spec = 0.3;
	vec4 ambient  = vec4(0.7,  0.7, 0.7,  1.0);
	vec4 specular = vec4(1.0,  1.0, 1.0,  1.0);
	vec4 diffuse_brown = vec4(0.35, 0.3, 0.15, 1.0);

	/* Ambient Result */	
	vec4 AmbientResult = coef_amb * ambient;


	float earth_rotate_rad = EarthDegree * (PI/180.0); 
	float light_rev_rad = light_rev_degree * (PI/180.0);

	float theta = -earth_rotate_rad;
	vec3 col0 = vec3(cos(theta), 0.0, -sin(theta));
	vec3 col1 = vec3(0.0, 1.0, 0.0);
	vec3 col2 = vec3(sin(theta), 0.0, cos(theta));
	mat3 Anti_Earth_Rotate = mat3(col0, col1, col2);
	
	theta = light_rev_rad;
	col0 = vec3(cos(theta), 0.0, -sin(theta));
	col1 = vec3(0.0, 1.0, 0.0);
	col2 = vec3(sin(theta), 0.0, cos(theta));
	mat3 Light_Rev = mat3(col0, col1, col2);

	vec3  ViewPos  = vec3(0.0, 0.0, 3.0);
	vec3  LightPos = vec3(0.0, 0.0, 3.0);
	LightPos = Light_Rev * Anti_Earth_Rotate * LightPos;	
	ViewPos = Anti_Earth_Rotate * ViewPos;

	/* Diffuse Result */		
	vec3  LightDir = normalize(LightPos - frag_position);  
	float diff = max(dot(normal, LightDir), 0.0);
	vec4 DiffuseColor = TextureColor;  // if texture mapping is off, then set it to "diffuse_brown"
	vec4 DiffuseResult = coef_diff * diff * DiffuseColor;

	/* Specular Result */		
	vec3 ViewDir = normalize(ViewPos - frag_position);
	vec3 ReflectDir = reflect(-LightDir, normal);  
	float spec = pow(max(dot(ViewDir, ReflectDir), 0.0), 32);

	// vec4 SpecularResult = coef_spec * spec * specular; 
	vec4 SpecularResult = vec4(coef_spec * spec * SpecularColor * vec3(specular), 1.0); 

	// vec4 result = AmbientResult + DiffuseResult;
	vec4 result = AmbientResult + DiffuseResult + SpecularResult;
	outColor = result;
}