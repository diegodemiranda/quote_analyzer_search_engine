#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl_operations.h"
#include "word_processing.h" // For add_citation_to_word

// --- AVL Utility Functions ---

// Get height of node (returns 0 for NULL node)
static int height_avl(AVLNode *N) {
    if (N == NULL)
        return 0;
    return N->height;
}

// Get maximum of two integers
static int max_avl(int a, int b) {
    return (a > b) ? a : b;
}

// Create a new AVL node
static AVLNode* create_avl_node(WordInfo *wordInfo) {
    AVLNode* node = (AVLNode*)malloc(sizeof(AVLNode));
    if (!node) {
        perror("Failed to allocate AVL node");
        return NULL;
    }
    node->data = wordInfo; // Store pointer to existing WordInfo
    node->left = NULL;
    node->right = NULL;
    node->height = 1; // New node is initially added at leaf
    return node;
}

// Right rotate subtree rooted with y
static AVLNode *right_rotate_avl(AVLNode *y) {
    AVLNode *x = y->left;
    AVLNode *T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update heights
    y->height = max_avl(height_avl(y->left), height_avl(y->right)) + 1;
    x->height = max_avl(height_avl(x->left), height_avl(x->right)) + 1;

    // Return new root
    return x;
}

// Left rotate subtree rooted with x
static AVLNode *left_rotate_avl(AVLNode *x) {
    AVLNode *y = x->right;
    AVLNode *T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;

    // Update heights
    x->height = max_avl(height_avl(x->left), height_avl(x->right)) + 1;
    y->height = max_avl(height_avl(y->left), height_avl(y->right)) + 1;

    // Return new root
    return y;
}

// Get Balance factor of node N
static int get_balance_avl(AVLNode *N) {
    if (N == NULL)
        return 0;
    return height_avl(N->left) - height_avl(N->right);
}

// --- AVL Insertion ---

// Inserts a WordInfo pointer into the AVL tree (Recursive)
AVLNode* insert_avl(AVLNode *node, WordInfo *wordInfo, const char *quote, const char *movie, int year) {
    // 1. Perform the normal BST insertion
    if (node == NULL) {
        // WordInfo should already exist (created by vector insert)
        if (!wordInfo) return NULL;
         // Frequency/citation handled by vector insert.
        return(create_avl_node(wordInfo));
    }


    int cmp = strcmp(wordInfo->word, node->data->word);

    if (cmp < 0)
        node->left = insert_avl(node->left, wordInfo, quote, movie, year);
    else if (cmp > 0)
        node->right = insert_avl(node->right, wordInfo, quote, movie, year);
    else {
        // Duplicate word found in AVL tree node.
        // Actual frequency/citation update happened in vector.
        // No action needed here for AVL node itself.
        return node; // Return unchanged node
    }


    // 2. Update height of this ancestor node
    node->height = 1 + max_avl(height_avl(node->left), height_avl(node->right));

    // 3. Get the balance factor of this ancestor node to check balance
    int balance = get_balance_avl(node);

    // 4. If node becomes unbalanced, there are 4 cases

    // Left Left Case
    if (balance > 1 && strcmp(wordInfo->word, node->left->data->word) < 0)
        return right_rotate_avl(node);

    // Right Right Case
    if (balance < -1 && strcmp(wordInfo->word, node->right->data->word) > 0)
        return left_rotate_avl(node);

    // Left Right Case
    if (balance > 1 && strcmp(wordInfo->word, node->left->data->word) > 0) {
        node->left = left_rotate_avl(node->left);
        return right_rotate_avl(node);
    }

    // Right Left Case
    if (balance < -1 && strcmp(wordInfo->word, node->right->data->word) < 0) {
        node->right = right_rotate_avl(node->right);
        return left_rotate_avl(node);
    }

    // Return the (possibly updated) node pointer
    return node;
}

// --- AVL Search ---

// Searches for a word in the AVL tree (Recursive - same as BST search)
WordInfo* search_avl(AVLNode *root, const char *word) {
     if (root == NULL) {
        return NULL; // Not found
    }

    int cmp = strcmp(word, root->data->word);

    if (cmp == 0) {
        return root->data; // Found
    } else if (cmp < 0) {
        return search_avl(root->left, word);
    } else {
        return search_avl(root->right, word);
    }
}

// --- AVL Free ---

// Frees the memory allocated for the AVL tree nodes (not the WordInfo structs).
void free_avl(AVLNode *root) {
     if (root != NULL) {
        free_avl(root->left);
        free_avl(root->right);
        // IMPORTANT: Only free the node, not root->data (WordInfo)
        free(root);
    }
}