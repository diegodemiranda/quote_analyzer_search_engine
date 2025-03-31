#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "structures.h"
#include "file_parser.h"
#include "word_processing.h"
#include "array_operations.h"
#include "bst_operations.h"
#include "avl_operations.h"
#include "freq_avl_operations.h"
#include "utils.h"

// --- Global Data Structures ---
WordVector word_vector = {NULL, 0, 0};
BSTNode *bst_root = NULL;
AVLNode *avl_root = NULL;
FreqAVLNode *freq_avl_root = NULL;

int data_loaded = 0; // Flag to check if data is loaded

// --- Function Prototypes ---
void display_menu();
void handle_load_file();
void handle_search_word();
void handle_search_frequency();
void cleanup_memory();
void display_citations(CitationInfo *citations);

// --- Main Function ---
int main() {
    int choice;

    atexit(cleanup_memory);

    do {
        display_menu();
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            clear_input_buffer(); // Clear invalid input
            choice = -1; // Set choice to invalid to loop again
            continue;
        }
        clear_input_buffer(); // Consume the newline character

        switch (choice) {
            case 1:
                handle_load_file();
                break;
            case 2:
                if (!data_loaded) {
                    printf("Error: No data loaded. Please load a file first (Option 1).\n");
                } else {
                    handle_search_word();
                }
                break;
            case 3:
                 if (!data_loaded) {
                    printf("Error: No data loaded. Please load a file first (Option 1).\n");
                } else {
                    handle_search_frequency();
                }
                break;
            case 0:
                printf("Exiting program.\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }
        printf("\n"); // Add a newline for better readability

    } while (choice != 0);

    return 0; // cleanup_memory() will be called automatically
}

// --- Menu and Handlers ---

void display_menu() {
    printf("========================================\n");
    printf("   Movie Quote Word Analyzer Menu\n");
    printf("========================================\n");
    printf("1. Load Movie Quotes File\n");
    printf("2. Search for a Word\n");
    printf("3. Search by Word Frequency Range\n");
    printf("0. Exit\n");
    printf("----------------------------------------\n");
}

void handle_load_file() {
    char filename[256];

    // --- Cleanup existing data before loading new ---
    if (data_loaded) {
        printf("Clearing existing data...\n");
        cleanup_memory(); // Free existing structures
        // Re-initialize global pointers/flags
        word_vector.words = NULL; word_vector.size = 0; word_vector.capacity = 0;
        bst_root = NULL;
        avl_root = NULL;
        freq_avl_root = NULL;
        data_loaded = 0;
        printf("Existing data cleared.\n");
    }


    printf("Enter the filename to load (e.g., movie_quotes.txt): ");
    if (scanf("%255s", filename) != 1) {
         printf("Error reading filename.\n");
         clear_input_buffer();
         return;
    }
    clear_input_buffer(); // Consume newline

    LoadTimes times = load_data_from_file(filename, &word_vector, &bst_root, &avl_root);

    if (times.vector_time_ms >= 0) { // Check for loading errors indicated by -1
        printf("\n--- Data Loading Performance ---\n");
        printf("Sorted Vector (Binary Search): %.4f ms\n", times.vector_time_ms);
        printf("Binary Search Tree (BST):      %.4f ms\n", times.bst_time_ms);
        printf("AVL Tree:                      %.4f ms\n", times.avl_time_ms);
        data_loaded = 1;

        // --- Pre-build Frequency AVL Tree ---
        printf("\nBuilding Frequency AVL Tree...\n");
        clock_t start_freq = timer_start();
        freq_avl_root = build_freq_avl_from_vector(&word_vector);
        double freq_build_time = timer_stop(start_freq);
        if (freq_avl_root) {
            printf("Frequency AVL Tree built successfully (%.4f ms).\n", freq_build_time);
        } else {
             printf("Warning: Frequency AVL Tree construction failed or resulted in an empty tree.\n");
        }

    } else {
        printf("Failed to load data from file '%s'.\n", filename);
        data_loaded = 0; // Ensure flag is off
    }
}

void handle_search_word() {
    char search_term[100];
    char *normalized_term = NULL;
    WordInfo *found_info = NULL;
    clock_t start_time;
    double elapsed_time;

    printf("Enter the word to search for: ");
     if (scanf("%99s", search_term) != 1) {
         printf("Error reading search term.\n");
         clear_input_buffer();
         return;
     }
    clear_input_buffer();

    normalized_term = normalize_word(search_term); // Normalize the input term

    if (!normalized_term) {
        printf("Invalid search term (must be > 3 alphabetic characters after cleaning).\n");
        return;
    }
     printf("Searching for normalized word: '%s'\n", normalized_term);
     printf("----------------------------------------\n");

    // --- 1. Search Vector ---
    printf("1. Searching in Sorted Vector (Binary Search)...\n");
    start_time = timer_start();
    found_info = search_vector(&word_vector, normalized_term);
    elapsed_time = timer_stop(start_time);
    if (found_info) {
        printf("   Found! Frequency: %d (Search Time: %.6f ms)\n", found_info->frequency, elapsed_time);
        display_citations(found_info->citations);
    } else {
        printf("   Not found in Sorted Vector (Search Time: %.6f ms)\n", elapsed_time);
    }
     printf("----------------------------------------\n");

    // --- 2. Search BST ---
    printf("2. Searching in Binary Search Tree (BST)...\n");
    start_time = timer_start();
    found_info = search_bst(bst_root, normalized_term);
    elapsed_time = timer_stop(start_time);
     if (found_info) {
        // Frequency and citations are the same as they point to the same WordInfo
        printf("   Found! Frequency: %d (Search Time: %.6f ms)\n", found_info->frequency, elapsed_time);
         // display_citations(found_info->citations); // No need to print again
    } else {
        printf("   Not found in BST (Search Time: %.6f ms)\n", elapsed_time);
    }
     printf("----------------------------------------\n");

    // --- 3. Search AVL ---
    printf("3. Searching in AVL Tree...\n");
    start_time = timer_start();
    found_info = search_avl(avl_root, normalized_term);
    elapsed_time = timer_stop(start_time);
     if (found_info) {
        // Frequency and citations are the same
        printf("   Found! Frequency: %d (Search Time: %.6f ms)\n", found_info->frequency, elapsed_time);
         // display_citations(found_info->citations); // No need to print again
    } else {
        printf("   Not found in AVL Tree (Search Time: %.6f ms)\n", elapsed_time);
    }
     printf("----------------------------------------\n");

    free(normalized_term); // Free the normalized search term
}


void handle_search_frequency() {
    int min_freq, max_freq;

    if (!freq_avl_root) {
        printf("Error: Frequency index (AVL Tree) is not built or is empty.\n");
        return;
    }


    printf("Enter minimum frequency: ");
    if (scanf("%d", &min_freq) != 1 || min_freq < 0) {
        printf("Invalid minimum frequency.\n");
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    printf("Enter maximum frequency: ");
     if (scanf("%d", &max_freq) != 1 || max_freq < min_freq) {
        printf("Invalid maximum frequency (must be >= minimum frequency).\n");
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    printf("\n--- Searching for words with frequency between %d and %d ---\n", min_freq, max_freq);
    printf("(Using Frequency-Sorted AVL Tree)\n");

    clock_t start_time = timer_start();
    search_freq_range_avl(freq_avl_root, min_freq, max_freq); // Perform the search
    double elapsed_time = timer_stop(start_time);

    printf("----------------------------------------\n");
    printf("Frequency range search completed in %.6f ms.\n", elapsed_time);

}


void display_citations(CitationInfo *citations) {
    CitationInfo *current = citations;
    int count = 0;
    printf("   Citations:\n");
    while (current != NULL && count < 10) { // Limit output for brevity
        printf("    - Quote: \"%.50s...\"\n", current->quote); // Show part of quote
        printf("      Movie: %s (%d)\n", current->movie, current->year);
        current = current->next;
        count++;
    }
     if (current != NULL) {
         printf("    - ... (and more citations)\n");
     }
     if (count == 0) {
         printf("    - (No citations found - data inconsistency?)\n");
     }
}

void cleanup_memory() {
    printf("\nCleaning up allocated memory...\n");

    // Free the tree nodes first (they only hold pointers)
    free_bst(bst_root);
    bst_root = NULL; // Prevent double free if called again
    free_avl(avl_root);
    avl_root = NULL;
    free_freq_avl(freq_avl_root);
    freq_avl_root = NULL;

    // Free the vector and the WordInfo structs it owns
    free_vector(&word_vector); // This handles WordInfo and Citations

    data_loaded = 0; // Reset flag

    printf("Memory cleanup complete.\n");
}