#ifndef UTILS_H
#define UTILS_H

#include <time.h>

// Gets the current high-resolution time
clock_t timer_start();

// Calculates the elapsed time in milliseconds
double timer_stop(clock_t start_time);

// Helper to clear input buffer
void clear_input_buffer();

#endif // UTILS_H