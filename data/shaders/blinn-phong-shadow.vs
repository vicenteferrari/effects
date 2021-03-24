#version 460

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_norm;
layout (location = 2) in vec2 in_tex;

out vec3 world_pos0;
out vec3 norm0;
out vec2 tex0;
out vec4 light_pos0;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 world;

uniform mat4 light_projection;
uniform mat4 light_view;

void main() {
	gl_Position = projection * view * world * vec4(in_pos, 1.0);
	world_pos0 = vec3(world * vec4(in_pos, 1.0));
	norm0 = mat3(transpose(inverse(world))) * in_norm;
	tex0 = in_tex;
	light_pos0 = light_projection * light_view * world * vec4(in_pos, 1.0);
}
