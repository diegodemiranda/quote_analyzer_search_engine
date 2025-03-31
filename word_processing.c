#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "word_processing.h"

// Normalizes a word: converts to lowercase, removes punctuation at start/end.
// Keeps internal hyphens/apostrophes if needed.
// Returns a new dynamically allocated string, or NULL if word is invalid/too short.
char* normalize_word(const char *raw_word) {
    if (raw_word == NULL) return NULL;

    int len = strlen(raw_word);
    char *cleaned_word = (char *)malloc(len + 1);
    if (!cleaned_word) {
        perror("Failed to allocate memory for cleaned word");
        return NULL;
    }

    int k = 0;
    for (int i = 0; i < len; i++) {
        // Keep only alphabetic characters
        if (isalpha((unsigned char)raw_word[i])) {
            cleaned_word[k++] = tolower((unsigned char)raw_word[i]);
        }
        // Add more sophisticated handling for hyphens/apostrophes if needed
    }
    cleaned_word[k] = '\0'; // Null-terminate

    // Check length constraint AFTER cleaning
    if (strlen(cleaned_word) <= 3) {
        free(cleaned_word);
        return NULL; // Word is too short or became empty after cleaning
    }

    return cleaned_word;
}

// Creates a new WordInfo structure
WordInfo* create_word_info(const char *word) {
    WordInfo *newInfo = (WordInfo *)malloc(sizeof(WordInfo));
    if (!newInfo) {
        perror("Failed to allocate memory for WordInfo");
        return NULL;
    }
    newInfo->word = strdup(word); // Duplicate the word string
    if (!newInfo->word) {
         perror("Failed to duplicate word string");
         free(newInfo);
         return NULL;
    }
    newInfo->frequency = 0; // Initial frequency will be set during insertion
    newInfo->citations = NULL;
    return newInfo;
}

// Creates a new CitationInfo structure
CitationInfo* create_citation_info(const char *quote, const char *movie, int year) {
    CitationInfo *newCitation = (CitationInfo *)malloc(sizeof(CitationInfo));
    if (!newCitation) {
        perror("Failed to allocate memory for CitationInfo");
        return NULL;
    }
    // Duplicate strings to avoid issues with buffer overwrites
    newCitation->quote = strdup(quote);
    newCitation->movie = strdup(movie);
    if (!newCitation->quote || !newCitation->movie) {
        perror("Failed to duplicate citation strings");
        free(newCitation->quote); // Free potentially allocated quote
        free(newCitation->movie); // Free potentially allocated movie
        free(newCitation);
        return NULL;
    }
    newCitation->year = year;
    newCitation->next = NULL;
    return newCitation;
}

// Adds a citation to the beginning of the citation list for a word.
void add_citation_to_word(WordInfo *wordInfo, const char *quote, const char *movie, int year) {
    if (!wordInfo) return;

    CitationInfo *newCitation = create_citation_info(quote, movie, year);
    if (!newCitation) {
        fprintf(stderr, "Warning: Failed to create citation info.\n");
        return; // Failed to create citation, skip adding it
    }

    // Add to the front of the list
    newCitation->next = wordInfo->citations;
    wordInfo->citations = newCitation;
}

// Frees the memory allocated for a CitationInfo linked list.
void free_citation_list(CitationInfo *head) {
    CitationInfo *current = head;
    CitationInfo *next;
    while (current != NULL) {
        next = current->next;
        free(current->quote);
        free(current->movie);
        free(current);
        current = next;
    }
}

// Frees the memory allocated for a WordInfo structure, including its citation list.
void free_word_info(WordInfo *wordInfo) {
    if (wordInfo) {
        free(wordInfo->word);
        free_citation_list(wordInfo->citations);
        free(wordInfo);
    }
}