#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>

typedef struct struct_process {

    int32_t pid;
    char name[100];
    int32_t process_state;
    int32_t registers[5];
    int32_t stack[512];
    struct struct_process *next;

} process;

typedef struct struct_table_of_process {

    struct struct_process *head_process;
    struct struct_process *tail_process;

} table_process;

typedef struct struct_slumbered_process {

    struct struct_process *process_slumbered;
    int32_t wake_up_date;
    struct struct_slumbered_process *next;

} slumbered_process;

typedef struct struct_dying_process {

    struct struct_process *killed_process;
    struct struct_dying_process *next;

} dying_process;

typedef enum process_state {

    ENABLED,
    AWAITING,
    BLOCKED,
    SLUMBERED,
    DYING

} process_state;

typedef void (*fct)(void);

void init_kernel_process(void);

void init_table_of_process(void);

void initialize_pid_numbers(void);

void init_enabled_process(void);

void init_dying_table_of_process(void);

int32_t create_process(void *function, char *s);

int32_t search_first_pid_number_available(void);

void insert_process_in_first_available_slot(process *process_to_insert);

process* search_process_with_pid_number(int32_t pid_number);

void tourniquet(void);

process * get_process_with_pid(int32_t pid);

process * get_process_i(int32_t i);

void ordonnance(void);

slumbered_process* pop_slumbered_process(void);

dying_process* pop_dying_process(void);

process* pop_process(void);

void insert_slumbered_process(process *process_to_insert, int32_t wake_up_time);

void init_slumbered_table_of_process(void);

slumbered_process* create_slumbered_process(process *process, int32_t wake_up_time);

dying_process* create_dying_process(process *process);

void wake_up_asleep_process(void);

slumbered_process* get_head_slumbered_process(void); 

process* get_head_process(void);

process* get_tail_process(void);

void dors(uint32_t nbr_secs);

void idle(void);

void proc1(void);

// void proc2(void);

// void proc3(void);

void end_process(void);

void free_the_killed_process(void);
