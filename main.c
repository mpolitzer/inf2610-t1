#include <limits.h>

#define TZ_WINDOW_INLINE
#include "tz/tz/gl/window.h"

#define TZ_SHADER_INLINE
#include "tz/tz/gl/shader.h"

#define TZ_SURF_INLINE
#include "tz/tz/gl/surf.h"

#define TZ_MAT4_INLINE
#include "tz/tz/math/mat4.h"

#define TZ_CAM_INLINE
#include "tz/tz/gl/cam.h"

#define TZ_TEX_INLINE
#include "tz/tz/gl/tex.h"

#define TZ_MSH_INLINE
#include "tz/tz/msh.h"


#define VERT_NAME   "../assets/shaders/main.vert"
#define FRAG_NAME   "../assets/shaders/main.frag"

//#define VERT_NAME   "../assets/shaders/main-nonbump.vert"
//#define FRAG_NAME   "../assets/shaders/main-nonbump.frag"

#define EYEBOT_MESH     "../assets/msh/eyebot.ply"
#define EYEBOT_NORMAL   "../assets/tex/eyebot-normal.tga"
#define EYEBOT_DIFFUSE  "../assets/tex/eyebot-diffuse.tga"
//#define EYEBOT_SPECULAR "../assets/tex/eyebot-specular-0.tga"
//#define EYEBOT_SPECULAR "../assets/tex/eyebot-specular-1.tga"
#define EYEBOT_SPECULAR "../assets/tex/eyebot-specular-2.tga"

#define CUBE_MESH      "../assets/msh/cube.ply"
#define CUBE_NORMAL    "../assets/tex/cube-normal.tga"
#define CUBE_DIFFUSE   "../assets/tex/cube-diffuse.tga"

//#define NORMAL_NAME "../assets/tex/normal2.tga"
//#define NORMAL_NAME "../assets/tex/planet-normals.tga"

//#define COLOR_NAME  "../assets/tex/mars_1k_color.tga"
//#define NORMAL_NAME "../assets/tex/mars_1k_normal.tga"

//#define COLOR_NAME  "../assets/tex/planet-diffuse-specular.tga"
//#define NORMAL_NAME "../assets/tex/planet-normals.tga"

//#define COLOR_NAME  "../assets/tex/earth-color.tga"
//#define NORMAL_NAME "../assets/tex/earth-normal.tga"

//#define COLOR_NAME  "../assets/tex/uv-test.tga"

//#define COLOR_NAME  "../assets/tex/moon-decalc.tga"
//#define NORMAL_NAME "../assets/tex/moon-normal.tga"

static float     _light_param=0;
static int       _enable_bump=0;
static int       _enable_spec=0;
static tz_window _window;
static tz_shader _shader;
static tz_cam    _cam;
static tz_msh    _eyebot_msh;
static tz_obj    _eyebot_obj;
static tz_obj    _eyebot_dbg;

static tz_msh    _cube_msh;
static tz_obj    _cube_obj;
static tz_obj    _cube_dbg;

static tz_tex     _color_map;
static tz_tex    _normal_map;
static tz_tex  _specular_map;

static tz_mat4   _view_matrix;
static tz_mat4   _proj_matrix;
static tz_mat4    _pvm_matrix;
static tz_mat4      _n_matrix;

static tz_vec4   _light;

static GLchar *readfile(const char *name) {
	char *data;
	size_t len;
	SDL_RWops *rw = SDL_RWFromFile(name, "rb");
	SDL_assert(rw && "failed to read file.");

	len = SDL_RWseek(rw, 0, SEEK_END);
	data = malloc(len+1);
	data[len] = '\0';

	SDL_RWseek(rw, 0, SEEK_SET);
	SDL_RWread(rw, data, len, 1);
	SDL_RWclose(rw);
	return data;
}

static void build_shader(tz_shader *s) {
	GLchar *file_contents;

	shader_create(s);
	if ((file_contents = readfile(VERT_NAME))) {
		shader_compile(s, file_contents, GL_VERTEX_SHADER);
		free(file_contents);
	}
	else SDL_assert("shader " VERT_NAME " not found");

	if ((file_contents = readfile(FRAG_NAME))) {
		shader_compile(s, file_contents, GL_FRAGMENT_SHADER);
		free(file_contents);
	}
	else SDL_assert("shader " FRAG_NAME " not found");

	shader_link(&_shader);
	shader_bind(&_shader);
}

void redraw(float t) {
	float tmp[4];
	tz_mat4 _model_matrix, _vm_matrix;
	tz_mat4 i_view_matrix;

	tz_mat4_mkidentity(&_model_matrix);

	tz_cam_mkview(&_cam, &_view_matrix);
	tz_vec4_store4fv(_cam.pos, tmp);
	glUniform4fv(3, 1, tmp);

	tz_mat4_inverse(&i_view_matrix, &_view_matrix);
	glUniformMatrix4fv(0, 1, GL_TRUE, _view_matrix.f);

	tz_mat4_mul(&_vm_matrix, &_view_matrix, &_model_matrix);
	tz_mat4_mul(&_pvm_matrix, &_proj_matrix, &_vm_matrix);
	glUniformMatrix4fv(1, 1, GL_TRUE,  _pvm_matrix.f);

	tz_mat4_inverse(&_n_matrix, &_model_matrix);
	tz_mat4_transpose(&_n_matrix, &_n_matrix);
	glUniformMatrix4fv(2, 1, GL_TRUE, _n_matrix.f);

	_light = tz_vec4_mkp(10*sin(0.5*t+M_PI), 5, 10*cos(0.5*t+M_PI));
	//tz_vec4 light = tz_mat4_mulv(&i_view_matrix, _light);
	tz_vec4 light = _light;

	tz_vec4_store4fv(light, tmp);
	glUniform4fv(4, 1, tmp);
	glUniform1i(5, _enable_bump);
	glUniform1i(6, _enable_spec);

	tz_obj_draw(&_eyebot_obj);
	//tz_obj_draw(&_eyebot_dbg);
	//tz_obj_draw(&_cube_obj);
	//tz_obj_draw(&_cube_dbg);
	tz_window_flip(&_window);
}

static uint32_t tick(uint32_t interval, void *param);

int main(int argc, char *argv[])
{
	int w = 800, h = 800;

	SDL_Init(SDL_INIT_EVERYTHING);
	if (tz_window_create(&_window, w, h, false) == false)
		return EXIT_FAILURE;

	if (tz_msh_read_ply(&_eyebot_msh, EYEBOT_MESH))
		return EXIT_FAILURE;

	tz_msh_gen_nor(&_eyebot_msh);
	tz_msh_gen_tan(&_eyebot_msh);
	//tz_msh_dbg_nor(&_eyebot_msh, &_eyebot_dbg);
	//tz_msh_dbg_tan(&_eyebot_msh, &_eyebot_dbg);
	tz_msh_buffer (&_eyebot_msh, &_eyebot_obj);

	if (tz_msh_read_ply(&_cube_msh, CUBE_MESH))
		return EXIT_FAILURE;

	//tz_msh_gen_nor(&_cube_msh);
	//tz_msh_gen_tan(&_cube_msh);
	////tz_msh_dbg_nor(&_cube_msh, &_cube_dbg);
	//tz_msh_dbg_tan(&_cube_msh, &_cube_dbg);
	//tz_msh_buffer (&_cube_msh, &_cube_obj);


	//glEnable(GL_BLEND);
	glViewport(0, 0, w, h);
	glClearColor(1, 0, 1, 1);
	glEnable(GL_PRIMITIVE_RESTART);
	//glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
	glPrimitiveRestartIndex(GL_PRIMITIVE_RESTART_FIXED_INDEX);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glClearColor(0.1,0.1,0.1,1);

	_cam.rho   = 3;
	_cam.phi   = M_PI/2;
	_cam.theta = 0;

	build_shader(&_shader);

	tz_mat4_perspective(&_proj_matrix, 45, h/(float)w, 0.1, 100);
	glUniformMatrix4fv(2, 1, GL_TRUE,  _proj_matrix.f);

	glActiveTexture(GL_TEXTURE0+0);
	if (!tz_tex_load_tga(&_color_map,  EYEBOT_DIFFUSE))
		return EXIT_FAILURE;

	glActiveTexture(GL_TEXTURE0+1);
	if (!tz_tex_load_tga(&_normal_map,  EYEBOT_NORMAL))
		return EXIT_FAILURE;

	glActiveTexture(GL_TEXTURE0+2);
	if (!tz_tex_load_tga(&_specular_map,  EYEBOT_SPECULAR))
		return EXIT_FAILURE;


	Uint32 delay = (33 / 10) * 10;
	SDL_TimerID tim = SDL_AddTimer(delay, tick, 0);
	(void)tim;

	SDL_Event e;
	while (SDL_WaitEvent(&e)) {
		switch (e.type) {
		case SDL_KEYUP: {
			if (e.key.keysym.sym == SDLK_ESCAPE) {
				goto finalize_program;
			}
			if (e.key.keysym.sym == SDLK_r) {
				shader_destroy(&_shader);
				build_shader(&_shader);
			}
			break;
		}
		case SDL_KEYDOWN: {
			if (e.key.keysym.sym == SDLK_v) {
				static bool line= false;

				line = !line;
				glPolygonMode(GL_FRONT_AND_BACK,
						line ? GL_LINE : GL_FILL);
			}
			if (e.key.keysym.sym == SDLK_e) {
				_light_param += 0.5;
			}
			if (e.key.keysym.sym == SDLK_q) {
				_light_param -= 0.5;
			}
			if (e.key.keysym.sym == SDLK_b) {
				_enable_bump ^= 1;
				printf("%d\n", _enable_bump);
			}
			if (e.key.keysym.sym == SDLK_p) {
				_enable_spec ^= 1;
				printf("%d\n", _enable_spec);
			}
			break;
		}
		case SDL_MOUSEMOTION: {
			int x, y;
			if (SDL_GetMouseState(&x, &y)) {
				_cam.theta += 2*e.motion.xrel/800.0;
				_cam.phi   -= 2*e.motion.yrel/800.0;
				//printf("%f %f\n", _cam.theta, _cam.phi);
			}
			break;
		}
		case SDL_MOUSEWHEEL: {
			float inc = SDL_GetModState() ? 0.1 : 1;
			_cam.rho += inc*e.wheel.y;
			printf("%f\n", _cam.rho);
			printf("%d\n", e.wheel.y);
			break;
		}
		case SDL_USEREVENT: {
			redraw(_light_param);
			//redraw(4.0+2*SDL_GetTicks()/1000.0);
			break;
		}}
	}

finalize_program:
	shader_destroy(&_shader);
	SDL_Quit();
	return EXIT_SUCCESS;
}

static uint32_t tick(uint32_t interval, void *param)
{
	SDL_Event event;
	SDL_UserEvent userevent;

	userevent.type = SDL_USEREVENT;
	userevent.code = 0;
	userevent.data1 = NULL;
	userevent.data2 = NULL;

	event.type = SDL_USEREVENT;
	event.user = userevent;

	SDL_PushEvent(&event);
	return(interval);
}

