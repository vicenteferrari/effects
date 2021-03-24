#version 460

out vec4 out_color;

in vec3 tex0;

uniform samplerCube skybox;

void main() {
	out_color = texture(skybox, tex0);
}
