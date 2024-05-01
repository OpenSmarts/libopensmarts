#ifndef OSM_UTILS_H
#define OSM_UTILS_H

#include <stdbool.h>

// Vector utilities

/**
 * Vector represents a dynamic array
 */
typedef struct {
	unsigned int count, size, elsz;
	void *data;
} Vector;

/**
 * Get an initialized vector struct
 */
Vector vect_init(unsigned int elsz);

/**
 * Add an element to an arbitrary index in the vector
 */
bool vect_add(Vector *vec, unsigned int index, void *el);

/**
 * Remove an element from an arbitrary index in the vector
 */
bool vect_remove(Vector *vec, unsigned int index);

/**
 * Push an element to the end of the vector
 */
bool vect_push(Vector *vec, void *el);

/**
 * Pop an element from the end of the vector
 */
bool vect_pop(Vector *vec);

/**
 * Get an element from the vector
 */
void *vect_get(Vector *vec, unsigned int index);

/**
 * Set an element inside the vector
 */
bool vect_set(Vector *vec, unsigned int index, void *el);

/**
 * Clear all data in a vector
 */
void vect_clear(Vector *vec);

/**
 * Remove all associated data from the vector
 */
void vect_end(Vector *vect);


#endif
