#version 460

in vec2 tex0;

uniform sampler2D quad_texture;

out vec4 out_color;

void main() {
	float depth = texture(quad_texture, tex0).x;
	out_color = vec4(vec3(depth), 1.0);
}
