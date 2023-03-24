#ifndef __START_H__
#define __START_H__

#define FIRST_STACK_SIZE 16384

#ifndef ASSEMBLER

extern char first_stack[FIRST_STACK_SIZE];

/* The kernel entry point */
void kernel_start(void);

void init_traitant_IT(int32_t num_IT, void (*traitant)(void));

void masque_IRQ(uint32_t num_IRQ, bool masque);

#endif

#endif
