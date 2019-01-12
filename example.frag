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
uniform bool texture_switch;  // broken, doesnt know why
uniform bool normal_on;
uniform bool specular_on;
uniform	bool one_on;

void main(){
	const float PI = 3.1415926;
	float coef_amb  = 0.1; 
	float coef_diff = 1.0; 
	float coef_spec = 0.3;
	vec4 ambient  = vec4(0.7,  0.7, 0.7,  1.0);
	vec4 specular = vec4(1.0,  1.0, 1.0,  1.0);
	vec4 diffuse_brown = vec4(0.35, 0.3, 0.15, 1.0);

	vec4 TextureColor;
	vec3 SpecularColor;
	vec3 normal;

	TextureColor  = texture(Texture, frag_texcoord);

	if(!one_on){
		TextureColor = diffuse_brown;
	}

	if(specular_on)
		SpecularColor = texture2D(SpecularMap, frag_texcoord).rgb;
	else
		SpecularColor = vec3(1.0, 1.0, 1.0);

	if(normal_on){
		mat3 TBN = mat3(Tangent, Bitangent, frag_normal);
		normal = texture(NormalMap, frag_texcoord).rgb;
		normal.y = 1-normal.y;
		normal = normalize(normal * 2.0 - 1.0);   
		normal = normalize(TBN * normal); 
	}
	else
		normal = frag_normal;
	


	/* Ambient Result */	
	vec4 AmbientResult = coef_amb * ambient;


	/* Diffuse part */
	float earth_rotate_rad = EarthDegree * (PI/180.0); 
	float light_rev_rad = light_rev_degree * (PI/180.0);

	float theta = -light_rev_rad;
	vec3 col0 = vec3(cos(theta), 0.0, -sin(theta));
	vec3 col1 = vec3(0.0, 1.0, 0.0);
	vec3 col2 = vec3(sin(theta), 0.0, cos(theta));
	mat3 Light_Rev = mat3(col0, col1, col2);

	vec3 column0 = vec3( cos((23.5)*(PI/180.0)), sin((23.5)*(PI/180.0)), 0.0);
	vec3 column1 = vec3(-sin((23.5)*(PI/180.0)), cos((23.5)*(PI/180.0)), 0.0);
	vec3 column2 = vec3(0.0, 0.0, 1.0);
	mat3 AntiOblique = mat3(column0, column1, column2);

	vec3  ViewPos  = vec3(0.0, 0.0, 3.0);
	vec3  LightPos = vec3(0.0, 0.0, 20.0);
	
	// LightPos = AntiOblique * Light_Rev * LightPos;	
	// vec3 not_oblique_frag_position = AntiOblique * frag_position;
	// LightPos = LightPos;		
	vec3 not_oblique_frag_position = frag_position;


	/* Diffuse Result */			 
	vec3  LightDir = normalize(LightPos - not_oblique_frag_position);  	
	float diff = max(dot(normal, LightDir), 0.0);
	vec4 DiffuseColor = TextureColor; //texture map or brown
	vec4 DiffuseResult = coef_diff * diff * DiffuseColor;


	/* Specular Result */		
	vec3 ViewDir = normalize(ViewPos - not_oblique_frag_position);	
	vec3 ReflectDir = normalize(reflect(-LightDir, normal));  
	float spec = pow(max(dot(ViewDir, ReflectDir), 0.0), 16);
	vec4 SpecularResult = vec4(coef_spec * spec * SpecularColor * vec3(specular), 1.0); 

	/*Final Result*/
	// vec4 result = AmbientResult + DiffuseResult + SpecularResult;
	if(gl_FrontFacing)
        outColor = diffuse_brown;
	else{
		vec4 result = AmbientResult + DiffuseResult;
		outColor = result;
	}
}