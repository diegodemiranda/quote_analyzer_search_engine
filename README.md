# QUOTE ANALIZER AND SEARCH ENGINE

## Structure:   
---
    ├── main.c
    ├── structures.h
    ├── file_parser.h
    ├── file_parser.c
    ├── word_processing.h
    ├── word_processing.c
    ├── array_operations.h
    ├── array_operations.c
    ├── bst_operations.h
    ├── bst_operations.c
    ├── avl_operations.h
    ├── avl_operations.c
    ├── freq_avl_operations.h
    ├── freq_avl_operations.c
    ├── utils.h
    ├── utils.c
    └── movie_quotes.txt

## How to Compile and Run:
---
**Compile:**   
Open a terminal or command prompt in the project directory and compile using GCC (or another C compiler):
```gcc main.c file_parser.c word_processing.c array_operations.c bst_operations.c avl_operations.c freq_avl_operations.c utils.c -o quote_analyzer -lm```  

gcc: The compiler.   
List all your .c files.   
-o quote_analyzer: Specifies the output executable name.   
-lm: Links the math library (needed for max functions if they were more complex, though maybe not strictly necessary here, but good practice if math operations are involved).   
Run: Execute the compiled program.      

**Run:**   
Open a terminal or command prompt in the project directory and type:   
```./quote_analyzer```

Interact: Follow the menu options:

Choose 1 and enter movie_quotes.txt to load the data. Observe the loading times.
Choose 2 and enter a word (e.g., time, love, jedi, kansas) to search. Observe search times and results.
Choose 3 and enter a frequency range (e.g., min 5, max 10) to find words in that range.
Choose 0 to exit (memory cleanup should happen automatically).
