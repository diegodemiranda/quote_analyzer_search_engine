#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freq_avl_operations.h"

// --- Freq AVL Utility Functions (Similar to word AVL, but compare frequency) ---

static int height_freq_avl(FreqAVLNode *N) {
    if (N == NULL) return 0;
    return N->height;
}

static int max_freq_avl(int a, int b) {
    return (a > b) ? a : b;
}

static FreqAVLNode* create_freq_avl_node(WordInfo *wordInfo) {
    FreqAVLNode* node = (FreqAVLNode*)malloc(sizeof(FreqAVLNode));
     if (!node) {
        perror("Failed to allocate Freq AVL node");
        return NULL;
    }
    node->data = wordInfo; // Store pointer
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    return node;
}

static FreqAVLNode *right_rotate_freq_avl(FreqAVLNode *y) {
    FreqAVLNode *x = y->left;
    FreqAVLNode *T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = max_freq_avl(height_freq_avl(y->left), height_freq_avl(y->right)) + 1;
    x->height = max_freq_avl(height_freq_avl(x->left), height_freq_avl(x->right)) + 1;
    return x;
}

static FreqAVLNode *left_rotate_freq_avl(FreqAVLNode *x) {
    FreqAVLNode *y = x->right;
    FreqAVLNode *T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = max_freq_avl(height_freq_avl(x->left), height_freq_avl(x->right)) + 1;
    y->height = max_freq_avl(height_freq_avl(y->left), height_freq_avl(y->right)) + 1;
    return y;
}

static int get_balance_freq_avl(FreqAVLNode *N) {
    if (N == NULL) return 0;
    return height_freq_avl(N->left) - height_freq_avl(N->right);
}

// --- Freq AVL Insertion ---

// Inserts WordInfo pointer based on frequency.
// Handles ties by inserting into the right subtree (arbitrary but consistent).
FreqAVLNode* insert_freq_avl(FreqAVLNode *node, WordInfo *wordInfo) {
    if (node == NULL)
        return(create_freq_avl_node(wordInfo));

    // Compare frequencies
    if (wordInfo->frequency < node->data->frequency)
        node->left = insert_freq_avl(node->left, wordInfo);
    else if (wordInfo->frequency > node->data->frequency)
        node->right = insert_freq_avl(node->right, wordInfo);
    else {
        // Tie in frequency - insert into right subtree for simplicity
        // A secondary comparison (e.g., alphabetical by word) could be added here.
        node->right = insert_freq_avl(node->right, wordInfo);
        // Note: If we allow multiple nodes for same frequency,
        // the structure needs modification (e.g., list at node).
        // For this simple approach, we just place it somewhere consistently.
    }


    // Update height
    node->height = 1 + max_freq_avl(height_freq_avl(node->left), height_freq_avl(node->right));

    // Get balance factor
    int balance = get_balance_freq_avl(node);

    // Balance the tree (4 cases)
    // Left Left Case
    if (balance > 1 && wordInfo->frequency < node->left->data->frequency)
        return right_rotate_freq_avl(node);

    // Right Right Case
    if (balance < -1 && wordInfo->frequency >= node->right->data->frequency) // >= due to tie handling
        return left_rotate_freq_avl(node);

    // Left Right Case
    if (balance > 1 && wordInfo->frequency >= node->left->data->frequency) { // >= due to tie handling
        node->left = left_rotate_freq_avl(node->left);
        return right_rotate_freq_avl(node);
    }

    // Right Left Case
    if (balance < -1 && wordInfo->frequency < node->right->data->frequency) {
        node->right = right_rotate_freq_avl(node->right);
        return left_rotate_freq_avl(node);
    }

    return node;
}

// --- Build Freq AVL ---

// Builds the Frequency AVL tree by iterating through the WordVector
FreqAVLNode* build_freq_avl_from_vector(const WordVector *vec) {
    FreqAVLNode *freq_root = NULL;
    if (!vec) return NULL;

    for (int i = 0; i < vec->size; i++) {
        if (vec->words[i]) {
             freq_root = insert_freq_avl(freq_root, vec->words[i]);
        }
    }
    return freq_root;
}

// --- Search Freq Range ---

// In-order traversal to find words within the frequency range.
void search_freq_range_avl(FreqAVLNode *root, int min_freq, int max_freq) {
    if (root == NULL) {
        return;
    }

    // If current node's frequency is greater than min, check left subtree
    if (root->data->frequency > min_freq) {
        search_freq_range_avl(root->left, min_freq, max_freq);
    }

    // Check if current node's frequency is within the range
    if (root->data->frequency >= min_freq && root->data->frequency <= max_freq) {
        printf("  - Word: '%s', Frequency: %d\n", root->data->word, root->data->frequency);
    }

     // If current node's frequency is less than max, check right subtree
    if (root->data->frequency < max_freq) {
         search_freq_range_avl(root->right, min_freq, max_freq);
    }
}


// --- Free Freq AVL ---

// Frees the memory allocated for the Frequency AVL tree nodes (not WordInfo).
void free_freq_avl(FreqAVLNode *root) {
    if (root != NULL) {
        free_freq_avl(root->left);
        free_freq_avl(root->right);
        // IMPORTANT: Only free the node, not root->data (WordInfo)
        free(root);
    }
}