#include "../include/osm/utils.h"
#include <stdlib.h>
#include <string.h>

#define VECT_MAX_GROW 1024
#define VECT_INIT_CAP 4

/**
 * Initialize a new vector
 */
Vector vect_init(unsigned int elsz)
{
	Vector out = {
		.elsz = elsz,
		.size = VECT_INIT_CAP,
		.count = 0
	};

	out.data = malloc(out.elsz * out.size);

	return out;
}

/**
 * Unexported function for auto-growing the vector
 */
void _vect_grow(Vector *vec)
{
	if (vec->elsz == 0)
	{
		return;
	}

	if (vec->size == 0)
	{
		vec->size = VECT_INIT_CAP;
		vec->data = realloc(vec->data, vec->size * vec->elsz);
		return;
	}

	if (vec->size > VECT_MAX_GROW)
	{
		vec->size += VECT_MAX_GROW;
	}
	else
	{
		vec->size *= 2;
	}

	vec->data = realloc(vec->data, vec->size * vec->elsz);
}

/**
 * Unexported function for auto-shrinking the vector
 */
void _vect_shrink(Vector *vec)
{
	if (vec->size / 2 < VECT_INIT_CAP)
	{
		return;
	}

	vec->size /= 2;
	vec->data = realloc(vec->data, vec->size * vec->elsz);
}

/**
 * Push a new element to the end of the vector
 * Returns false if the vector was invalid
 */
bool vect_push(Vector *vec, void *el)
{
	if (vec->elsz == 0 || vec->size == 0)
		return false;

	memcpy(vec->data + (vec->count * vec->elsz), el, vec->elsz);
	vec->count++;

	if (vec->size == vec->count)
	{
		_vect_grow(vec);
	}

	return true;
}

/**
 * Pop an element off of the end of the vector
 * Returns true if an element was removed, false if there was no element to remove.
 */
bool vect_pop(Vector *vec)
{
	if (vec->count < 1)
		return false;

	vec->count--;
	
	if (vec->count < vec->size / 3)
	{
		_vect_shrink(vec);
	}

	return true;
}

/**
 * Get an element from the vector
 * Returns NULL if the index is invalid
 */
void *vect_get(Vector *vec, unsigned int index)
{
	if (index >= vec->count)
		return NULL;

	return vec->data + (index * vec->elsz);
}

/**
 * Set an element in the vector
 * If the index is outside the vector, returns false.
 */
bool vect_set(Vector *vec, unsigned int index, void *el)
{
	if (index >= vec->count)
		return false;

	memcpy(vec->data + (index * vec->elsz), el, vec->elsz);
	return true;
}

/**
 * Remove an element from the given position in the vector
 * Returns false if the index was invalid
 * O(n)
 */
bool vect_remove(Vector *vec, unsigned int index)
{
	if (index >= vec->count)
		return false;

	if (index == vec->count - 1)
	{
		return vect_pop(vec);
	}

	char *curr = vec->data + (vec->elsz * index);
	char *next = vec->data + (vec->elsz * (index + 1));

	for(int i = 0; i < vec->elsz * (vec->count - (index + 1)); i++)
	{
		*(curr + i) = *(next + i);
	}
	
	return vect_pop(vec);
}

/**
 * Add an element into the vector
 * Returns false if the index is invalid
 * O(n)
 */
bool vect_insert(Vector *vec, unsigned int index, void *el)
{
	if (index > vec->count)
		return false;

	if (index == vec->count)
		return vect_push(vec, el);

	char *curr = vec->data + (vec->elsz * index);
	char *next = vec->data + (vec->elsz * (index + 1));

	for (int i = vec->elsz * (vec->count - (index + 1)); i > 0; i--)
	{
		*(next + i - 1) = *(curr + i - 1);
	}

	vec->count++;

	if (vec->size == vec->count)
	{
		_vect_grow(vec);
	}

	return true;
}

/**
 * Clear all data from the vector
 */
void vect_clear(Vector *vec)
{
	int elsz = vec->elsz;
	vect_end(vec);
	*vec = vect_init(elsz);
}

/**
 * Free associated data of the vector
 */
void vect_end(Vector *vec)
{
	vec->size = 0;
	vec->count = 0;
	vec->elsz = 0;
	free(vec->data);
	vec->data = NULL;
}


