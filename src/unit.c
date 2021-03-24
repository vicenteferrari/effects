//
// Created by vfs on 01/01/2021.
//

typedef enum UnitType {
	UNIT_NONE,
	UNIT_PAWN,
	UNIT_KNIGHT,
	UNIT_BISHOP,
	UNIT_ROOK,
	UNIT_QUEEN,
	UNIT_KING,

	UNIT_ARCHER
} UnitType;

typedef struct Unit {
	UnitType type;

	Model *model;

	u32 x;
	u32 y;
} Unit;
