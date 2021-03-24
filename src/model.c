//
// Created by vfs on 12/5/20.
//

extern SDL_Window *window;
extern u64 allocated;

typedef struct Vertex {
	vec3s pos;
	vec3s norm;
	vec2s tex;
} Vertex;

typedef struct Texture {
	char filename[256];

	u8 *data;
	s32 width;
	s32 height;
	s32 channels;
	GLenum format;
	GLenum texture_target;

	GLuint texture_object;
} Texture;

typedef struct Material {
	Texture *diffuse;
	Texture *specular;
	float shininess;
} Material;

typedef struct Mesh {
	u32 vertex_count;
	Vertex *vertices;
	u32 index_count;
	u32 *indices;

	Material material;

	GLuint vertex_array_object;
	GLuint vertex_buffer_object;
	GLuint index_buffer_object;
} Mesh;

#define MAX_TEXTURES 8

typedef struct Model {
	vec3s pos;

	u32 mesh_count;
	Mesh *meshes;
	u32 texture_count;
	Texture textures[8];

	char directory[256];
	char filename[256];
} Model;

void texture_init(Texture *texture, GLenum texture_target, const char *directory, const char *filename);
void texture_bind(Texture *texture, GLenum texture_unit);
void mesh_init(Mesh *mesh, Model *model, struct aiMesh *aiMesh, const struct aiScene *aiScene);
void mesh_render(Mesh *mesh, Shader *shader);
void model_init(Model *model, const char *directory, const char* filename);
void model_render(Model *model, Shader *shader);
void process_aiNode(Model *model, struct aiNode *aiNode, const struct aiScene *aiScene);
void process_aiMesh(Model *model, Mesh *mesh, struct aiMesh *aiMesh, const struct aiScene *aiScene);
void print_materials(const struct aiScene *aiScene);

void texture_init(Texture *texture, GLenum texture_target, const char *directory, const char *filename) {
	char path[256];
	sprintf(path, "%s/%s", directory, filename);
	strcpy(texture->filename, filename);

	printf("loading texture from file %s\n", path);
	texture->texture_target = texture_target;
	texture->data = stbi_load(path, &texture->width, &texture->height, &texture->channels, 0);


	if (texture->data) {
		if (texture->channels == 1)
			texture->format = GL_RED;
		else if (texture->channels == 3)
			texture->format = GL_RGB;
		else if (texture->channels == 4)
			texture->format = GL_RGBA;

		glGenTextures(1, &texture->texture_object);
		glBindTexture(GL_TEXTURE_2D, texture->texture_object);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, texture->width, texture->height, 0, texture->format, GL_UNSIGNED_BYTE, texture->data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

//		stbi_image_free(data);
	}
	else {
		char buffer[256];
		sprintf(buffer, "FAILED to load texture at path: %s\n", path);
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ERROR", buffer, window);
		stbi_image_free(texture->data);
	}
}

void texture_bind(Texture *texture, GLenum texture_unit) {
	glActiveTexture(texture_unit);
	glBindTexture(texture->texture_target, texture->texture_object);
}

void mesh_init(Mesh *mesh, Model *model, struct aiMesh *aiMesh, const struct aiScene *aiScene) {
//	mesh->material_index = aiMesh->mMaterialIndex;

	mesh->vertex_count = aiMesh->mNumVertices;
	mesh->vertices = (Vertex *) malloc(mesh->vertex_count * sizeof(Vertex));
	for (u32 i = 0; i < mesh->vertex_count; ++i) {
		mesh->vertices[i].pos.x = aiMesh->mVertices[i].x;
		mesh->vertices[i].pos.y = aiMesh->mVertices[i].y;
		mesh->vertices[i].pos.z = aiMesh->mVertices[i].z;

		mesh->vertices[i].norm.x = aiMesh->mNormals[i].x;
		mesh->vertices[i].norm.y = aiMesh->mNormals[i].y;
		mesh->vertices[i].norm.z = aiMesh->mNormals[i].z;

		mesh->vertices[i].tex.x = aiMesh->mTextureCoords[0][i].x;
		mesh->vertices[i].tex.y = aiMesh->mTextureCoords[0][i].y;
	}


	mesh->index_count = aiMesh->mNumFaces * 3;
	mesh->indices = (u32 *) malloc(mesh->index_count * sizeof(u32));
	for (u32 i = 0; i < aiMesh->mNumFaces; ++i) {
		struct aiFace aiFace = aiMesh->mFaces[i];
		for (u32 j = 0; j < aiFace.mNumIndices; ++j) {
			mesh->indices[(i * aiFace.mNumIndices) + j] = aiFace.mIndices[j];
		}
	}

	struct aiMaterial *aiMaterial = aiScene->mMaterials[aiMesh->mMaterialIndex];

	for (u32 i = 0; i < aiGetMaterialTextureCount(aiMaterial, aiTextureType_DIFFUSE); ++i) {
		struct aiString path;
		aiGetMaterialTexture(aiMaterial, aiTextureType_DIFFUSE, i, &path, NULL, NULL, NULL, NULL, NULL, NULL);

		for (u32 j = 0; j < model->texture_count; ++j) {
			if (strcmp(model->textures[j].filename, path.data) == 0) {
				mesh->material.diffuse = &model->textures[j];
				goto _end_diffuse;
			}
		}

		texture_init(&model->textures[model->texture_count++], GL_TEXTURE_2D, model->directory, path.data);
		mesh->material.diffuse = &model->textures[model->texture_count - 1];
		_end_diffuse:;
	}

	for (u32 i = 0; i < aiGetMaterialTextureCount(aiMaterial, aiTextureType_SPECULAR); ++i) {
		struct aiString path;
		aiGetMaterialTexture(aiMaterial, aiTextureType_SPECULAR, i, &path, NULL, NULL, NULL, NULL, NULL, NULL);

		for (u32 j = 0; j < model->texture_count; ++j) {
			if (strcmp(model->textures[j].filename, path.data) == 0) {
				mesh->material.specular = &model->textures[j];
				goto _end_specular;
			}
		}

		texture_init(&model->textures[model->texture_count++], GL_TEXTURE_2D, model->directory, path.data);
		mesh->material.diffuse = &model->textures[model->texture_count - 1];
		_end_specular:;
	}

	mesh->material.shininess = 32.0f;

	glGenVertexArrays(1, &mesh->vertex_array_object);
	glBindVertexArray(mesh->vertex_array_object);

	glGenBuffers(1, &mesh->vertex_buffer_object);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertex_buffer_object);
	glBufferData(GL_ARRAY_BUFFER, mesh->vertex_count * sizeof(Vertex), mesh->vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &mesh->index_buffer_object);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->index_buffer_object);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->index_count * sizeof(u32), mesh->indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, pos));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, norm));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, tex));
}

void mesh_render(Mesh *mesh, Shader *shader) {

	// bind textures
	if (mesh->material.diffuse != NULL) {
		shader_set_s32(shader, "material.diffuse", 0);
		texture_bind(mesh->material.diffuse, GL_TEXTURE0);
	}

	if (mesh->material.specular != NULL) {
		shader_set_s32(shader, "material.specular", 1);
		texture_bind(mesh->material.specular, GL_TEXTURE1);
	}

	shader_set_float(shader, "material.shininess", mesh->material.shininess);

	// draw
	glBindVertexArray(mesh->vertex_array_object);
	glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void model_init(Model *model, const char *directory, const char* filename) {
	model->pos = (vec3s) {2.0f, 2.0f, 0.0f};

	strcpy(model->directory, directory);
	strcpy(model->filename, filename);

	char path[256];
	sprintf(path, "%s/%s", directory, filename);
	printf("loading model %s\n", path);

	const struct aiScene *aiScene = aiImportFile(path,
									 aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);

	if (aiScene) {
		printf("processing model %s\n", path);

		model->mesh_count = 0;
		model->meshes = (Mesh *) calloc(aiScene->mNumMeshes, sizeof(Mesh));

		model->texture_count = 0;

//		print_materials(aiScene);
		process_aiNode(model, aiScene->mRootNode, aiScene);
	}

	aiReleaseImport(aiScene);
}

void model_render(Model *model, Shader *shader) {
	mat4s trans = glms_translate(glms_mat4_identity(), model->pos);
	shader_set_mat4(shader, "world", trans);

	for (u32 i = 0; i < model->mesh_count; ++i) {
		mesh_render(&model->meshes[i], shader);
	}
}

void process_aiNode(Model *model, struct aiNode *aiNode, const struct aiScene *aiScene) {
	printf("processing node %s\n", aiNode->mName.data);
	for (u32 i = 0; i < aiNode->mNumMeshes; ++i) {
		struct aiMesh *aiMesh = aiScene->mMeshes[aiNode->mMeshes[i]];
		process_aiMesh(model, &model->meshes[model->mesh_count++], aiMesh, aiScene);
	}

	for (u32 i = 0; i < aiNode->mNumChildren; ++i) {
		process_aiNode(model, aiNode->mChildren[i], aiScene);
	}
}

void process_aiMesh(Model *model, Mesh *mesh, struct aiMesh *aiMesh, const struct aiScene *aiScene) {
	printf("processing mesh %s\n", aiMesh->mName.data);
	mesh_init(mesh, model, aiMesh, aiScene);
}

//void print_materials(const struct aiScene *aiScene) {
//	for (u32 i = 0; i < aiScene->mNumMaterials; ++i) {
//		for (u32 j = 0; j < aiGetMaterialTextureCount(aiScene->mMaterials[i], aiTextureType_DIFFUSE); ++j) {
//			struct aiString path;
//			enum aiTextureMapping mapping;
//			u32 uv;
//			ai_real blend;
//			enum aiTextureOp texop;
//			enum aiTextureMapMode mapmode;
//			u32 flags;
//
//			aiGetMaterialTexture(aiScene->mMaterials[i], aiTextureType_DIFFUSE, j, &path, &mapping, &uv, &blend, &texop, &mapmode, &flags);
//			printf("path: %s\n", path.data);
//			printf("aiTextureMapping: %d\n", mapping);
//			printf("uv: %d\n", uv);
//			printf("aiTextureOp: %d\n", texop);
//			printf("aiTextureMapMode: %d\n", mapmode);
//
//		}
//	}
//}
