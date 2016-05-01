#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_shading_language_420pack  : enable

struct material_t {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

material_t materials[] = material_t[](
	material_t( // wut
		vec3(0.05, 0.05, 0.05),
		vec3(0.50, 0.50, 0.50),
		vec3(1.00, 1.00, 1.00), 10.0),
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

layout(binding  = 0) uniform sampler2D  diffuse_map;
layout(binding  = 1) uniform sampler2D   normal_map;
layout(binding  = 2) uniform sampler2D specular_map;

in vec3 f_view;
in vec3 f_light;
in vec2 f_tex;

out vec4 o_color;

void main()
{
	int mi = 0;
	
	float l_len = length(f_light);
	float att   = 1/dot(vec3(1, l_len, l_len*l_len), vec3(0.1, 0.05, 0.01));

	vec4 diffuse = texture(diffuse_map, f_tex);
	vec4 normal  = texture(normal_map, f_tex);
	vec4 specular= texture(specular_map, f_tex);

	vec3 n = vec3(0,0,1);
	if (enable_bump != 0)
		n = 2*normal.rgb-1;
	
	vec3 s = vec3(1,1,1);
	if (enable_spec != 0)
		s = specular.rgb;

	vec3 l     = normalize(f_light);
	vec3 v     = normalize(f_view);

	vec3 diff  = vec3(0,0,0),
	     spec  = vec3(0,0,0);

	// ambient
	//diff += att * materials[mi].ambient;

	float ndotl = max(dot(n,l), 0);
	// diffuse
	diff += att * ndotl;

	// specular
	vec3 refl = reflect(-l, n);
	float angle = max(dot(refl,v), 0.0);
	float expo  = materials[mi].shininess;
	spec += att*materials[mi].specular*pow(angle, expo);

	o_color.rgb = diff * diffuse.rgb + spec * s.rgb;
	o_color.a   = 1;
}

