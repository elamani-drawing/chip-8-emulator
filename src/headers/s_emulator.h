#ifndef S_EMULATOR_h 
#define S_EMULATOR_h 

#include <SDL2/SDL.h>
#include "./../modules/cpu.c"
#include "./../modules/pixel.c"
#include "./../modules/s_input.c"

//represente l'emulateur
struct s_emulator
{
    //possede
    struct s_cpu cpu; //un cpu
    struct s_screen screen; //un ecran
    struct s_input in;//des evennements
    //Mix_Music *sound; //le son
};

//methodes
int load_rom(struct s_cpu *cpu, const char path[]);
int initialize_SDL(void);
void destroy_SDL(void);
int initialize_emulator(struct s_emulator *emulator);
void destroy_emulator(struct s_emulator *emulator);
void resize_screen(struct s_screen *screen);
void emulate(struct s_emulator *emulator);

Uint16 get_opcode(struct s_cpu *cpu);
Uint8 get_action(struct s_jump *table, Uint16 opcode);
void interpret(struct s_emulator *emulator);


void opcode_DXYN(struct s_emulator *emulator, struct s_screen *screen, Uint8 b1, Uint8 b2, Uint8 b3);
void opcode_1NNN(struct s_emulator *emulator, Uint8 b1, Uint8 b2, Uint8 b3);
void opcode_2NNN(struct s_emulator *emulator, Uint8 b1, Uint8 b2, Uint8 b3);
void opcode_00EE(struct s_emulator *emulator, Uint8 b1, Uint8 b2, Uint8 b3);
void opcode_3XNN(struct s_emulator *emulator, Uint8 b1, Uint8 b2, Uint8 b3);
void opcode_8XY4(struct s_emulator *emulator, Uint8 b1, Uint8 b2, Uint8 b3);
void opcode_8XY7(struct s_emulator *emulator, Uint8 b1, Uint8 b2, Uint8 b3);
void opcode_CXNN(struct s_emulator *emulator, Uint8 b1, Uint8 b2, Uint8 b3);
void opcode_FX33(struct s_emulator *emulator, Uint8 b1, Uint8 b2, Uint8 b3);
void opcode_00E0(struct s_emulator *emulator, Uint8 b1, Uint8 b2, Uint8 b3);
void opcode_FX29(struct s_emulator *emulator, Uint8 b1, Uint8 b2, Uint8 b3);


#endif