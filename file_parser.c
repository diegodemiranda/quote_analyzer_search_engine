#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "file_parser.h"
#include "word_processing.h"
#include "array_operations.h"
#include "bst_operations.h"
#include "avl_operations.h"
#include "utils.h"

#define MAX_LINE_LENGTH 2048
#define INITIAL_VECTOR_CAPACITY 1000

static void trim_quotes_whitespace(char *str) {
    if (!str) return;
    char *start = str;
    char *end = str + strlen(str) - 1;

    while (isspace((unsigned char)*start) || *start == '"') {
        start++;
    }

    while (end > start && (isspace((unsigned char)*end) || *end == '"')) {
        end--;
    }

    *(end + 1) = '\0';

    if (start != str) {
        //'memmove()' foi usado em vez de 'strcpy()' porque lida corretamente com sobreposição de memória que ocorre
        //quando deslocamos caracteres dentro da mesma ‘string’.
        memmove(str, start, (end - start) + 2);
    }
}

LoadTimes load_data_from_file(const char *filename, WordVector *vec, BSTNode **bst_root, AVLNode **avl_root) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Falha ao abrir arquivo.");
        LoadTimes times = { -1.0, -1.0, -1.0 };
        return times;
    }

    char line[MAX_LINE_LENGTH];
    char quote_buffer[MAX_LINE_LENGTH];
    char movie_buffer[MAX_LINE_LENGTH];
    char year_str[20];

    init_vector(vec, INITIAL_VECTOR_CAPACITY);
    *bst_root = NULL;
    *avl_root = NULL;

    LoadTimes times = {0.0, 0.0, 0.0};
    int line_num = 0;

    printf("Carregando os dados do arquivo '%s'...\n", filename);

    while (fgets(line, sizeof(line), file)) {
        line_num++;
        line[strcspn(line, "\r\n")] = 0;

        char *first_quote = strchr(line, '"');
        if (!first_quote) continue;

        char *end_first_quote = strchr(first_quote + 1, '"');
        if (!end_first_quote) continue;

        char *comma1 = strchr(end_first_quote, ',');
        if (!comma1) continue;

        char *second_quote = strchr(comma1, '"');
        if (!second_quote) continue;

        char *end_second_quote = strchr(second_quote + 1, '"');
        if (!end_second_quote) continue;

        char *comma2 = strchr(end_second_quote, ',');
        if (!comma2) continue;

        char *third_quote = strchr(comma2, '"');
        if (!third_quote) continue;

        char *end_third_quote = strchr(third_quote + 1, '"');
        if (!end_third_quote) continue;

        strncpy(quote_buffer, first_quote + 1, end_first_quote - first_quote - 1);
        quote_buffer[end_first_quote - first_quote - 1] = '\0';

        strncpy(movie_buffer, second_quote + 1, end_second_quote - second_quote - 1);
        movie_buffer[end_second_quote - second_quote - 1] = '\0';

        strncpy(year_str, third_quote + 1, end_third_quote - third_quote - 1);
        year_str[end_third_quote - third_quote - 1] = '\0';

        int year = atoi(year_str);
        if (year == 0 && strcmp(year_str, "0") != 0) {
            fprintf(stderr, "Aviso: Formato do ano inválido na linha %d: '%s'. Usando 0.\n", line_num, year_str);
            year = 0;
        }

        // --- Processa as palavras da frase ---
        char *quote_copy = strdup(quote_buffer);
        if (!quote_copy) {
            perror("Falha ao copiar a frase");
            continue;
        }
        char *token = strtok(quote_copy, " .,!?;:()[]{}-_\t\n\r");

        while (token != NULL) {
            char *normalized = normalize_word(token);
            if (normalized) {
                clock_t start_vec = timer_start();
                WordInfo *word_in_vector = insert_sorted_vector(vec, normalized, quote_buffer, movie_buffer, year);
                times.vector_time_ms += timer_stop(start_vec);

                if (word_in_vector) {
                    clock_t start_bst = timer_start();
                    *bst_root = insert_bst(*bst_root, word_in_vector, quote_buffer, movie_buffer, year);
                    times.bst_time_ms += timer_stop(start_bst);

                    clock_t start_avl = timer_start();
                    *avl_root = insert_avl(*avl_root, word_in_vector, quote_buffer, movie_buffer, year);
                    times.avl_time_ms += timer_stop(start_avl);
                } else {
                    fprintf(stderr, "Aviso: falha ao processar a palavra '%s' por completo, pulando inserção na "
                                    "árvore.\n", normalized);
                }

                free(normalized);
            }
            token = strtok(NULL, " .,!?;:()[]{}-_\t\n\r");
        }
        free(quote_copy);
    }

    fclose(file);
    printf("Carregamento dos dados completo. Foram processadas %d palavras únicas.\n", vec->size);
    return times;
}
