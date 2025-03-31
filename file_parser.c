#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "file_parser.h"
#include "word_processing.h"
#include "array_operations.h"
#include "bst_operations.h"
#include "avl_operations.h"
#include "utils.h"

#define MAX_LINE_LENGTH 2048
#define INITIAL_VECTOR_CAPACITY 1000

// Helper to trim leading/trailing whitespace and quotes from a string in-place
static void trim_quotes_whitespace(char *str) {
    if (!str) return;
    char *start = str;
    char *end = str + strlen(str) - 1;

    // Trim leading space/quotes
    while (isspace((unsigned char)*start) || *start == '"') {
        start++;
    }

    // Trim trailing space/quotes
    while (end > start && (isspace((unsigned char)*end) || *end == '"')) {
        end--;
    }

    // Null-terminate the trimmed string
    *(end + 1) = '\0';

    // Shift the string if needed (if leading chars were trimmed)
    if (start != str) {
        memmove(str, start, (end - start) + 2); // +1 for char, +1 for null terminator
    }
}

// Parses the movie quotes file
LoadTimes load_data_from_file(const char *filename, WordVector *vec, BSTNode **bst_root, AVLNode **avl_root) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        LoadTimes times = { -1.0, -1.0, -1.0 }; // Indicate error
        return times;
    }

    char line[MAX_LINE_LENGTH];
    char quote_buffer[MAX_LINE_LENGTH];
    char movie_buffer[MAX_LINE_LENGTH];
    char year_str[20]; // Ample space for year + quotes + null terminator

    // Initialize structures
    init_vector(vec, INITIAL_VECTOR_CAPACITY);
    *bst_root = NULL;
    *avl_root = NULL;

    LoadTimes times = {0.0, 0.0, 0.0};
    clock_t start_vec, start_bst, start_avl;
    int line_num = 0;

    printf("Loading data from %s...\n", filename);

    while (fgets(line, sizeof(line), file)) {
        line_num++;
        line[strcspn(line, "\r\n")] = 0; // Remove trailing newline/carriage return

        // --- Simple CSV Parsing (Assumes format "Quote","Movie","Year") ---
        char *first_quote = strchr(line, '"');
        if (!first_quote) continue; // Skip lines not starting with quote

        char *end_first_quote = strchr(first_quote + 1, '"');
        if (!end_first_quote) continue; // Malformed quote

        char *comma1 = strchr(end_first_quote, ',');
        if (!comma1) continue; // Missing first comma

        char *second_quote = strchr(comma1, '"');
        if (!second_quote) continue; // Missing movie quote

        char *end_second_quote = strchr(second_quote + 1, '"');
        if (!end_second_quote) continue; // Malformed movie

        char *comma2 = strchr(end_second_quote, ',');
        if (!comma2) continue; // Missing second comma

        char *third_quote = strchr(comma2, '"');
        if (!third_quote) continue; // Missing year quote

        char *end_third_quote = strchr(third_quote + 1, '"');
        if (!end_third_quote) continue; // Malformed year

        // Extract parts
        strncpy(quote_buffer, first_quote + 1, end_first_quote - first_quote - 1);
        quote_buffer[end_first_quote - first_quote - 1] = '\0';

        strncpy(movie_buffer, second_quote + 1, end_second_quote - second_quote - 1);
        movie_buffer[end_second_quote - second_quote - 1] = '\0';

        strncpy(year_str, third_quote + 1, end_third_quote - third_quote - 1);
        year_str[end_third_quote - third_quote - 1] = '\0';

        // Trim potential extra spaces (though format seems strict)
        // trim_quotes_whitespace(quote_buffer); // Usually not needed if format is exact
        // trim_quotes_whitespace(movie_buffer);
        // trim_quotes_whitespace(year_str);

        int year = atoi(year_str);
        if (year == 0 && strcmp(year_str, "0") != 0) { // Basic atoi error check
             fprintf(stderr, "Warning: Invalid year format on line %d: '%s'. Using 0.\n", line_num, year_str);
             year = 0; // Default or skip? Using 0 for now.
        }


        // --- Process words in the quote ---
        char *quote_copy = strdup(quote_buffer); // Work on a copy for strtok
        if (!quote_copy) {
             perror("Failed to duplicate quote buffer");
             continue;
        }
        char *token = strtok(quote_copy, " .,!?;:()[]{}-_\t\n\r"); // Define delimiters

        while (token != NULL) {
            char *normalized = normalize_word(token); // Cleans, checks length (>3), allocates memory
            if (normalized) {
                // --- Insert into Vector (and update/create WordInfo) ---
                start_vec = timer_start();
                WordInfo *word_in_vector = insert_sorted_vector(vec, normalized, quote_buffer, movie_buffer, year);
                times.vector_time_ms += timer_stop(start_vec);

                // --- Insert pointer into BST ---
                if (word_in_vector) { // Check if vector insertion was successful
                    start_bst = timer_start();
                    *bst_root = insert_bst(*bst_root, word_in_vector, quote_buffer, movie_buffer, year);
                    times.bst_time_ms += timer_stop(start_bst);

                    // --- Insert pointer into AVL ---
                    start_avl = timer_start();
                    *avl_root = insert_avl(*avl_root, word_in_vector, quote_buffer, movie_buffer, year);
                    times.avl_time_ms += timer_stop(start_avl);
                } else {
                    fprintf(stderr, "Warning: Failed to process word '%s' fully, skipping tree insertions.\n", normalized);
                }

                free(normalized); // Free the normalized word string
            }
            token = strtok(NULL, " .,!?;:()[]{}-_\t\n\r"); // Get next token
        }
        free(quote_copy); // Free the duplicated quote string
    }

    fclose(file);
    printf("Data loading complete. Processed %d unique words.\n", vec->size);
    return times;
}
