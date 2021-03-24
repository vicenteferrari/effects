//
// Created by vfs on 01/01/2021.
//

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (2 * sizeof(void *))
#endif

typedef struct Arena {
	uint8_t *buffer;
	size_t buffer_size;
	size_t previous_offset;
	size_t current_offset;
} Arena;

uintptr_t align_forward(uintptr_t ptr, size_t align);
bool is_power_of_two(uintptr_t x);

void arena_init(Arena *arena, uint8_t *buffer, size_t buffer_size) {
	arena->buffer = buffer;
	arena->buffer_size = buffer_size;
	arena->previous_offset = 0;
	arena->current_offset = 0;
}

void *arena_alloc_align(Arena *arena, size_t size, size_t align) {
	uintptr_t curr_ptr = (uintptr_t) arena->buffer + (uintptr_t) arena->current_offset;
	uintptr_t offset = align_forward(curr_ptr, align);
	offset -= (uintptr_t) arena->buffer;

	if (offset + size <= arena->buffer_size) {
		void *ptr = arena->buffer + offset;
		arena->previous_offset = offset;
		arena->current_offset = offset + size;

		memset(ptr, 0, size);
		return ptr;
	}
}

void *arena_alloc(Arena *arena, size_t size) {
	return arena_alloc_align(arena, size, DEFAULT_ALIGNMENT);
}

void *arena_resize_align(Arena *arena, void *old_memory, size_t old_size, size_t new_size, size_t align) {
	unsigned char *old_mem = (unsigned char *)old_memory;

	assert(is_power_of_two(align));

	if (old_mem == NULL || old_size == 0) {
		return arena_alloc_align(arena, new_size, align);
	} else if (arena->buffer <= old_mem && old_mem < arena->buffer + arena->buffer_size) {
		if (arena->buffer + arena->previous_offset == old_mem) {
			arena->current_offset = arena->previous_offset + new_size;
			if (new_size > old_size) {
				// Zero the new memory by default
				memset(&arena->buffer[arena->current_offset], 0, new_size - old_size);
			}
			return old_memory;
		} else {
			void *new_memory = arena_alloc_align(arena, new_size, align);
			size_t copy_size = old_size < new_size ? old_size : new_size;
			// Copy across old memory to the new memory
			memmove(new_memory, old_memory, copy_size);
			return new_memory;
		}

	} else {
		assert(0 && "Memory is out of bounds of the buffer in this arena");
		return NULL;
	}

}

void arena_free_all(Arena *arena) {
	arena->current_offset = 0;
	arena->previous_offset = 0;
}

// Because C doesn't have default parameters
void *arena_resize(Arena *a, void *old_memory, size_t old_size, size_t new_size) {
	return arena_resize_align(a, old_memory, old_size, new_size, DEFAULT_ALIGNMENT);
}

uintptr_t align_forward(uintptr_t ptr, size_t align) {
	uintptr_t p, a, modulo;

	assert(is_power_of_two(align));

	p = ptr;
	a = (uintptr_t)align;
	// Same as (p % a) but faster as 'a' is a power of two
	modulo = p & (a-1);

	if (modulo != 0) {
		// If 'p' address is not aligned, push the address to the
		// next value which is aligned
		p += a - modulo;
	}
	return p;
}

bool is_power_of_two(uintptr_t x) {
	return (x & (x-1)) == 0;
}
