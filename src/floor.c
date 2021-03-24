//
// Created by vfs on 22/12/2020.
//

typedef struct Floor {
	vec3s pos;
	vec3s scale;

	GLuint vertex_array_object;
	GLuint vertex_buffer_object;
	u32 index_count;
	GLuint index_buffer_object;

	Texture texture;
} Floor;

void floor_init(Floor *floor) {
	floor->pos = (vec3s) {0.0f, 0.0f, 0.0f};
	floor->scale = (vec3s) {100.0f, 1.0f, 100.0f};

	const struct aiScene *aiScene = aiImportFile("models/floor/floor.obj",
												 aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);
	if (aiScene) {
		struct aiMesh *aiMesh = aiScene->mMeshes[0];
		u32 vertex_count = aiMesh->mNumVertices;
		Vertex *vertices = (Vertex *) calloc(vertex_count, sizeof(Vertex));

		for (u32 i = 0; i < vertex_count; ++i) {
			vertices[i].pos.x = aiMesh->mVertices[i].x;
			vertices[i].pos.y = aiMesh->mVertices[i].y;
			vertices[i].pos.z = aiMesh->mVertices[i].z;

			vertices[i].norm.x = aiMesh->mNormals[i].x;
			vertices[i].norm.y = aiMesh->mNormals[i].y;
			vertices[i].norm.z = aiMesh->mNormals[i].z;

			vertices[i].tex.x = aiMesh->mTextureCoords[0][i].x * 50.0f;
			vertices[i].tex.y = aiMesh->mTextureCoords[0][i].y * 50.0f;
		}

		floor->index_count = aiMesh->mNumFaces * 3;
		u32 *indices = (u32 *) calloc(floor->index_count, sizeof(u32));
		for (u32 i = 0; i < aiMesh->mNumFaces; ++i) {
			struct aiFace aiFace = aiMesh->mFaces[i];
			for (u32 j = 0; j < aiFace.mNumIndices; ++j) {
				indices[(i * aiFace.mNumIndices) + j] = aiFace.mIndices[j];
			}
		}

		texture_init(&floor->texture, GL_TEXTURE_2D, "models/floor", "diffuse.png");

		glGenVertexArrays(1, &floor->vertex_array_object);
		glBindVertexArray(floor->vertex_array_object);

		glGenBuffers(1, &floor->vertex_buffer_object);
		glBindBuffer(GL_ARRAY_BUFFER, floor->vertex_buffer_object);
		glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(Vertex), vertices, GL_STATIC_DRAW);

		glGenBuffers(1, &floor->index_buffer_object);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floor->index_buffer_object);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, floor->index_count * sizeof(u32), indices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, pos));
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, norm));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, tex));
	}
}

void floor_render(Floor *floor, Shader *shader) {
	mat4s transform = glms_mat4_identity();
	transform = glms_translate(transform, floor->pos);
	transform = glms_rotate(transform, 0.0f, (vec3s) {0.0f, 1.0f, 0.0f});
	transform = glms_scale(transform, floor->scale);

	shader_set_mat4(shader, "world", transform);

	texture_bind(&floor->texture, GL_TEXTURE1);
	shader_set_s32(shader, "material.diffuse", 1);

	shader_set_float(shader, "material.shininess", 32.f);

	// draw
	glBindVertexArray(floor->vertex_array_object);
	glDrawElements(GL_TRIANGLES, floor->index_count, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
