//
// Created by vfs on 01/01/2021.
//

typedef struct Map {
	u32 x;
	u32 y;

	Tile* map_data;
	u32 map_size;
} Map;

void map_init(Map *map, const char *directory, const char* filename) {
	char path[256];
	sprintf(path, "%s/%s", directory, filename);

	s32 width, height, channels;

	u8 *img_data = stbi_load(path, &width, &height, &channels, 0);

	map->map_data = malloc(width * height * sizeof(Tile));

	for (u32 r = 0; r < height; ++r) {
		for (u32 c = 0; c < width; ++c) {
			if (img_data[r * channels * width + c * channels + 0] == 0 && img_data[r * channels * width + c * channels + 1] == 0 && img_data[r * channels * width + c * channels + 2] == 0) {

			}
		}
	}
}
