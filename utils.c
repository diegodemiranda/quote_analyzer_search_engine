#include "utils.h"
#include <stdio.h> // For getchar

clock_t timer_start() {
  return clock();
}

double timer_stop(clock_t start_time) {
  clock_t end_time = clock();
  return ((double)(end_time - start_time) * 1000.0) / CLOCKS_PER_SEC; // Time in ms
}

void clear_input_buffer() {
  int c;
  while ((c = getchar()) != '\n' && c != EOF);
}
