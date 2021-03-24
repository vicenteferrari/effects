#version 460

layout (location = 0) in vec2 in_pos;
layout (location = 1) in vec2 in_tex;

out vec2 tex0;

void main() {
	gl_Position = vec4(in_pos, 0.0, 1.0);
	tex0 = in_tex;
}
