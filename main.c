#include <stdio.h>
#include <stdlib.h>
#include "structures.h"
#include "file_parser.h"
#include "word_processing.h"
#include "array_operations.h"
#include "bst_operations.h"
#include "avl_operations.h"
#include "freq_avl_operations.h"
#include "utils.h"


WordVector word_vector = {NULL, 0, 0};
BSTNode *bst_root = NULL;
AVLNode *avl_root = NULL;
FreqAVLNode *freq_avl_root = NULL;
int data_loaded = 0;


void display_menu();
void handle_load_file();
void handle_search_word();
void handle_search_frequency();
void cleanup_memory();
void display_citations(CitationInfo *citations);


int main() {
    int choice;

    atexit(cleanup_memory);

    do {
        display_menu();
        printf("Entre com a sua escolha: ");
        if (scanf("%d", &choice) != 1) {
            printf("Entrada inválida! Por favor, digite um número.\n");
            clear_input_buffer();
            choice = -1;
            continue;
        }
        clear_input_buffer();

        switch (choice) {
            case 1:
                handle_load_file();
                break;
            case 2:
                if (!data_loaded) {
                    printf("Erro: dados não foram carregados. Por favor, carregue um arquivo primeiro (Opção 1).\n");
                } else {
                    handle_search_word();
                }
                break;
            case 3:
                if (!data_loaded) {
                    printf("Erro: dados não foram carregados. Por favor, carregue um arquivo primeiro (Opção 1).\n");
                } else {
                    handle_search_frequency();
                }
                break;
            case 0:
                printf("Saindo do programa.\n");
                break;
            default:
                printf("Opção inválida. Por favor, tente novamente.\n");
                break;
        }
        printf("\n");

    } while (choice != 0);

    return 0;
}

// --- Menu e Funções Auxiliares ---

void display_menu() {
    printf("==================================================\n"
    "Analisador de Palavras de Citações de Filmes\n"
    "==================================================\n"
    "1. Carregar arquivo de citações\n"
    "2. Busca por palavra\n"
    "3. Busca por intervalo de frequência\n"
    "0. Sair\n"
    "----------------------------------------\n");
}

void handle_load_file() {
    char filename[256];

    if (data_loaded) {
        printf("Eliminando dados existentes\n");
        cleanup_memory();

        word_vector.words = NULL; word_vector.size = 0; word_vector.capacity = 0;
        bst_root = NULL;
        avl_root = NULL;
        freq_avl_root = NULL;
        data_loaded = 0;
        printf("Dados existentes foram eliminados\n");
    }

    printf("Entre com o nome do arquivo (ex.: movie_quotes.txt): ");
    if (scanf("%255s", filename) != 1) {
        printf("Erro ao ler o arquivo\n");
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    const LoadTimes times = load_data_from_file(filename, &word_vector, &bst_root, &avl_root);

    if (times.vector_time_ms >= 0) {
        printf("\n--- Tempo de carregamento dos dados ---\n");
        printf("Vetor (busca binária)        : %.4f ms\n", times.vector_time_ms);
        printf("Árvore de Busca Binária (ABB): %.4f ms\n", times.bst_time_ms);
        printf("Árvore AVL                   : %.4f ms\n", times.avl_time_ms);
        data_loaded = 1;

        printf("\nConstruindo Árvore AVL de frequência\n");
        const clock_t start_freq = timer_start();
        freq_avl_root = build_freq_avl_from_vector(&word_vector);
        const double freq_build_time = timer_stop(start_freq);
        if (freq_avl_root) {
            printf("Árvore construída com sucesso (%.4f ms).\n", freq_build_time);
        } else {
            printf("Aviso: construção da Árvore AVL falhou ou gerou uma árvore vazia.\n");
        }

    } else {
        printf("Falha ao carregar os dados do arquivo '%s'.\n", filename);
        data_loaded = 0;
    }
}

void handle_search_word() {
    char search_term[100];
    char *normalized_term = NULL;
    WordInfo *found_info = NULL;

    printf("Entre com a palavra desejada para a busca: ");
    if (scanf("%99s", search_term) != 1) {
        printf("Erro ao ler a palavra.\n");
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    normalized_term = normalize_word(search_term);

    if (!normalized_term) {
        printf("Palavra inválida (ela deve possuir mais que 3 caracteres).\n");
        return;
    }
    printf("Procurando pela palavra: '%s'\n", normalized_term);
    printf("----------------------------------------\n");

    printf("1. Busca no vetor (busca binária)\n");
    clock_t start_time = timer_start();
    found_info = search_vector(&word_vector, normalized_term);
    double elapsed_time = timer_stop(start_time);
    if (found_info) {
        printf("   Palavra encontrada! Frequência: %d (Tempo de busca: %.6f ms)\n", found_info->frequency, elapsed_time);
        display_citations(found_info->citations);
    } else {
        printf("   Palavra não encontrada no vetor (Tempo de busca: %.6f ms).\n", elapsed_time);
    }
    printf("----------------------------------------\n");

    printf("2. Busca na Árvore de Busca Binária (ABB)\n");
    start_time = timer_start();
    found_info = search_bst(bst_root, normalized_term);
    elapsed_time = timer_stop(start_time);
    if (found_info) {
        printf("   Palavra encontrada! Frequência: %d (Tempo de busca: %.6f ms)\n", found_info->frequency, elapsed_time);
        display_citations(found_info->citations);
    } else {
        printf("   Palavra não encontrada na ABB (Tempo de busca: %.6f ms)\n", elapsed_time);
    }
    printf("----------------------------------------\n");

    printf("3. Busca na Árvore AVL\n");
    start_time = timer_start();
    found_info = search_avl(avl_root, normalized_term);
    elapsed_time = timer_stop(start_time);
    if (found_info) {
        printf("   Palavra encontrada! Frequência: %d (Tempo de busca: %.6f ms)\n", found_info->frequency, elapsed_time);
        display_citations(found_info->citations);
    } else {
        printf("   Palavra não encontrada na AVL (Tempo de busca: %.6f ms)\n", elapsed_time);
    }
    printf("----------------------------------------\n");

    free(normalized_term);
}

void handle_search_frequency() {
    int min_freq, max_freq;

    if (!freq_avl_root) {
        printf("Erro: Árvore AVL não construída ou vazia.\n");
        return;
    }

    printf("Entre com a frequência mínima: ");
    if (scanf("%d", &min_freq) != 1 || min_freq < 0) {
        printf("Frequência mínima inválida.\n");
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    printf("Entre com a frequência máxima: ");
    if (scanf("%d", &max_freq) != 1 || max_freq < min_freq) {
        printf("Frequência máxima inválida (deve ser >= frequência mínima).\n");
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    printf("\n--- Procurando por palavras com frequência entre %d e %d ---\n", min_freq, max_freq);
    printf("(Usando Árvore AVL organizada por frequência)\n");

    clock_t start_time = timer_start();
    search_freq_range_avl(freq_avl_root, min_freq, max_freq); // Realiza a busca
    double elapsed_time = timer_stop(start_time);

    printf("----------------------------------------\n");
    printf("Busca por intervalo de frequência concluída em %.6f ms.\n", elapsed_time);
}

void display_citations(CitationInfo *citations) {
    CitationInfo *current = citations;
    int count = 0;
    printf("   Citações:\n");
    while (current != NULL) {
        printf("    - Citação: \"%.50s...\"\n", current->quote);
        printf("      Filme: %s (%d)\n", current->movie, current->year);
        current = current->next;
        count++;
    }
    if (count == 0) {
        printf("    - (Não foram encontradas citações)\n");
    }
}

void cleanup_memory() {
    printf("\nLimpando memória alocada...\n");

    // Libera os nós das árvores primeiro (eles apenas apontam para dados)
    free_bst(bst_root);
    bst_root = NULL; // Evita liberação dupla se chamada novamente
    free_avl(avl_root);
    avl_root = NULL;
    free_freq_avl(freq_avl_root);
    freq_avl_root = NULL;

    // Libera o vetor e os WordInfo que ele possui
    free_vector(&word_vector);

    data_loaded = 0;
    printf("Memória limpa.\n");
}
