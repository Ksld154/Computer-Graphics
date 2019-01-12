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

uniform float EarthDegree;
uniform int timeValue;

void Calcute_Tan_Bitan(){

	vec3 e1 = geom_position[1].xyz - geom_position[0].xyz;
	vec3 e2 = geom_position[2].xyz - geom_position[0].xyz;

	vec2 delta_uv1 = geom_texcoord[1] - geom_texcoord[0];
	vec2 delta_uv2 = geom_texcoord[2] - geom_texcoord[0];
	float c = 1.0 / ((delta_uv1.x * delta_uv2.y) - (delta_uv1.y * delta_uv2.x));

	Tangent.x = c * (delta_uv2.y*e1.x - delta_uv1.y*e2.x);  // dv2*e1x - dv1*e2x
	Tangent.y = c * (delta_uv2.y*e1.y - delta_uv1.y*e2.y);
	Tangent.z = c * (delta_uv2.y*e1.z - delta_uv1.y*e2.z);
	// Tangent = normalize(Tangent);
	
	Bitangent.x = c * (-delta_uv2.x*e1.x + delta_uv1.x*e2.x);  // -du2*e1x + du1*e2x
	Bitangent.y = c * (-delta_uv2.x*e1.y + delta_uv1.x*e2.y);
	Bitangent.z = c * (-delta_uv2.x*e1.z + delta_uv1.x*e2.z);
	// Bitangent = normalize(Bitangent);
}

vec3 GetNormal(){
    // 计算平行于三角形图元平面的向量：a
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    // 计算平行于三角形图元平面的向量：b
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    // 通过叉乘计算三角形图元的法向量
    return normalize(cross(a, b));
}

/**
 计算(三角形图元)爆破后的位置向量
 
 @param position 顶点位置向量
 @param normal 顶点法向量
 @return 顶点爆破后的位置向量
*/

vec4 explode(vec4 position, vec3 normal){
    // float magnitude = 0.5;  // 爆破级别
    // 爆破距离
    // vec3 direction = normal * ((sin(timeValue) + 1.0) / 2.0) * magnitude;
    // vec3 direction = EarthDegree * 0.01;
    // float magnitude = ((sin(time) + 1) / 2.0f) * 2.0f; // 使位移偏量保持在[0, 2.0f]范围内
	
	float magnitude = EarthDegree * 0.03;
  	vec4 offset = vec4(normal * magnitude, 0.0f);
	
	
	// 沿着法向量位移(爆破)距离
    // return position + vec4(direction, 0.0);
    return position + offset;
}

void main() {
	vec3 normal = GetNormal();

	for(int i = 0; i < gl_in.length(); i++){
		// gl_Position = gl_in[i].gl_Position;
		gl_Position = explode(gl_in[i].gl_Position, normal);

		// frag_position = geom_position[i];
		// frag_normal   = geom_normal[i];

		frag_position = vec3(gl_Position);
		frag_normal   = normal;
		frag_texcoord = geom_texcoord[i];
		
		Calcute_Tan_Bitan();
		
		EmitVertex();
	}
	EndPrimitive();
}