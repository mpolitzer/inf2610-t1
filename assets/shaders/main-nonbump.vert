#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_shading_language_420pack  : enable

#define MAX_LIGHTS 8

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

out vec3 f_view;
out vec3 f_normal;
out vec2 f_tex;

void main() {
	gl_Position = PVM * v_pos;

	f_view  = ((eye - v_pos)).xyz;
	f_normal= (N  *  (v_nor)).xyz;
	f_tex   = v_tex;
}
