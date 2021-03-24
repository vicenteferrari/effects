//
// Created by vfs on 11/30/20.
//

#include <stdio.h>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <cglm/struct.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui/cimgui.h"

#include "common.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "imgui_impl_sdl.c"
#include "imgui_impl_opengl3.c"
#include "allocator.c"
#include "camera.c"
#include "shader.c"
#include "lights.c"
#include "model.c"
#include "skybox.c"
#include "floor.c"
#include "unit.c"
#include "building.c"
#include "tile.c"
#include "map.c"

SDL_Window *window;
SDL_GLContext context;

bool running = true;
const u8 *keyboard_state;
s32 mouse_x, mouse_y;
bool relative_mouse;

u64 frequency;

void glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity,
				   GLsizei length, const char *message, const void *userParam) {

	// ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

	printf("---------------\n");
	printf("Debug message (%d): %s\n", id, message);

	switch (source) {
		case GL_DEBUG_SOURCE_API:
			printf("Source: API");
			break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			printf("Source: Window System");
			break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			printf("Source: Shader Compiler");
			break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:
			printf("Source: Third Party");
			break;
		case GL_DEBUG_SOURCE_APPLICATION:
			printf("Source: Application");
			break;
		case GL_DEBUG_SOURCE_OTHER:
			printf("Source: Other");
			break;
	}

	printf("\n");

	switch (type) {
		case GL_DEBUG_TYPE_ERROR:
			printf("Type: Error");
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			printf("Type: Deprecated Behaviour");
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			printf("Type: Undefined Behaviour");
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			printf("Type: Portability");
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			printf("Type: Performance");
			break;
		case GL_DEBUG_TYPE_MARKER:
			printf("Type: Marker");
			break;
		case GL_DEBUG_TYPE_PUSH_GROUP:
			printf("Type: Push Group");
			break;
		case GL_DEBUG_TYPE_POP_GROUP:
			printf("Type: Pop Group");
			break;
		case GL_DEBUG_TYPE_OTHER:
			printf("Type: Other");
			break;
	}

	printf("\n");

	switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH:
			printf("Severity: high");
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			printf("Severity: medium");
			break;
		case GL_DEBUG_SEVERITY_LOW:
			printf("Severity: low");
			break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			printf("Severity: notification");
			break;
	}

	printf("\n\n");
}

void init();

int main(int argc, char *argv[]) {
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);
	printf("Hello, sailor!\n");

	init();

	Floor floor;
	floor_init(&floor);
//
	Model model;
	model_init(&model, "models/backpack", "backpack.obj");
//
	Skybox skybox;
	skybox_init(&skybox);
//
	Camera camera;
	camera_init(&camera);
//
	DirectionalLight directional_light = {};
	directional_light_init(&directional_light);

	PointLight point_lights[MAX_POINT_LIGHTS];
	point_lights[0] = (PointLight) {
			{1.0f, 1.0f, 3.0f},
			{1.0f, 1.0f, 1.0f},
			0.1f,
			0.7f,
			1.0f,
			1.0f,
			0.09f,
			0.032f
	};

	SpotLight spot_light = {
			camera.pos,
			{1.0f, 1.0f, 1.0f},
			camera.dir,
			cosf(glm_rad(12.5f)),
			cosf(glm_rad(17.5f)),
			0.1f,
			0.7f,
			1.0f,
			1.0f,
			0.09f,
			0.032f
	};

//	point_lights[1] = (PointLight) {
//			{1.0f, 1.0f, 1.0f},
//			0.1f,
//			0.7f,
//			{0.0f, 2.0f, 1.0f},
//			1.0f,
//			1.0f,
//			1.0f
//	};
//
	Shader floor_shader;
	shader_init(&floor_shader, "shaders/floor.vs", "shaders/floor.fs");
//
	Shader skybox_shader;
	shader_init(&skybox_shader, "shaders/skybox.vs", "shaders/skybox.fs");
//
	Shader shader;
	shader_init(&shader, "shaders/blinn-phong-shadow.vs", "shaders/blinn-phong-shadow.fs");
	shader_bind(&shader);
//
	set_directional_light(&directional_light, &shader);
	set_point_light(&point_lights[0], &shader);
	set_spot_light(&spot_light, &shader);
//
	set_directional_light(&directional_light, &floor_shader);
	set_point_light(&point_lights[0], &floor_shader);
	set_spot_light(&spot_light, &floor_shader);
//
	Shader shadow_shader;
	shader_init(&shadow_shader, "shaders/shadow.vs", "shaders/shadow.fs");

//	Map test_map;
//	map_init(&test_map, "maps", "test_map.png");

	while (running) {
		u64 frame_time_old = SDL_GetPerformanceCounter();

		SDL_Event e;
		while (SDL_PollEvent(&e)) {

			ImGui_ImplSDL2_ProcessEvent(&e);
			switch (e.type) {
				case SDL_KEYDOWN: {
					switch (e.key.keysym.sym) {
						case SDLK_ESCAPE: {
							running = false;
							break;
						}
						case SDLK_F2: {
							if (SDL_GetRelativeMouseMode()) {
								SDL_SetRelativeMouseMode(SDL_FALSE);
								SDL_SetWindowGrab(window, SDL_FALSE);
								relative_mouse = false;
							} else {
								SDL_SetRelativeMouseMode(SDL_TRUE);
								SDL_SetWindowGrab(window, SDL_TRUE);
								relative_mouse = true;
							}
							break;
						}
					}
					break;
				}

				case SDL_QUIT: {
					running = false;
					break;
				}
			}
		}

		camera_input(&camera);

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);





		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, directional_light.shadow_framebuffer_object);

		glClear(GL_DEPTH_BUFFER_BIT);

		shader_bind(&shadow_shader);
		shader_set_mat4(&shadow_shader, "projection", get_directional_light_projection_mat(&directional_light));
		shader_set_mat4(&shadow_shader, "view", get_directional_light_view_mat(&directional_light));

		glCullFace(GL_FRONT);
		model_render(&model, &shadow_shader);
		glCullFace(GL_BACK);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, WIDTH, HEIGHT);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);





//		glBindFramebuffer(GL_FRAMEBUFFER, 0);
//		glViewport(0, 0, WIDTH, HEIGHT);
//		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//
//		float quad_vertices[] = {
//				// positions   // texCoords
//				-1.0f,  1.0f,  0.0f, 1.0f,
//				-1.0f, -1.0f,  0.0f, 0.0f,
//				1.0f, -1.0f,  1.0f, 0.0f,
//
//				-1.0f,  1.0f,  0.0f, 1.0f,
//				1.0f, -1.0f,  1.0f, 0.0f,
//				1.0f,  1.0f,  1.0f, 1.0f
//		};
//
//		GLuint quad_array_buffer;
//		glGenVertexArrays(1, &quad_array_buffer);
//		glBindVertexArray(quad_array_buffer);
//
//		GLuint quad_vertex_buffer;
//		glGenBuffers(1, &quad_vertex_buffer);
//		glBindBuffer(GL_ARRAY_BUFFER, quad_vertex_buffer);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
//
//		glEnableVertexAttribArray(0);
//		glEnableVertexAttribArray(1);
//
//		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
//		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));
//
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, directional_light.shadow_texture_object);
//
//		Shader quad_shader;
//		shader_init(&quad_shader, "shaders/quad.vs", "shaders/quad.fs");
//
//		shader_set_s32(&quad_shader, "quad_texture", 0);
//
//		glDrawArrays(GL_TRIANGLES, 0, 6);


		shader_bind(&shader);

		shader_set_mat4(&shader, "projection", camera_get_projection(&camera));
		shader_set_mat4(&shader, "view", camera_get_view(&camera));
		shader_set_vec3(&shader, "world_camera_pos", camera.pos);

		spot_light.pos = camera.pos;
		spot_light.dir = camera.dir;
		shader_set_vec3(&shader, "spot_light.pos", spot_light.pos);
		shader_set_vec3(&shader, "spot_light.dir", spot_light.dir);

		cubemap_bind(&skybox.cubemap, GL_TEXTURE2);
		shader_set_s32(&shader, "skybox", 2);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, directional_light.shadow_texture_object);
		shader_set_s32(&shader, "shadow_texture", 3);
//
		glEnable(GL_FRAMEBUFFER_SRGB);
		model_render(&model, &shader);
		glDisable(GL_FRAMEBUFFER_SRGB);
//
		shader_bind(&floor_shader);
		shader_set_mat4(&floor_shader, "projection", camera_get_projection(&camera));
		shader_set_mat4(&floor_shader, "view", camera_get_view(&camera));
		shader_set_vec3(&floor_shader, "world_camera_pos", camera.pos);

		shader_set_mat4(&floor_shader, "light_projection", get_directional_light_projection_mat(&directional_light));
		shader_set_mat4(&floor_shader, "light_view", get_directional_light_view_mat(&directional_light));

		spot_light.pos = camera.pos;
		spot_light.dir = camera.dir;
		shader_set_vec3(&floor_shader, "spot_light.pos", spot_light.pos);
		shader_set_vec3(&floor_shader, "spot_light.dir", spot_light.dir);


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, directional_light.shadow_texture_object);
		shader_set_s32(&floor_shader, "shadow_texture", 0);

		glEnable(GL_FRAMEBUFFER_SRGB);
		floor_render(&floor, &floor_shader);
		glDisable(GL_FRAMEBUFFER_SRGB);
//
		shader_bind(&skybox_shader);
		shader_set_mat4(&skybox_shader, "projection", camera_get_projection(&camera));

		// we get rid of the translation of the matrix so the 1x1 box always stays around the camera
//		mat4s skybox_view_matrix = glm::mat4(glm::mat3(camera_get_view(&camera)));
		mat4s skybox_view_matrix = glms_mat4_ins3(glms_mat4_pick3(camera_get_view(&camera)));
		skybox_view_matrix.m03 = 0.0f;
		skybox_view_matrix.m13 = 0.0f;
		skybox_view_matrix.m23 = 0.0f;
		skybox_view_matrix.m30 = 0.0f;
		skybox_view_matrix.m31 = 0.0f;
		skybox_view_matrix.m32 = 0.0f;
		skybox_view_matrix.m33 = 1.0f;

		shader_set_mat4(&skybox_shader, "view", skybox_view_matrix);

		glEnable(GL_FRAMEBUFFER_SRGB);
		skybox_render(&skybox, &skybox_shader);
		glDisable(GL_FRAMEBUFFER_SRGB);


		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(window);
		igNewFrame();

		if (igBegin("Test window", NULL, 0)) {
//			igSliderFloat3("pos", &model.pos.x, -2.0f, 2.0f, "%.3f", 0);
		}
		igEnd();

		igRender();
		ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());


		SDL_GL_SwapWindow(window);

		u64 frame_time_new = SDL_GetPerformanceCounter();
		u64 frame_time = frame_time_new - frame_time_old;
		printf("frame time: %f\n", (float) frame_time / (float) frequency);
		printf("fps: %f\n", 1 / ((float) frame_time / (float) frequency));
	}

	SDL_Quit();
	return 0;
}

void init() {
	SDL_Init(SDL_INIT_VIDEO);

	keyboard_state = SDL_GetKeyboardState(NULL);
	SDL_SetRelativeMouseMode(SDL_FALSE);
	relative_mouse = false;
	frequency = SDL_GetPerformanceFrequency();

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
//	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);

	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	glEnable(GL_MULTISAMPLE);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
	SDL_GL_SetSwapInterval(0);

	window = SDL_CreateWindow("ogldev", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
										  WIDTH, HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, context);

	GLenum res = glewInit();
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: \"%s\"\n", glewGetErrorString(res));
	}

//	stbi_set_flip_vertically_on_load(true);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	// debug stuff
	int flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(glDebugOutput, NULL);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
	}

	igCreateContext(NULL);
	ImGuiIO *io = igGetIO();
	igStyleColorsDark(NULL);

	ImGui_ImplSDL2_InitForOpenGL(window, context);
	ImGui_ImplOpenGL3_Init("#version 460");
}
