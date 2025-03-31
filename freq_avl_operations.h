#ifndef FREQ_AVL_OPERATIONS_H
#define FREQ_AVL_OPERATIONS_H

#include "structures.h"

// Inserts a WordInfo pointer into the Frequency AVL Tree based on frequency.
// Handles ties arbitrarily (or could use word as secondary key).
FreqAVLNode* insert_freq_avl(FreqAVLNode *node, WordInfo *wordInfo);

// Builds the Frequency AVL tree by traversing the WordVector.
FreqAVLNode* build_freq_avl_from_vector(const WordVector *vec);

// Searches for and prints words within a given frequency range (inclusive).
void search_freq_range_avl(FreqAVLNode *root, int min_freq, int max_freq);

// Frees the memory allocated for the Frequency AVL tree nodes (not WordInfo).
void free_freq_avl(FreqAVLNode *root);

#endif // FREQ_AVL_OPERATIONS_H