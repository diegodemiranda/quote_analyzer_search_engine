#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "array_operations.h"
#include "word_processing.h" // For create_word_info, add_citation_to_word, free_word_info

// Initializes a WordVector
void init_vector(WordVector *vec, int initial_capacity) {
    vec->words = (WordInfo **)malloc(initial_capacity * sizeof(WordInfo *));
    if (!vec->words) {
        perror("Failed to allocate vector");
        exit(EXIT_FAILURE);
    }
    vec->size = 0;
    vec->capacity = initial_capacity;
}

// Helper function to resize the vector
static int resize_vector(WordVector *vec) {
    int new_capacity = vec->capacity * 2;
    WordInfo **new_words = (WordInfo **)realloc(vec->words, new_capacity * sizeof(WordInfo *));
    if (!new_words) {
        perror("Failed to resize vector");
        return 0; // Failure
    }
    vec->words = new_words;
    vec->capacity = new_capacity;
    return 1; // Success
}

// Inserts WordInfo into the sorted vector. Handles duplicates.
// Returns pointer to WordInfo in vector
WordInfo* insert_sorted_vector(WordVector *vec, const char *word, const char *quote, const char *movie, int year) {
    // --- Binary search to find position or existing word ---
    int low = 0, high = vec->size - 1;
    int mid;
    int cmp = -1; // Initialize cmp to handle empty vector case correctly
    int found_index = -1;

    while (low <= high) {
        mid = low + (high - low) / 2; // Avoid overflow
        cmp = strcmp(word, vec->words[mid]->word);

        if (cmp == 0) {
            // Word found
            found_index = mid;
            break;
        } else if (cmp < 0) {
            high = mid - 1;
        } else {
            low = mid + 1;
        }
    }

    // --- Handle insertion or update ---
    WordInfo *target_info;

    if (found_index != -1) {
        // Word exists, update frequency and add citation
        target_info = vec->words[found_index];
        target_info->frequency++;
        add_citation_to_word(target_info, quote, movie, year);
    } else {
        // Word not found, insert in sorted order
        if (vec->size >= vec->capacity) {
            if (!resize_vector(vec)) {
                fprintf(stderr, "Error: Could not resize vector, skipping insertion of '%s'\n", word);
                return NULL; // Indicate failure
            }
        }

        // The correct insertion index is 'low' (where the search ended)
        int insert_pos = low;

        // Shift elements to make space
        for (int i = vec->size; i > insert_pos; i--) {
            vec->words[i] = vec->words[i - 1];
        }

        // Create new WordInfo
        target_info = create_word_info(word);
        if (!target_info) {
             fprintf(stderr, "Error: Could not create WordInfo for '%s', skipping insertion.\n", word);
             // Shift back if needed? No, just don't increment size.
             return NULL; // Indicate failure
        }
        target_info->frequency = 1; // First occurrence
        add_citation_to_word(target_info, quote, movie, year);

        // Insert the new WordInfo pointer
        vec->words[insert_pos] = target_info;
        vec->size++;
    }
    return target_info; // Return pointer to the WordInfo in the vector
}


// Searches for a word in the vector using binary search.
WordInfo* search_vector(const WordVector *vec, const char *word) {
    int low = 0, high = vec->size - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        int cmp = strcmp(word, vec->words[mid]->word);
        if (cmp == 0) {
            return vec->words[mid]; // Found
        } else if (cmp < 0) {
            high = mid - 1;
        } else {
            low = mid + 1;
        }
    }
    return NULL; // Not found
}

// Frees the memory used by the WordVector
// IMPORTANT: This assumes the vector OWNS the WordInfo structures.
void free_vector(WordVector *vec) {
    if (vec && vec->words) {
        for (int i = 0; i < vec->size; i++) {
            if (vec->words[i]) {
                free_word_info(vec->words[i]); // Free the WordInfo struct and its contents
            }
        }
        free(vec->words); // Free the array of pointers
        vec->words = NULL;
        vec->size = 0;
        vec->capacity = 0;
    }
}