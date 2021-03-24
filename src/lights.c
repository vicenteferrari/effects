//
// Created by vfs on 12/5/20.
//

#define MAX_POINT_LIGHTS 1
const u32 SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

typedef struct DirectionalLight {
	vec3s color;
	vec3s dir;

	float ambient_intensity;
	float diffuse_intensity;
	float specular_intensity;

	GLuint shadow_framebuffer_object;
	GLuint shadow_texture_object;
} DirectionalLight;

typedef struct PointLight {
	vec3s pos;
	vec3s color;

	float ambient_intensity;
	float diffuse_intensity;
	float specular_intensity;

	float constant;
	float linear;
	float quadratic;
} PointLight;

typedef struct SpotLight {
	vec3s pos;
	vec3s color;
	vec3s dir;

	float cut_off;
	float outer_cut_off;

	float ambient_intensity;
	float diffuse_intensity;
	float specular_intensity;

	float constant;
	float linear;
	float quadratic;
} SpotLight;

void directional_light_init(DirectionalLight *light) {
	light->color = (vec3s) {1.0f, 1.0f, 1.0f};
	light->dir = glms_normalize((vec3s) {0.6f, -1.0f, 1.0f});
	light->ambient_intensity = 0.3f;
	light->diffuse_intensity = 1.0f;
	light->specular_intensity = 0.7f;

	glGenFramebuffers(1, &light->shadow_framebuffer_object);

	glGenTextures(1, &light->shadow_texture_object);
	glBindTexture(GL_TEXTURE_2D, light->shadow_texture_object);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, light->shadow_framebuffer_object);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, light->shadow_texture_object, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void set_directional_light(DirectionalLight *light, Shader *shader) {
	shader_set_vec3(shader, "directional_light.color", light->color);
	shader_set_vec3(shader, "directional_light.dir", light->dir);
	shader_set_float(shader, "directional_light.ambient_intensity", light->ambient_intensity);
	shader_set_float(shader, "directional_light.diffuse_intensity", light->diffuse_intensity);
	shader_set_float(shader, "directional_light.specular_intensity", light->specular_intensity);
}

mat4s get_directional_light_view_mat(DirectionalLight *light) {
	return glms_lookat((vec3s) {-2.0f, 4.0f, -2.0f}, (vec3s) {0.0f, 0.0f, 0.0f}, (vec3s) {0.0f, 1.0f, 0.0f});
}

mat4s get_directional_light_projection_mat(DirectionalLight *light) {
	float near_plane = 1.f, far_plane = 10.f;
	return glms_ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
}

void set_point_light(PointLight *point_light, Shader *shader) {
	shader_set_vec3(shader, "point_lights[0].pos", point_light->pos);
	shader_set_vec3(shader, "point_lights[0].color", point_light->color);
	shader_set_float(shader, "point_lights[0].ambient_intensity", point_light->ambient_intensity);
	shader_set_float(shader, "point_lights[0].diffuse_intensity", point_light->diffuse_intensity);
	shader_set_float(shader, "point_lights[0].specular_intensity", point_light->specular_intensity);
	shader_set_float(shader, "point_lights[0].constant", point_light->constant);
	shader_set_float(shader, "point_lights[0].linear", point_light->linear);
	shader_set_float(shader, "point_lights[0].quadratic", point_light->quadratic);
}

void set_spot_light(SpotLight *spot_light, Shader *shader) {
	shader_set_vec3(shader, "spot_light.pos", spot_light->pos);
	shader_set_vec3(shader, "spot_light.color", spot_light->color);
	shader_set_vec3(shader, "spot_light.dir", spot_light->dir);
	shader_set_float(shader, "spot_light.cut_off", spot_light->cut_off);
	shader_set_float(shader, "spot_light.outer_cut_off", spot_light->outer_cut_off);
	shader_set_float(shader, "spot_light.ambient_intensity", spot_light->ambient_intensity);
	shader_set_float(shader, "spot_light.diffuse_intensity", spot_light->diffuse_intensity);
	shader_set_float(shader, "spot_light.specular_intensity", spot_light->specular_intensity);
	shader_set_float(shader, "spot_light.constant", spot_light->constant);
	shader_set_float(shader, "spot_light.linear", spot_light->linear);
	shader_set_float(shader, "spot_light.quadratic", spot_light->quadratic);
}
