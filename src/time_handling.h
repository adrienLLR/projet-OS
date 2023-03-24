#ifndef time_handling_h
#define time_handling_h

#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <cpu.h>
#include <string.h>
#include <ecran.h>
#include <segment.h>

void tic_PIT(void);

char * get_time(void);

int32_t get_time_in_seconds(void);

int chose_to_get_hour_min_sec(int which);

void acquit(void);

void increment_clock(void);

void add_to_buffer_two_digits_at_min(char *s, int number, int strlen);

void update_timer_variable(int heures, int minutes, int secondes);

void initialize_timer(void);

int get_frequency(void);

void set_frequency_of_timer(int frequency);

#endif 