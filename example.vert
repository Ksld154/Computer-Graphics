#version 450

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;


out vec3 geom_position;
out vec3 geom_normal;
out vec2 geom_texcoord;

uniform mat4 Projection;
uniform mat4 ModelView;
uniform float EarthDegree;


void main() {
	const float PI = 3.1415926;
	float theta = (PI/180.0) * (EarthDegree);
	
	vec3 trans_pos = pos;
	vec3 trans_nor = normal;

	vec3 col0 = vec3(cos(theta), 0.0, -sin(theta));
	vec3 col1 = vec3(0.0, 1.0, 0.0);
	vec3 col2 = vec3(sin(theta), 0.0, cos(theta));
	mat3 Rotate_itself = mat3(col0, col1, col2);

	vec3 column0 = vec3( cos((-23.5)*(PI/180.0)), sin((-23.5)*(PI/180.0)), 0.0);
	vec3 column1 = vec3(-sin((-23.5)*(PI/180.0)), cos((-23.5)*(PI/180.0)), 0.0);
	vec3 column2 = vec3(0.0, 0.0, 1.0);
	mat3 Oblique = mat3(column0, column1, column2);
	
	// trans_pos = Oblique * Rotate_itself * pos;
	// trans_nor = Rotate_itself * normal;
	trans_pos = pos;
	trans_nor = normal;
	
	gl_Position = Projection * ModelView * vec4(trans_pos, 1.0);

	geom_position = vec3(gl_Position);	
	geom_normal   = normalize(trans_nor);
	geom_texcoord = vec2(texcoord);
}