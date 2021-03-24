//
// Created by vfs on 12/1/20.
//

extern const u8 *keyboard_state;
extern s32 mouse_x, mouse_y;
extern float dt;
extern bool relative_mouse;

typedef struct Camera {
	vec3s pos;
	float yaw;
	float pitch;

	vec3s dir;
	vec3s up;

	float fov;
	float ar;
	float z_near;
	float z_far;

	mat4s view;
	mat4s projection;
} Camera;

void camera_init(Camera *camera) {
//	camera->scale.x = 0.0f;
//	camera->scale.y = 0.0f;
//	camera->scale.z = 0.0f;

	camera->pos.x = 0.0f;
	camera->pos.y = 4.0f;
	camera->pos.z = 5.0f;

	camera->yaw = -90.0f;
	camera->pitch = 0.0f;

	camera->dir.x = 0.0f;
	camera->dir.y = 0.0f;
	camera->dir.z = -1.0f;

	camera->up.x = 0.0f;
	camera->up.y = 1.0f;
	camera->up.z = 0.0f;

//	camera->rotation.x = 0.0f;
//	camera->rotation.y = 0.0f;
//	camera->rotation.z = 0.0f;

	camera->fov = 90.0f;
	camera->ar = (float) WIDTH / (float) HEIGHT;
	camera->z_near = 0.1f;
	camera->z_far = 100.0f;
}

//void camera_scale(Camera *camera, float scale_x, float scale_y, float scale_z) {
//	camera->scale.x = scale_x;
//	camera->scale.y = scale_y;
//	camera->scale.z = scale_z;
//}

//void camera_pos(Camera *camera, float pos_x, float pos_y, float pos_z) {
//	camera->pos.x = pos_x;
//	camera->pos.y = pos_y;
//	camera->pos.z = pos_z;
//}

//void camera_rot(Camera *camera, float rot_x, float rot_y, float rot_z) {
//	camera->rotation.x = rot_x;
//	camera->rotation.y = rot_y;
//	camera->rotation.z = rot_z;
//}

mat4s camera_get_view(Camera *camera) {
	camera->view = glms_lookat(camera->pos, glms_vec3_add(camera->pos, camera->dir), camera->up);

	return camera->view;
}

mat4s camera_get_projection(Camera *camera) {
	camera->projection = glms_perspective(glm_rad(camera->fov), camera->ar, camera->z_near, camera->z_far);

	return camera->projection;
}

void camera_input(Camera *camera) {
	float movement_speed = 0.05f;

	if (keyboard_state[SDL_SCANCODE_W]) {
		camera->pos = glms_vec3_add(camera->pos, glms_vec3_scale(glms_normalize(camera->dir), movement_speed));
	}
	if (keyboard_state[SDL_SCANCODE_S]) {
		camera->pos = glms_vec3_sub(camera->pos, glms_vec3_scale(glms_normalize(camera->dir), movement_speed));
	}
	if (keyboard_state[SDL_SCANCODE_A]) {
		camera->pos = glms_vec3_sub(camera->pos, glms_vec3_scale(glms_normalize(glms_cross(camera->dir, camera->up)), movement_speed));
	}
	if (keyboard_state[SDL_SCANCODE_D]) {
		camera->pos = glms_vec3_add(camera->pos, glms_vec3_scale(glms_normalize(glms_cross(camera->dir, camera->up)), movement_speed));
	}

	SDL_GetRelativeMouseState(&mouse_x, &mouse_y);
	float sensitivity = 0.1f;

	if (relative_mouse) {
		camera->yaw += mouse_x * sensitivity;
		camera->pitch += -mouse_y * sensitivity;
	}

	if (camera->pitch > 89.0f)
		camera->pitch = 89.0f;
	if (camera->pitch < -89.0f)
		camera->pitch = -89.0f;

	camera->dir.x = cosf(glm_rad(camera->yaw)) * cosf(glm_rad(camera->pitch));
	camera->dir.y = sinf(glm_rad(camera->pitch));
	camera->dir.z = sinf(glm_rad(camera->yaw)) * cosf(glm_rad(camera->pitch));


}
