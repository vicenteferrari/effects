#version 460

in vec2 tex0;

out vec4 out_color;

uniform sampler2D diffuse;

void main() {
	out_color = texture2D(diffuse, tex0);
}
