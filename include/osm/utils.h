#ifndef OSM_UTILS_H
#define OSM_UTILS_H

// Vector utilities

/**
 * Vector represents a dynamic array
 */
typedef struct {
	unsigned int count, size, elsz;
	void *data;
} Vector;

Vector vect_init(unsigned int elsz);

/**
 * Add an element to an arbitrary index in the vector
 */
void vect_add(Vector *vec, unsigned int index);

/**
 * Remove an element from an arbitrary index in the vector
 */
void vect_remove(Vector *vec, unsigned int index);

/**
 * Push an element to the end of the vector
 */
void vect_push(Vector *vec, void *el);

/**
 * Pop an element from the end of the vector
 */
void vect_pop(Vector *vec);

/**
 * Get an element from the vector
 */
void *vect_get(Vector *vec, unsigned int index);

/**
 * Set an element inside the vector
 */
void *vect_set(Vector *vec, unsigned int index, void *el);

/**
 * Remove all associated data from the vector
 */
void vect_end(Vector *vect);


#endif
