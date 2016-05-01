#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_shading_language_420pack  : enable

struct light_t {
	vec4 pos;
	vec3 att;
};

light_t lights[] = light_t[](
	light_t (vec4(  3,  4,  5, 1), vec3(0.2, 0.5, 0.01))
	//,light_t(vec3( 0, 5, 0), vec3(0.5, 0.1, 0.05))
	//,light_t(vec3( 7, 11, 7), vec3(0.1, 0.01, 0.02))
	//,light_t(vec3( 3,-4, 2), vec3(0.5, 0.1, 0.05))
	//,light_t(vec3( 7, 6, 7), vec3(0.1, 0.01, 0.02))
);

/* -------------------------------------------------------------------------- */
layout(location = 0) uniform mat4 VM;
layout(location = 1) uniform mat4 PVM;
layout(location = 2) uniform mat4 N;
layout(location = 3) uniform vec4 eye;
layout(location = 4) uniform vec4 light;
layout(location = 5) uniform int  enable_bump;
layout(location = 6) uniform int  enable_spec;

/* in clip coornates */
layout(location = 0) in vec4 v_pos;
layout(location = 1) in vec4 v_nor;
layout(location = 2) in vec4 v_tan;
layout(location = 3) in vec2 v_tex;

out vec3 f_light;
out vec3 f_view;
out vec2 f_tex;

void main() {
	gl_Position = PVM * v_pos;
	
	vec3 nor   = normalize(v_nor.xyz);
	vec3 tg    = normalize(v_tan.xyz);
	vec3 bit   = normalize(cross(nor, tg) * v_tan.w);
	mat3 f_TBN = transpose(mat3(tg, bit, nor));

	f_view  = f_TBN * (eye - v_pos).xyz;
	f_light = f_TBN * (light).xyz;
	f_tex   = v_tex;
}
