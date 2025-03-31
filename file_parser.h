#ifndef FILE_PARSER_H
#define FILE_PARSER_H

#include "structures.h" // Needs struct definitions
#include <time.h>       // For clock_t

// Structure to hold timing results for loading
typedef struct LoadTimes {
  double vector_time_ms;
  double bst_time_ms;
  double avl_time_ms;
} LoadTimes;

// Parses the movie quotes file and populates the data structures.
// Returns timings for each structure's loading process.
// Takes pointers to the data structure roots/vector to modify them.
LoadTimes load_data_from_file(const char *filename, WordVector *vec, BSTNode **bst_root, AVLNode **avl_root);

#endif // FILE_PARSER_H