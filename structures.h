#ifndef STRUCTURES_H
#define STRUCTURES_H

// --- Basic Nodes ---

// Structure to store information about where a word appears
typedef struct CitationInfo {
  char *quote;
  char *movie;
  int year;
  struct CitationInfo *next; // Linked list for multiple occurrences in different quotes
} CitationInfo;

// Structure to store a unique word, its frequency, and list of citations
typedef struct WordInfo {
  char *word;
  int frequency;
  CitationInfo *citations; // Head of the linked list of citations
} WordInfo;

// --- Structure Nodes ---

// Node for Binary Search Tree (BST)
typedef struct BSTNode {
  WordInfo *data;         // Pointer to the shared WordInfo
  struct BSTNode *left;
  struct BSTNode *right;
} BSTNode;

// Node for AVL Tree
typedef struct AVLNode {
  WordInfo *data;         // Pointer to the shared WordInfo
  struct AVLNode *left;
  struct AVLNode *right;
  int height;
} AVLNode;

// Node for AVL Tree sorted by Frequency
typedef struct FreqAVLNode {
  WordInfo *data;          // Pointer to the shared WordInfo
  struct FreqAVLNode *left;
  struct FreqAVLNode *right;
  int height;
} FreqAVLNode;

// Structure to hold the dynamic array for binary search
typedef struct WordVector {
  WordInfo **words; // Array of pointers to WordInfo
  int size;         // Current number of elements
  int capacity;     // Current allocated capacity
} WordVector;


#endif // STRUCTURES_H