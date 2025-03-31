#ifndef AVL_OPERATIONS_H
#define AVL_OPERATIONS_H

#include "structures.h"

// Inserts a WordInfo pointer into the AVL Tree.
// Does NOT duplicate WordInfo, assumes pointer is valid and managed elsewhere.
// Updates frequency and citations if word already exists (indirectly via vector).
// Balances the tree as needed.
AVLNode* insert_avl(AVLNode *node, WordInfo *wordInfo, const char *quote, const char *movie, int year);

// Searches for a word in the AVL tree.
// Returns a pointer to the WordInfo if found, NULL otherwise.
WordInfo* search_avl(AVLNode *root, const char *word);

// Frees the memory allocated for the AVL tree nodes (not the WordInfo structs).
void free_avl(AVLNode *root);

#endif // AVL_OPERATIONS_H