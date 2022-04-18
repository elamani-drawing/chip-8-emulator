#include "./../headers/cpu.h"

/**
 * On initialise le cpu 
 * @param cpu 
 */
void initialize_cpu(struct s_cpu *cpu)
{
    //on initialise l'espace memoire
    //Ici, toutes nos variables sont initialisés à zéro, sauf le program counter qui, rappelons-le, doit être initialisé à 0x200 que nous avons ici représenté sous la forme de la constante START_ADDRESS.
    memset(cpu, 0, sizeof(*cpu));
    cpu->pc = START_ADDRESS;
}

/**
 * la fonction qui décrémente nos compteurs et qui sera appelée toutes les 16 millisecondes. Nus décomptons jusqu’à ce que les compteurs soient nuls.
 * @param cpu 
 */
void count(struct s_cpu *cpu)
{
    if(cpu->sys_counter > 0)
        cpu->sys_counter--;
    if(cpu->sound_counter > 0)
        cpu->sound_counter--;
} 

