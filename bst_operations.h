#ifndef BST_OPERATIONS_H
#define BST_OPERATIONS_H

#include "structures.h"

// Inserts a WordInfo pointer into the BST.
// Does NOT duplicate WordInfo, assumes pointer is valid and managed elsewhere.
// Updates frequency and citations if word already exists.
BSTNode* insert_bst(BSTNode *root, WordInfo *wordInfo, const char *quote, const char *movie, int year);

// Searches for a word in the BST.
// Returns a pointer to the WordInfo if found, NULL otherwise.
WordInfo* search_bst(BSTNode *root, const char *word);

// Frees the memory allocated for the BST nodes (not the WordInfo structs).
void free_bst(BSTNode *root);

#endif // BST_OPERATIONS_H