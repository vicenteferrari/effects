//
// Created by vfs on 01/01/2021.
//

typedef enum TileType {
	TILE_NONE,
	TILE_TEST,
	TILE_GRASS
} TileType;

typedef struct Tile {
	TileType type;

	Model *plane;
	Unit *unit_on_top;
} Tile;

void tile_init(Tile *tile) {
//	map->map_data[r + c * channels].type = TILE_TEST;
}
