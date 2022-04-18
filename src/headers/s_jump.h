#ifndef S_JUMP_H 
#define S_JUMP_H 

#include <SDL2/SDL.h>
#include "./../headers/s_emulator.h" 
#define OPCODE_NB 35//nombre dinstruction

/**
 * @brief une structure qui contient pour chaque opcode le masque et l’identifiant correspondant. 
 * 
 */
struct s_jump 
{
    Uint16 mask [OPCODE_NB];   //la Chip 8 peut effectuer 35 opérations, chaque opération possédant son masque 
    Uint16 id[OPCODE_NB];   //idem, chaque opération possède son propre identifiant 
}; 

//methodes
//jumps



#endif