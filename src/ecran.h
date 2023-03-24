#ifndef ecran_h
#define ecran_h

#include <stdlib.h>
#include <stdio.h>

uint16_t *ptr_mem(uint32_t lig, uint32_t col);

void ecrit_car(uint32_t lig, uint32_t col, char c);

void efface_ecran(void);

void place_curseur(uint32_t lig, uint32_t col);

void traite_car(char c);

void defilement(void);

void console_putbytes(const char *s, int len);

void print_string_len_lower_79_top_right_corner(const char *s);



#endif 