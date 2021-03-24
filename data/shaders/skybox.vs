#version 460

layout (location = 0) in vec3 in_pos;

out vec3 tex0;

uniform mat4 projection;
uniform mat4 view;

void main() {
	vec4 pos = projection * view * vec4(in_pos, 1.0);
	gl_Position = pos.xyww;
	tex0 = in_pos;
}
