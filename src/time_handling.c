#include <time_handling.h>
#include <process_handling.h>

#define START_ADDRESS_IT_VECTOR 0x1000
#define COMMAND_PORT_IT_CONTROLER 0x20
#define DATA_PORT_IT_CONTROLER 0x21
#define COMMAND_PORT_CLOCK 0x43
#define DATA_PORT_CLOCK 0x40

static char time[9]; // on initialise le timer
static uint8_t frequency = 100;
static uint16_t count_until_get_one_sec = 0; // lorsque le compteur arrive à frequency cela veut dire que l'on a une seconde

char* get_time() {
    return time;
}

int32_t get_time_in_seconds() {
    int32_t heures = chose_to_get_hour_min_sec(0);
    int32_t minutes = chose_to_get_hour_min_sec(1);
    int32_t secondes = chose_to_get_hour_min_sec(2);
    return heures*60*60 + minutes*60 + secondes;
}

int get_frequency(void) {
    return frequency;
}

void initialize_timer(void) {
    sprintf(time, "00:00:00");
}

void set_frequency_of_timer(int frequency) {
   outb(0x34, COMMAND_PORT_CLOCK); 
   outb((0x1234DD / frequency) % 256, DATA_PORT_CLOCK);
   outb(( (uint16_t) (0x1234DD / frequency) ) >> 8, DATA_PORT_CLOCK );
}

void tic_PIT(void) {
    acquit();
    count_until_get_one_sec++; //on incrémente le compteur
    if (count_until_get_one_sec == frequency) { // si on a cette égalité alors il s'est écoulé une seconde
        increment_clock();
        count_until_get_one_sec = 0;
        print_string_len_lower_79_top_right_corner(time);
    }
    ordonnance();
    return;
}

void acquit(void) {
    outb(0x20, COMMAND_PORT_IT_CONTROLER); // on réalise l'acquittement pour le contrôleur d'interruption
}

int chose_to_get_hour_min_sec(int which) { 
    return (( (int) time[3*which] ) - (int) '0' )*10 + ((int) time[3*which + 1] - (int) '0'); // which = 0 retourne les heures, 1 retourne les minutes, 2 retourne les secondes
}

void increment_clock(void) {
    int secondes = chose_to_get_hour_min_sec(2);
    int minutes = chose_to_get_hour_min_sec(1);
    int heures = chose_to_get_hour_min_sec(0);
    secondes++;
    if (secondes == 60) {
        secondes = 0;
        minutes++;
        if (minutes == 60) {
            minutes = 0;
            heures++;
            if (heures == 24) {
                heures = 0;
            }
        }
    }
    update_timer_variable(heures, minutes, secondes);
}

void add_to_buffer_two_digits_at_min(char *buffer, int number, int strlen) { 
    if (number < 10) { // si il n'y a que un chiffre alors on ajoute un 0 devant
        sprintf(buffer + strlen, "0");
        sprintf(buffer + strlen + 1, "%i", number);
    }
    else {
        sprintf(buffer + strlen, "%i", number);
    }
}

void update_timer_variable(int heures, int minutes, int secondes) {
    add_to_buffer_two_digits_at_min(time, heures, 0);
    sprintf(time + 2, ":");
    add_to_buffer_two_digits_at_min(time, minutes, 3);
    sprintf(time + 5, ":");
    add_to_buffer_two_digits_at_min(time, secondes, 6);
}




