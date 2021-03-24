#version 460

layout (location = 0) in vec3 in_pos;
layout (location = 2) in vec2 in_tex;

out vec2 tex0;

uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;

void main() {
	gl_Position = projection * view * world * vec4(in_pos, 1.0);
	tex0 = in_tex;
}
