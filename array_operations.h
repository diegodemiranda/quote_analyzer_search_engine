#ifndef ARRAY_OPERATIONS_H
#define ARRAY_OPERATIONS_H

#include "structures.h"

// Initializes a WordVector
void init_vector(WordVector *vec, int initial_capacity);

// Inserts WordInfo into the sorted vector. Handles duplicates (increments frequency).
// Returns a pointer to the (potentially new) WordInfo struct in the vector.
WordInfo* insert_sorted_vector(WordVector *vec, const char *word, const char *quote, const char *movie, int year);

// Searches for a word in the vector using binary search.
// Returns a pointer to the WordInfo if found, NULL otherwise.
WordInfo* search_vector(const WordVector *vec, const char *word);

// Frees the memory used by the WordVector, including the WordInfo structs it owns.
void free_vector(WordVector *vec);

#endif // ARRAY_OPERATIONS_H