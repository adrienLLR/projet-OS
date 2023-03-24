#include <process_handling.h>
#include <time_handling.h>
#include <tinyalloc.h>
#include <string.h>
#include <cpu.h>

#define MAX_NUMBER_OF_PROCESS 100

extern void ctx_sw(int32_t *ptr1, int32_t *ptr2);

static table_process *table_of_process; // Contient les processus du système
static slumbered_process **table_of_slumbered_process; // Contient les processus endormis du système
static dying_process **table_of_dying_process; // Contient les processus mourant
process *enabled_process = NULL; // Pointe sur le processus actif
int32_t pid_numbers[MAX_NUMBER_OF_PROCESS]; // Contient les pid disponible et indisponible

/*
 * Au lancement, fait les initialisations nécessaires pour que la gestion des processus se fasse.
 */
void init_kernel_process() {
    init_table_of_process();
    init_slumbered_table_of_process();
    init_dying_table_of_process();
    initialize_pid_numbers();
    create_process((void*) idle, "idle");
    init_enabled_process();
}

/* 
 * Init the process table 
 */
void init_table_of_process() {
    table_of_process = malloc(sizeof(table_process));
    table_of_process->head_process = NULL;
    table_of_process->tail_process = NULL;
}

/*
 * Init the slumbered process table
 */
void init_slumbered_table_of_process() {
    table_of_slumbered_process = malloc(sizeof(slumbered_process*));
}

/*
 * At the start of the kernel, point the enable process pointer on the idle programm which is always present
 */
void init_enabled_process() {
    process *process = get_head_process();
    while (process != NULL) {
        if (strcmp(process->name, "idle") == 0) {
            enabled_process = process;
            enabled_process->process_state = ENABLED;
            return;
        }
        process = process->next;
    }
    return;
}

/*
 * Init the dying process table
 */
void init_dying_table_of_process() {
    table_of_dying_process = malloc(sizeof(dying_process*));
}

/*
 * Return a pointer towards the process at the head of the process table.
 */
process* get_head_process() {
    return table_of_process->head_process;
}

slumbered_process* get_head_slumbered_process() {
    return *table_of_slumbered_process;
}

/*
 * Return a pointer towards the process at the head of the process table.
 */
process* get_tail_process() {
    return table_of_process->tail_process;
}


/*
 * Return the process of a known pid, NULL if it is not present.
 */
process* get_process_with_pid(int32_t pid) {
    process *process = get_head_process();
    while (process != NULL) {
        if (process->pid == pid) {
            return process;
        }
        process = process->next;
    }
    return NULL;
}

/*
 * Return the process i
 */
process* get_process_i(int32_t i) {
    process* process = get_head_process();
    for (int k = 1; k < i; k++) {
        process = process->next;
    }
    return process;
}

/*
 * Initialize the pid numbers of the array
 */
void initialize_pid_numbers() {
    for (int32_t i = 0; i < MAX_NUMBER_OF_PROCESS; i++) {
        pid_numbers[i] = i;
    }
}

/* 
 * Return the first pid number available, -1 if there is none
 */ 
int32_t search_first_pid_number_available() {
    for (int32_t i = 0; i < MAX_NUMBER_OF_PROCESS; i++) {
        if (pid_numbers[i] == i) {
            pid_numbers[i] = -1; // on va le donner donc il devient inutilisable
            return i; // si on a l'égalité alors le pid number n'est pas utilisé
        }
    }
    return -1; // aucun pid number n'est disponible
}

/*
 * Append process to the linked list.
 */
void insert_process(process *process_to_insert) {
    // Enlever son next permet d'éviter de potentiels problèmes
    process_to_insert->next = NULL;
    if (table_of_process->tail_process == NULL && table_of_process->head_process == NULL) {
        table_of_process->head_process = process_to_insert;
        table_of_process->tail_process = process_to_insert;
    }
    else {
        table_of_process->tail_process->next = process_to_insert;
        table_of_process->tail_process = process_to_insert;
    }
}

/*
 * Insert a slumbered process according to its wake up time.
 */
void insert_slumbered_process(process *process_to_insert, int32_t wake_up_time) {
    process_to_insert->process_state = SLUMBERED;
    slumbered_process *process_asleep = create_slumbered_process(process_to_insert, wake_up_time);

    // Si il n'y a aucun processus endormis dans la table
    if (*table_of_slumbered_process == NULL) { 
        *table_of_slumbered_process = process_asleep;
    }
    else {
        slumbered_process *head_process = *table_of_slumbered_process;
        // Si la tête de liste a une date de réveil plus grande que le process à insérer
        if (head_process->wake_up_date >= wake_up_time) {
            *table_of_slumbered_process = process_asleep;
            process_asleep->next = head_process;
        }
        else {
            slumbered_process *current_process = head_process;
            // Tant que le prochain processus endormis est non nul et qu'il a une date de réveil inférieure à wake up time
            while(current_process->next != NULL && current_process->next->wake_up_date <= wake_up_time) {
                current_process = current_process->next;
            }
            process_asleep->next = current_process->next;
            current_process->next = process_asleep;
        }
    }
}

/*
 * Put a killed process in the dying linked list.
 */
void insert_dying_process(process *killed_process) {
    dying_process *gonna_die_process = create_dying_process(killed_process);
    gonna_die_process->next = *table_of_dying_process;
    *table_of_dying_process = gonna_die_process;
}

/*
 * Pop the head process. Return NULL if linked list is empty.
 */
process* pop_process() {
    process* head_process = get_head_process();
    if (head_process == NULL) {
        return NULL;
    }
    if (head_process == get_tail_process()) {
        table_of_process->head_process = NULL;
        table_of_process->tail_process = NULL;
        return head_process;
    }
    table_of_process->head_process = head_process->next;
    return head_process;
}

/* 
 * Pop the head of the slumbered process linked list. Return NULL if linked list is empty.
 */
slumbered_process* pop_slumbered_process() {
    if (*table_of_slumbered_process == NULL) {
        return NULL;
    }
    slumbered_process *process_asleep = *table_of_slumbered_process;
    *table_of_slumbered_process = process_asleep->next;
    return process_asleep;
}

/*
 * Pop the head of the dying process linked list. Return NULL if linked list is empty.
 */
dying_process* pop_dying_process() {
    if (*table_of_dying_process == NULL) {
        return NULL;
    }
    dying_process* head_killed_process = *table_of_dying_process;
    *table_of_dying_process = head_killed_process->next;
    return head_killed_process;
}

/*
 * Create a process and insert it in the process table.
 * Return -1 if there was an error.
*/
int32_t create_process(void *function, char *s) {
    process *new_process = malloc(sizeof(process)); 
    if (new_process == NULL) {
        printf("Impossible de créer un nouveau processus : plus de mémoire\n");
        return -1;
    }
    int32_t pid = search_first_pid_number_available();
    if (pid == -1) {
        printf("Impossible de créer un nouveau processus : plus de pid\n");
        return -1;
    }
    new_process->pid = pid;
    pid_numbers[new_process->pid] = -2; // ce pid number est utilisé maintenant donc on enlève la valeur du tableau pid_numbers
    strcpy(new_process->name, s);
    new_process->process_state = AWAITING;
    new_process->registers[1] = (int32_t)(&new_process->stack[511]); //Stack pointer, Pointer to the top of the stack.
    new_process->stack[511] = (int32_t) function; // On met l'adresse de la fonction définissant le programme au sommet de la pile
    new_process->next = NULL;
    insert_process(new_process);
    return 0;
}

/*
 * Create a struct slumbered process
 */
slumbered_process* create_slumbered_process(process *process, int32_t wake_up_time) {
    slumbered_process *process_asleep = malloc(sizeof(slumbered_process));
    process_asleep->process_slumbered = process;
    process_asleep->wake_up_date = wake_up_time;
    process_asleep->next = NULL;
    return process_asleep;
}

/*
 * Create a struct dying process.
 */
dying_process* create_dying_process(process *process) {
    dying_process *proc = malloc(sizeof(dying_process));
    proc->killed_process = process;
    proc->next = NULL;
    return proc;
}

/*
 * Wake up all the process that have to be awaken.
 */
void wake_up_asleep_process() {
    int32_t time_in_seconds = get_time_in_seconds();
    slumbered_process *process_asleep = get_head_slumbered_process();
    while (process_asleep != NULL && process_asleep->wake_up_date <= time_in_seconds) {
        insert_process(pop_slumbered_process()->process_slumbered);
        process_asleep = get_head_slumbered_process();
    }
}

/*
 * Free the process that have been ended.
 */
void free_the_killed_process() {
    dying_process* current_dying_process = pop_dying_process();
    dying_process* next_dying_process;
    if (current_dying_process != NULL) next_dying_process = current_dying_process->next;
    while (current_dying_process != NULL) {
        process* process_killed = current_dying_process->killed_process;
        free(process_killed);
        free(current_dying_process);
        current_dying_process = next_dying_process;
        if (current_dying_process != NULL) next_dying_process = current_dying_process->next;
    }
}

/* 
 * Retourne le nom du processus
*/
char* mon_nom() {
    return enabled_process->name;
}

int32_t mon_pid() {
    return enabled_process->pid;
}

/*
 * Turnstile algorithm which make the process run in a circular order (0->1->2->3->....->0->1...)
*/
void tourniquet() {
    process* old_process = pop_process();
    if (enabled_process->process_state == ENABLED) {
        insert_process(old_process);
        old_process->process_state = AWAITING;
    }

    if (enabled_process->process_state == DYING) {
        insert_dying_process(enabled_process);
    }

    process* new_process = get_head_process();
    new_process->process_state = ENABLED;

    enabled_process = new_process;

    ctx_sw(old_process->registers, new_process->registers);
}

/*
 * Use the turnstile algorithm for the scheduler
 */
void ordonnance() {
    wake_up_asleep_process();
    free_the_killed_process();
    tourniquet();
}

/*
 * Allow a process to sleep for a given time (in seconds)
 */
void sleep(uint32_t nbr_secs) {
    insert_slumbered_process(enabled_process, get_time_in_seconds() + nbr_secs);
    ordonnance();
}

/*
 * Kill a process by inserting him to the dying process table and switching its state to DYING.
 */
void end_process() {
    enabled_process->process_state = DYING;
    ordonnance();
}

//----------- Sixieme test
void proc1(void)
{
    for (int32_t i = 0; i < 2; i++) {
        printf("[temps = %u] processus %s pid = %i\n", get_time_in_seconds(), mon_nom(), mon_pid());
        sleep(2);
    }
    end_process();
}

void idle() {
    for (;;) {
        sti();
        hlt();
        cli();
    }
}

//------------ Cinquieme test  
// void idle() {
//     for (;;) {
//         sti();
//         hlt();
//         cli();
//     }
// }

// void proc1(void) {
//     for (;;) {
//         printf("[temps = %u] processus %s pid = %i\n", get_time_in_seconds(), mon_nom(), mon_pid());
//         sleep(2);
//     }
// }

// void proc2(void) {
//     for (;;) {
//         printf("[temps = %u] processus %s pid = %i\n", get_time_in_seconds(), mon_nom(), mon_pid());
//         sleep(3);
//     }
// }

// void proc3(void) {
//     for (;;) {
//         printf("[temps = %u] processus %s pid = %i\n", get_time_in_seconds(), mon_nom(), mon_pid());
//         sleep(5);
//     }
// }

//--------------------------

//------------ Quatrieme test   Pourquoi on doit appeler sti et cli dans les fonctions et pas dans le start.c ?
// void idle(void) {
//     for (;;) {
//         printf("[%s] pid = %i\n", mon_nom(), mon_pid());
//         sti();
//         hlt();
//         cli();
//     }
// }


// void proc(void) {
//     for (;;) {
//         printf("[%s] pid = %i\n", mon_nom(), mon_pid());
//         sti();
//         hlt();
//         cli();
//     }
// }
//--------------------------

//------------ Troisieme test
// void idle(void) {
//     for (;;) {
//         printf("[%s] pid = %i\n", mon_nom(), mon_pid());
//         ordonnance();
//     }
// }


// void proc(void) {
//     for (;;) {
//         printf("[%s] pid = %i\n", mon_nom(), mon_pid());
//         ordonnance();
//     }
// }
//--------------------------

//-----------Deuxieme test

// void idle(void)
// {
//     for (int i = 0; i < 3; i++) {
//         printf("[idle] je tente de passer la main a proc1...\n");
//         ctx_sw(get_process(0)->registers, get_process(1)->registers);
//         printf("[idle] proc1 m’a redonne la main\n");
//     }
//     printf("[idle] je bloque le systeme\n");
//     hlt();
// }

// void proc1(void) {
//     for (;;) {
//         printf("[proc1] idle m’a donne la main\n");
//         printf("[proc1] je tente de lui la redonner...\n");
//         ctx_sw(get_process(1)->registers, get_process(0)->registers);
//     }
// }

//-----------------------

// --------- Premier test
// void idle(void) {
//     printf("[idle] je tente de passer la main a proc1...\n");
//     ctx_sw(get_process(0)->registers, get_process(1)->registers);
// }
// void proc1(void) {
//     printf("[proc1] idle m’a donne la main\n");
//     printf("[proc1] j’arrete le systeme\n");
//     hlt();
// }
// --------------------
