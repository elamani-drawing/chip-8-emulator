#ifndef CPU_H 
#define CPU_H 

#include <SDL2/SDL.h>
#include "./s_jump.h"
#include "./../headers/pixel.h"
#include "./../headers/s_emulator.h"
#define MEMORY_SIZE 4096
#define START_ADDRESS 512
#define MAX_JUMP 16//maximum de saut

#define OPCODE_NB 35//nombre dinstruction

struct s_cpu
{
    Uint8 memory[MEMORY_SIZE];
    Uint8 V[16];//le registre 
    Uint16 I;//stocke une adresse mémoire ou dessinateur 
    Uint16 jump[MAX_JUMP];//pour gérer les sauts dans « mémoire », 16 au maximum 
    Uint8 jump_nb;//stocke le nombre de sauts effectués pour ne pas dépasser 16 
    Uint8 sys_counter;//compteur pour la synchronisation 
    Uint8 sound_counter;//compteur pour le son 
    Uint16 pc; //pour parcourir le tableau « mémoire » 
    struct s_jump jump_table;  //id et mask de lopcode
};

//methodes
//cpu
void initialize_cpu(struct s_cpu *cpu) ; 
void count(struct s_cpu *cpu) ; 


#endif