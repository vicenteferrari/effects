#version 460

layout (location = 0) in vec3 in_pos;

uniform mat4 world;

void main() {
	gl_Position = world * vec4(in_pos, 1.0);
}
