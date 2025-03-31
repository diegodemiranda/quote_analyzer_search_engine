#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bst_operations.h"
#include "word_processing.h" // For add_citation_to_word

// Creates a new BST node
static BSTNode* create_bst_node(WordInfo *wordInfo) {
    BSTNode *newNode = (BSTNode *)malloc(sizeof(BSTNode));
    if (!newNode) {
        perror("Failed to allocate BST node");
        return NULL; // Indicate failure
    }
    newNode->data = wordInfo; // Store pointer to existing WordInfo
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

// Inserts a WordInfo pointer into the BST (Recursive)
BSTNode* insert_bst(BSTNode *root, WordInfo *wordInfo, const char *quote, const char *movie, int year) {
    if (root == NULL) {
        // WordInfo should already exist (created by vector insert)
        // We just create the BST node pointing to it.
        if (!wordInfo) return NULL; // Should not happen if called correctly
        // If this is the *first* time this word is encountered *by the BST*,
        // the frequency/citation were already handled by the vector insert.
        // If the node *already* existed in the BST (handled below), then
        // frequency/citation were *also* handled by the vector insert.
        // So, no need to update frequency/citations here.
        return create_bst_node(wordInfo);
    }

    // Compare based on the word in the WordInfo struct
    int cmp = strcmp(wordInfo->word, root->data->word);

    if (cmp < 0) {
        root->left = insert_bst(root->left, wordInfo, quote, movie, year);
    } else if (cmp > 0) {
        root->right = insert_bst(root->right, wordInfo, quote, movie, year);
    } else {
        // Word already exists in the BST (node exists).
        // The actual frequency update and citation add happened
        // when insert_sorted_vector found the existing WordInfo.
        // No action needed here for the BST node itself.
        ; // Do nothing, node already points to the updated WordInfo
    }

    return root;
}


// Searches for a word in the BST (Recursive)
WordInfo* search_bst(BSTNode *root, const char *word) {
    if (root == NULL) {
        return NULL; // Not found
    }

    int cmp = strcmp(word, root->data->word);

    if (cmp == 0) {
        return root->data; // Found
    } else if (cmp < 0) {
        return search_bst(root->left, word);
    } else {
        return search_bst(root->right, word);
    }
}

// Frees the memory allocated for the BST nodes (not the WordInfo structs).
void free_bst(BSTNode *root) {
    if (root != NULL) {
        free_bst(root->left);
        free_bst(root->right);
        // IMPORTANT: Only free the node, not root->data (WordInfo)
        // as it's owned by the vector.
        free(root);
    }
}