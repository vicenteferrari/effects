//
// Created by vfs on 12/4/20.
//

extern SDL_Window *window;

typedef struct Shader {
	char vertex_text_path[256];
	char fragment_text_path[256];

	GLuint program_object_id;
} Shader;

void shader_init(Shader *shader, const char *vertex_text_path, const char *fragment_text_path) {
	strcpy(shader->vertex_text_path, vertex_text_path);
	strcpy(shader->fragment_text_path, fragment_text_path);

	s32 vertex_text_size;
	const char *vertex_text = read_entire_file(vertex_text_path, (u32 *) &vertex_text_size);

	s32 fragment_text_size;
	const char *fragment_text = read_entire_file(fragment_text_path, (u32 *) &fragment_text_size);

	GLuint vertex_object_id;

	vertex_object_id = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_object_id, 1, &vertex_text, NULL);
	glCompileShader(vertex_object_id);

	GLint success;
	glGetShaderiv(vertex_object_id, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar info_log[1024];
		glGetShaderInfoLog(vertex_object_id, sizeof(info_log), NULL, info_log);
		fprintf(stderr, "Error compiling VERTEX shader at %s: \"%s\"\n", vertex_text_path, info_log);

		char buffer[1024];
		sprintf(buffer, "Error compiling VERTEX shader at %s: \"%s\"\n", vertex_text_path, info_log);
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Shader Compilation ERROR", buffer, window);
	}


	GLuint fragment_object_id;
	fragment_object_id = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_object_id, 1, &fragment_text, NULL);
	glCompileShader(fragment_object_id);

	glGetShaderiv(fragment_object_id, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar info_log[1024];
		glGetShaderInfoLog(fragment_object_id, sizeof(info_log), NULL, info_log);
		fprintf(stderr, "Error compiling FRAGMENT shader at %s: \"%s\"\n", fragment_text_path, info_log);

		char buffer[1024];
		sprintf(buffer, "Error compiling FRAGMENT shader at %s: \"%s\"\n", fragment_text_path, info_log);
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Shader Compilation ERROR", buffer, window);
	}


	shader->program_object_id = glCreateProgram();
	glAttachShader(shader->program_object_id, vertex_object_id);
	glAttachShader(shader->program_object_id, fragment_object_id);
	glLinkProgram(shader->program_object_id);

	glGetProgramiv(shader->program_object_id, GL_LINK_STATUS, &success);
	if (!success) {
		GLchar info_log[1024];
		glGetProgramInfoLog(shader->program_object_id, sizeof(info_log), NULL, info_log);
		printf("Error linking program: %s\n", info_log);
	}

	glValidateProgram(shader->program_object_id);

	glDeleteShader(vertex_object_id);
	glDeleteShader(fragment_object_id);
}

void shader_bind(Shader *shader) {
	glUseProgram(shader->program_object_id);
}

void shader_set_bool(Shader *shader, const char *name, bool value) {
	glUseProgram(shader->program_object_id);
	glUniform1i(glGetUniformLocation(shader->program_object_id, name), (s32) value);
}

void shader_set_s32(Shader *shader, const char *name, s32 value) {
	glUseProgram(shader->program_object_id);
	glUniform1i(glGetUniformLocation(shader->program_object_id, name), value);
}

void shader_set_float(Shader *shader, const char *name, float value) {
	glUseProgram(shader->program_object_id);
	glUniform1f(glGetUniformLocation(shader->program_object_id, name), value);
}

void shader_set_vec3(Shader *shader, const char *name, vec3s vector) {
	glUseProgram(shader->program_object_id);
	glUniform3f(glGetUniformLocation(shader->program_object_id, name), vector.x, vector.y, vector.z);
}

void shader_set_vec4(Shader *shader, const char *name, vec4s vector) {
	glUseProgram(shader->program_object_id);
	glUniform4f(glGetUniformLocation(shader->program_object_id, name), vector.x, vector.y, vector.z, vector.w);
}

void shader_set_mat4(Shader *shader, const char *name, mat4s matrix) {
	glUseProgram(shader->program_object_id);
	glUniformMatrix4fv(glGetUniformLocation(shader->program_object_id, name), 1, GL_FALSE, &matrix.raw[0][0]);
}
