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
	if (vec->size == 1)
	{
		return;
	}

	vec->size /= 2;
	vec->data = realloc(vec->data, vec->size * vec->elsz);
}

/**
 * Push a new element to the end of the vector
 */
void vect_push(Vector *vec, void *el)
{
	memcpy(vec->data + (vec->count * vec->elsz), el, vec->elsz);
	vec->count++;

	if (vec->size == vec->count)
	{
		_vect_grow(vec);
	}
}

/**
 * Pop an element off of the end of the vector
 */
void vect_pop(Vector *vec)
{
	vec->count--;
	
	if (vec->count < vec->size / 3)
	{
		_vect_shrink(vec);
	}
}

void vect_end(Vector *vec)
{
	vec->size = 0;
	vec->count = 0;
	vec->elsz = 0;
	free(vec->data);
	vec->data = NULL;
}
