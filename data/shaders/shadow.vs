#version 460

layout (location = 0) in vec3 in_pos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 world;

void main() {
	gl_Position = projection * view * world * vec4(in_pos, 1.0);
}
