//
// Created by vfs on 12/1/20.
//

#ifndef OGLDEV_COMMON_H
#define OGLDEV_COMMON_H

#define ARRAYCOUNT(array) sizeof(array) / sizeof(array[0])

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

const u32 WIDTH = 1280, HEIGHT = 720;

const char *read_entire_file(const char *filename, u32 *size) {
	FILE *file = fopen(filename, "rb");

	if (!file)
		return NULL;

	fseek(file, 0, SEEK_END);
	u32 file_size = ftell(file);
	*size = file_size;
	rewind(file);

	char *buffer = (char *) malloc(file_size + 1);
	fread(buffer, 1, file_size, file);
	fclose(file);

	buffer[file_size] = '\0';

	return buffer;
}

#endif //OGLDEV_COMMON_H
