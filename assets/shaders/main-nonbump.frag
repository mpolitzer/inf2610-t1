#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_shading_language_420pack  : enable

#define MAX_LIGHTS 8

struct light_t {
	vec3 pos;
};

light_t lights[] = light_t[](
	light_t(vec3(5, 5, 5))
	//,
);

struct material_t {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

material_t materials[] = material_t[](
	material_t( // wut
		vec3(0.50, 0.50, 0.50),
		vec3(0.50, 0.50, 0.50),
		vec3(0.50, 0.50, 0.50), 10.0),
	material_t( // copper
		vec3(0.21, 0.13, 0.05),
		vec3(0.71, 0.43, 0.18),
		vec3(0.39, 0.27, 0.17), 25.6),
	material_t( // obsidian
		vec3(0.05, 0.05, 0.06),
		vec3(0.18, 0.17, 0.22),
		vec3(0.33, 0.32, 0.34), 38.4)
);

/* -------------------------------------------------------------------------- */
layout(location = 0) uniform mat4 VM;
layout(location = 1) uniform mat4 PVM;
layout(location = 2) uniform mat4 N;
layout(location = 3) uniform vec4 eye;
layout(location = 4) uniform vec4 light;
layout(location = 5) uniform int  enable_bump;
layout(location = 6) uniform int  enable_spec;


layout(binding  = 0) uniform sampler2D diffuse_texture;
layout(binding  = 1) uniform sampler2D normal_texture;
layout(binding  = 2) uniform sampler2D specular_texture;

in vec3 f_view;
in vec3 f_light;
in vec3 f_normal;
in vec2 f_tex;

out vec4 o_color;

void main()
{
	int mi = 0;

	vec4 diffuse = texture(diffuse_texture,  f_tex);
	vec4 specular= texture(specular_texture, f_tex);

	vec3 dif = vec3(0,0,0),
		 spec= vec3(0,0,0);

	vec3 v = normalize(f_view.xyz);
	vec3 n = normalize(f_normal);
	vec3 l = normalize(light.xyz);

	float l_len = length(light.xyz);
	float att   = 1/dot(vec3(1, l_len, l_len*l_len), vec3(.1, .01, .01));
	//float att   = 1;

	float ndotl = max(dot(n,l), 0);
	dif += att*materials[mi].diffuse*ndotl;

	if (ndotl > 0) {
		vec3  r     = reflect(-l,n);
		float angle = max(dot(r, v), 0);
		spec += att*materials[mi].specular*pow(angle,materials[mi].shininess);
	}

	//o_color.rgb = dif*diffuse.rgb + spec*specular.r;
	o_color.rgb = dif*diffuse.rgb + spec;
	o_color.a   = 1;
}

