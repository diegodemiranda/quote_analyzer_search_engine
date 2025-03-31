#ifndef WORD_PROCESSING_H
#define WORD_PROCESSING_H

#include "structures.h"

// Normalizes a word: converts to lowercase, removes punctuation.
// Returns a new dynamically allocated string, or NULL if word is invalid/too short.
// The caller is responsible for freeing the returned string.
char* normalize_word(const char *raw_word);

// Creates a new WordInfo structure. Remember to free it later.
WordInfo* create_word_info(const char *word);

// Creates a new CitationInfo structure. Remember to free it later.
CitationInfo* create_citation_info(const char *quote, const char *movie, int year);

// Adds a citation to the beginning of the citation list for a word.
void add_citation_to_word(WordInfo *wordInfo, const char *quote, const char *movie, int year);

// Frees the memory allocated for a CitationInfo linked list.
void free_citation_list(CitationInfo *head);

// Frees the memory allocated for a WordInfo structure, including its citation list.
void free_word_info(WordInfo *wordInfo);


#endif // WORD_PROCESSING_H