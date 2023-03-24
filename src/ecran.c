#include <stdio.h>
#include <inttypes.h>
#include <ecran.h>
#include <stdlib.h>
#include <cpu.h>
#include <string.h>

#define START_ADDRESS 0xB8000
#define COMMAND_PORT_GESTION_POS_CURSOR 0x3D4
#define DATA_PORT_GESTION_POS_CURSOR 0x3D5
#define NUMBER_COLUMN 80
#define NUMBER_LINE 25
#define FIRST_LINE 1
#define FIRST_COLUMN 0

static uint32_t lig_glob = FIRST_LINE;
static uint32_t col_glob = FIRST_COLUMN;

uint16_t *ptr_mem(uint32_t lig, uint32_t col) {
    return (uint16_t*) (START_ADDRESS + 2 * (lig * NUMBER_COLUMN + col));
}

void ecrit_car(uint32_t lig, uint32_t col, char c) {
    *(ptr_mem(lig, col)) = 0x0F00 | c;

}

void efface_ecran(void) {
    for ( uint16_t i = 0; i < 2000; i++ ) {
           *( (uint16_t*) (START_ADDRESS + 2 * i) ) = 0x0F00 | 0x0020;
        }
    place_curseur(0, 0);
}

void place_curseur(uint32_t lig, uint32_t col) {
    uint16_t pos = col + lig * NUMBER_COLUMN;
    uint16_t pos_bas =  pos & 0x00FF;
    uint16_t pos_haut = pos >> 8;

    outb(0x0F, COMMAND_PORT_GESTION_POS_CURSOR);
    outb( (uint8_t) pos_bas , DATA_PORT_GESTION_POS_CURSOR);
    outb(0x0E, COMMAND_PORT_GESTION_POS_CURSOR);
    outb( (uint8_t) pos_haut , DATA_PORT_GESTION_POS_CURSOR);
}

void traite_car(char c) {
    if ( 32 <= c && c <= 126 ) {
        ecrit_car(lig_glob, col_glob, c);
        col_glob++;
        if (col_glob == NUMBER_COLUMN) {
            col_glob = FIRST_COLUMN;
            lig_glob++;
        }
    }
    else if ( ( 0 <= c && c <= 31 ) || c == 127 ) {
        if (c == 8) {
            if (col_glob != FIRST_COLUMN) col_glob--;
        } 
        if (c == 9) col_glob = (col_glob + 8)%NUMBER_COLUMN;
        if (c == 10) {
            if (lig_glob < NUMBER_LINE) {
            lig_glob++;
            col_glob = FIRST_COLUMN;
            }
        }
        if (c==12) {
            efface_ecran();
            lig_glob = FIRST_LINE;
            col_glob = FIRST_COLUMN;
        }
        if (c==13) {
            col_glob = FIRST_COLUMN;
        }    
    }
    if (lig_glob == NUMBER_LINE) {
        defilement();
        lig_glob--;
    }
    place_curseur(lig_glob, col_glob);
}

void defilement(void) { 
    memmove( (void*) ptr_mem(1, 0) , (const void*) ptr_mem(2, 0) , (size_t) 2 * (NUMBER_LINE - 1) * NUMBER_COLUMN);
}

void console_putbytes(const char *s, int len) {
    for (int i = 0; i < len; i++) {
        traite_car(*(s + i));
    }
}

void print_string_len_lower_79_top_right_corner(const char *s) {
    int string_length = strlen(s);
    if (string_length > 79 ) return;
    uint32_t old_lig_glob = lig_glob;
    uint32_t old_col_glob = col_glob;
    lig_glob = 0;
    col_glob = 0 + ( NUMBER_COLUMN - 1 ) - string_length; 
    for (int i = 0; i < string_length; i++) {
        traite_car(*(s+i));
    }
    lig_glob = old_lig_glob;
    col_glob = old_col_glob;
    place_curseur(lig_glob, col_glob);
}