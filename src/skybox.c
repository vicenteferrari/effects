//
// Created by vfs on 19/12/2020.
//

typedef struct Cubemap {
	char filename[6][256];

	u8 *data[6];
	s32 width;
	s32 height;
	s32 channels;
	GLenum format;

	GLuint cubemap_object;
} Cubemap;

typedef struct Skybox {
	GLuint vertex_array_object;
	GLuint vertex_buffer_object;

	Cubemap cubemap;
} Skybox;

void cubemap_init(Cubemap *cubemap, const char *directory, const char filename[6][256]) {
	cubemap->width = 0;
	cubemap->height = 0;
	cubemap->channels = 0;

	glGenTextures(1, &cubemap->cubemap_object);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->cubemap_object);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	for (u32 i = 0; i < 6; ++i) {
		char path[256];
		sprintf(path, "%s/%s", directory, filename[i]);
		strcpy(cubemap->filename[i], filename[i]);

		printf("loading texture from file %s\n", path);

		s32 new_width = 0;
		s32 new_height = 0;
		s32 new_channels = 0;
		cubemap->data[i] = stbi_load(path, &new_width, &new_height, &new_channels, 0);

		if (i > 0) {
			assert(new_width == cubemap->width && new_height == cubemap->height && new_channels == cubemap->channels);
		}

		cubemap->width = new_width;
		cubemap->height = new_height;
		cubemap->channels = new_channels;

		if (cubemap->data[i]) {
			if (cubemap->channels == 1)
				cubemap->format = GL_RED;
			else if (cubemap->channels == 3)
				cubemap->format = GL_RGB;
			else if (cubemap->channels == 4)
				cubemap->format = GL_RGBA;


			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, cubemap->width, cubemap->height,
						 0, cubemap->format, GL_UNSIGNED_BYTE, cubemap->data[i]);

//		stbi_image_free(data);
		}
		else {
			char buffer[256];
			sprintf(buffer, "FAILED to load texture at path: %s\n", path);
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ERROR", buffer, window);
			stbi_image_free(cubemap->data);
		}
	}
}

void cubemap_bind(Cubemap *cubemap, GLenum texture_unit) {
	glActiveTexture(texture_unit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->cubemap_object);
}

void skybox_init(Skybox *skybox) {
	char filenames[6][256] = {
			"right.jpg",
			"left.jpg",
			"top.jpg",
			"bottom.jpg",
			"front.jpg",
			"back.jpg"
	};

	cubemap_init(&skybox->cubemap, "models/skybox", filenames);

	float skybox_vertices[] = {
			// positions
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f,  1.0f
	};

	glGenVertexArrays(1, &skybox->vertex_array_object);
	glBindVertexArray(skybox->vertex_array_object);

	glGenBuffers(1, &skybox->vertex_buffer_object);
	glBindBuffer(GL_ARRAY_BUFFER, skybox->vertex_buffer_object);
	glBufferData(GL_ARRAY_BUFFER,  sizeof(skybox_vertices), skybox_vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
}

void skybox_render(Skybox *skybox, Shader *shader) {
	// bind textures
	shader_set_s32(shader, "skybox", 0);
	cubemap_bind(&skybox->cubemap, GL_TEXTURE0);


	// draw
	glDepthMask(GL_FALSE);
	glBindVertexArray(skybox->vertex_array_object);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthMask(GL_TRUE);
}
