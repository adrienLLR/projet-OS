#include <cpu.h>
#include <inttypes.h>
#include <ecran.h>
#include <time_handling.h>
#include <process_handling.h>
#include <segment.h>
#include <stdbool.h>

extern void traitant_IT_32();

void init_traitant_IT(int32_t num_IT, void (*traitant)(void)) {
    uint32_t adresse_traitant = (uint32_t) (traitant);
    uint32_t premier_mot = (((uint32_t) KERNEL_CS ) << 16) | (adresse_traitant & 0x0000FFFF);
    uint32_t deuxieme_mot = (adresse_traitant & 0xFFFF0000) | 0x00008E00;
    *( (uint32_t*) (0x1000 + (num_IT) * 8) ) = premier_mot;
    *( (uint32_t*) (0x1000 + (num_IT) * 8 + 4) ) = deuxieme_mot;
}

void masque_IRQ(uint32_t num_IRQ, bool masque) {
    uint8_t mask = inb(0x21);
    if (masque == true) mask = (((mask >> num_IRQ) | 0x01) << num_IRQ) | mask;
    if (masque == false) mask = (((mask >> num_IRQ) & 0xFE) << num_IRQ) | ((mask << (8 - num_IRQ)) >> num_IRQ);
    outb((uint8_t) mask, 0x21);
}

void kernel_start(void)
{
    // Initialisation des traitants d'interruptions
    init_traitant_IT(32, traitant_IT_32);

    // Initialisation du timer
    efface_ecran();
    uint32_t frequency = get_frequency();
    set_frequency_of_timer(frequency);
    initialize_timer();
    char *time = get_time();
    print_string_len_lower_79_top_right_corner(time);

    // Initialisation des processus
    init_kernel_process();

    //--------- Pour tester si l'ordonnanceur marche pour N processus ---------------------------------
    // int32_t N = 8;
    // char name[100] = "proc"; 
    // for (int i = 0; i < N; i++) {
    //     name[4] = i + '1';
    //     create_process((void*) proc, name);
    // }
    //--------------------------------------------------------------------------------------------------

    create_process((void*) proc1, "proc1");
    // create_process((void*) proc2, "proc2");
    // create_process((void*) proc3, "proc3");

    // on démasque l'horloge
    masque_IRQ(0, false);
    idle();

}



