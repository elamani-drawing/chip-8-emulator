#include "./../headers/s_emulator.h"

#define VITESSECPU 4 //nombre d'opérations par tour 
#define FPS 16      //pour le rafraîchissement 

/**
 * Charge une rom
 * @param cpu 
 * @param path le chemin bers la rom
 * @return int 
 */
int load_rom(struct s_cpu *cpu, const char path[])
{
    FILE *rom = fopen(path, "rb");
    if(!rom)
    {
        perror("Error fopen:");
        return -1;
    }
    //copy de la rom dans l'espace memoire de la chip
    fread(&cpu->memory[START_ADDRESS], sizeof(Uint8) * (MEMORY_SIZE - START_ADDRESS), 1, rom);
    fclose(rom);
    return 0;
}

/**
 * @brief Initialise la SDL
 * 
 * @return int 
 */
int initialize_SDL(void)
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "Error SDL_Init: %s.\n", SDL_GetError());
        return -1;
    }
    return 0;
}

/**
 * @brief Quitte proprement la SDL
 * 
 */
void destroy_SDL(void)
{
    SDL_Quit();
}

/**
 * @brief Initialise l'émulateur
 * 
 * @param emulator la structure s_emulator à initialisé
 * @return int 
 */
int initialize_emulator(struct s_emulator *emulator)
{
    int status = -1;
    initialize_cpu(&emulator->cpu);
    memset(&emulator->in, 0 ,sizeof(struct s_input));
    if(0 == initialize_SDL())
    {
        status = initialize_screen(&emulator->screen);
        
        if(0 > status)
            destroy_SDL();        
    }
    return status;
}

/**
 * @brief Détuit l'émulateur en détruisant lécran et le SDL
 * 
 * @param emulator 
 */
void destroy_emulator(struct s_emulator *emulator)
{
    destroy_screen(&emulator->screen);
    destroy_SDL();
}

void resize_screen(struct s_screen *screen)
{
    int w, h;
    SDL_GetWindowSize(screen->w, &w, &h);
    screen->pixel_height = h / PIXEL_BY_HEIGHT;
    screen->pixel_width = w / PIXEL_BY_WIDTH;
}
/**
 * @brief Lance l'émulation
 * 
 * @param emulator 
 */
void emulate(struct s_emulator *emulator)
{
    //set_pixels(&emulator->screen);
    while(!emulator->in.quit)
    {
        update_event(&emulator->in);
        /*if(cpu->sound_counter != 0)
        {
            Mix_PlayMusic(sound, 1);
            cpu->sound_counter = 0;
        }*/      
        //outes les seize millisecondes (nous allons mettre ce 16 dans une constante FPS), il nous faudra faire quatre opérations et afficher une image.
        update_screen(&emulator->screen);
        SDL_Delay(FPS);
        //if(emulator->in.resize)
            //resize_screen(&emulator->screen);  
    }
}




/**
 * @brief reccupere l'op code et lenvoie dans la memoire
 * 
 * @return Uint16 
 */
Uint16 get_opcode(struct s_cpu *cpu)   //dans cpu.c 
{  
    return (cpu->memory[cpu->pc] << 8) + cpu->memory[cpu->pc + 1];
}

/**
 * @brief teste tous les opcodes jusqu’à trouver le bon
 * 
 * @param table 
 * @param opcode 
 * @return Uint8 
 */
Uint8 get_action(struct s_jump *table, Uint16 opcode)
{
    for(size_t i = 0; i < OPCODE_NB; i++)
    {
        if((table->mask[i] & opcode) == table->id[i])  /* On a trouvé l'action à effectuer */ 
            //on renvoie l'indice de l'action à effectuer 
            return i;/* Plus la peine de continuer la boucle car la condition n'est vraie qu'une seule fois*/ 
    }
    fprintf(stderr, "Bad action, unknwown opcode %d.\n", opcode);
    return 0;
}

/**
 * @brief Reccupere la valeur d'instruction à éffectuer et execute laction
 * 
 * @param emulator 
 */
void interpret(struct s_emulator *emulator)
{
    Uint16 opcode = get_opcode(&emulator->cpu);
    Uint8 b3,b2,b1;
    b3 = (opcode & (0x0F00)) >> 8;  /* les 4 bits de poids fort, b3 représente X */
    b2 = (opcode & (0x00F0)) >> 4;  /* idem, b2 représente Y */
    b1 = (opcode & (0x000F));       /* les 4 bits de poids faible */
    Uint8 action = get_action(&emulator->cpu.jump_table, opcode);
    switch(action)
    {
        case 0:{ 
               //Cet opcode n'est pas implémenté. 
                break; 
              } 
     case 1:{ 
                //00E0 efface l'écran. 
                opcode_00E0(emulator, b1, b2, b3);

                break; 

               } 

     case 2:{//00EE revient du saut. 

               opcode_00EE(emulator, b1,b2, b3);

                break; 
            } 
    case 3:{ //1NNN effectue un saut à l'adresse 1NNN. 

                opcode_1NNN(emulator, b1, b2, b3);

                break; 
            } 
    case 4:{ 
            //2NNN appelle le sous-programme en NNN, mais on revient ensuite. 
                opcode_2NNN(emulator,b1,b2, b3);
                
                break; 
            } 
    case 5:{//3XNN saute l'instruction suivante si VX est égal à NN. 
                opcode_3XNN(emulator, b1, b2, b3);

                break; 
            } 
    case 6:{//4XNN saute l'instruction suivante si VX et NN ne sont pas égaux. 
                
                if(emulator->cpu.V[b3]!=((b2<<4)+b1)) 
                { 
                    emulator->cpu.pc+=2; 
                } 

                break; 
            } 
    case 7:{ 
           //5XY0 saute l'instruction suivante si VX et VY sont égaux. 
                if(emulator->cpu.V[b3]==emulator->cpu.V[b2]) 
                { 
                    emulator->cpu.pc+=2; 
                } 
                break; 
            } 

    case 8:{ 
            //6XNN définit VX à NN. 
                emulator->cpu.V[b3]=(b2<<4)+b1; 
                break; 
            } 
    case 9:{ 
                //7XNN ajoute NN à VX. 
                emulator->cpu.V[b3]+=(b2<<4)+b1; 

                break; 
            } 
    case 10:{ 
                //8XY0 définit VX à la valeur de VY. 
                emulator->cpu.V[b3]=emulator->cpu.V[b2]; 

                break; 
            } 
    case 11:{ 
                //8XY1 définit VX à VX OR VY. 
                emulator->cpu.V[b3]=emulator->cpu.V[b3]|emulator->cpu.V[b2]; 
                break; 
            } 
    case 12:{ 
                //8XY2 définit VX à VX AND VY. 
                emulator->cpu.V[b3]=emulator->cpu.V[b3]&emulator->cpu.V[b2]; 
                break; 
            } 
    case 13:{ 
                //8XY3 définit VX à VX XOR VY. 
                emulator->cpu.V[b3]=emulator->cpu.V[b3]^emulator->cpu.V[b2]; 
                
                break; 
            } 
    case 14:{ 
                //8XY4 ajoute VY à VX. VF est mis à 1 quand il y a un dépassement de mémoire (carry), et à 0 quand il n'y en pas. 
                if((emulator->cpu.V[b3]+emulator->cpu.V[b2])>255) 
                { 
                    emulator->cpu.V[0xF]=1; //cpu.V[15] 
                } 
                else 
                { 
                    emulator->cpu.V[0xF]=0; //cpu.V[15] 
                } 
                emulator->cpu.V[b3]+=emulator->cpu.V[b2];

                break; 
            } 
    case 15:{ 
                //8XY5 VY est soustraite de VX. VF est mis à 0 quand il y a un emprunt, et à 1 quand il n'y a en pas. 
                 if((emulator->cpu.V[b3]<emulator->cpu.V[b2])) 
                { 
                    emulator->cpu.V[0xF]=0; //cpu.V[15] 
                } 
                else 
                { 
                    emulator->cpu.V[0xF]=1; //cpu.V[15] 
                } 
                emulator->cpu.V[b3]-=emulator->cpu.V[b2]; 


                break; 
            } 
    case 16:{ 

                //8XY6 décale (shift) VX à droite de 1 bit. VF est fixé à la valeur du bit de poids faible de VX avant le décalage. 
                emulator->cpu.V[0xF]=( emulator->cpu.V[b3]&(0x01)); 
                emulator->cpu.V[b3]=( emulator->cpu.V[b3]>>1); 

                break; 
            } 
    case 17:{ 
                //8XY7 VX = VY - VX. VF est mis à 0 quand il y a un emprunt et à 1 quand il n'y en a pas. 
                opcode_8XY7(emulator, b1, b2, b3);

                break; 
            } 
    case 18:{ 
                //8XYE décale (shift) VX à gauche de 1 bit. VF est fixé à la valeur du bit de poids fort de VX avant le décalage. 
                emulator->cpu.V[0xF]=(emulator->cpu.V[b3]>>7); 
                emulator->cpu.V[b3]=(emulator->cpu.V[b3]<<1); 

                break; 
             } 

    case 19:{ 

                //9XY0 saute l'instruction suivante si VX et VY ne sont pas égaux. 
                if(emulator->cpu.V[b3]!=emulator->cpu.V[b2]) 
                    { 
                        emulator->cpu.pc+=2; 
                    } 
                break; 
            } 
    case 20:{ 
            //ANNN affecte NNN à I. 

                emulator->cpu.I=(b3<<8)+(b2<<4)+b1; 

                break; 
            } 
    case 21:{ 
            //BNNN passe à l'adresse NNN + V0. 

            emulator->cpu.pc=(b3<<8)+(b2<<4)+b1+emulator->cpu.V[0]; 
            emulator->cpu.pc-=2; 

            break; 

            } 
    case 22:{ 

            //CXNN définit VX à un nombre aléatoire inférieur à NN. 
            emulator->cpu.V[b3]=(rand())%((b2<<4)+b1+1); 


            break; 

            } 

    case 23:{ 
            //DXYN dessine un sprite aux coordonnées (VX, VY). 

            //dessinerEcran(b1,b2,b3); 
            opcode_DXYN(emulator, &emulator->screen, b1, b2, b3);
            break; 

            } 
    case 24:{ 
                //EX9E saute l'instruction suivante si la clé stockée dans VX est pressée. 
               

                break; 
            } 
    case 25:{ 
            //EXA1 saute l'instruction suivante si la clé stockée dans VX n'est pas pressée. 
               
                break; 
            } 

    case 26:{ 
                //FX07 définit VX à la valeur de la temporisation. 
                emulator->cpu.V[b3]=emulator->cpu.sys_counter; 

                break; 
            } 
    case 27:{ 
                //FX0A attend l'appui sur une touche et la stocke ensuite dans VX. 
               

                break; 
            } 


    case 28:{ 
                //FX15 définit la temporisation à VX. 
                emulator->cpu.sys_counter=emulator->cpu.V[b3]; 

                break; 
            } 
    case 29:{ 
                //FX18 définit la minuterie sonore à VX. 
                

                break; 
            } 
    case 30:{ 
            //FX1E ajoute à VX I. VF est mis à 1 quand il y a overflow (I+VX>0xFFF), et à 0 si tel n'est pas le cas. 

                if((emulator->cpu.I+emulator->cpu.V[b3])>0xFFF) 
                { 
                    emulator->cpu.V[0xF]=1; 
                } 
                else 
                { 
                    emulator->cpu.V[0xF]=0; 
                } 
                emulator->cpu.I+=emulator->cpu.V[b3]; 

                break; 
            } 

    case 31:{ 
                //FX29 définit I à l'emplacement du caractère stocké dans VX. Les caractères 0-F (en hexadécimal) sont représentés par une police 4x5. 
                emulator->cpu.I=emulator->cpu.V[b3]*5; 
                break; 
            } 

    case 32:{ 
                //FX33 stocke dans la mémoire le code décimal représentant VX (dans I, I+1, I+2). 

                emulator->cpu.memory[emulator->cpu.I]=(emulator->cpu.V[b3]-emulator->cpu.V[b3]%100)/100; 
                emulator->cpu.memory[emulator->cpu.I+1]=(((emulator->cpu.V[b3]-emulator->cpu.V[b3]%10)/10)%10); 
                emulator->cpu.memory[emulator->cpu.I+2]=emulator->cpu.V[b3]-emulator->cpu.memory[emulator->cpu.I]*100-10*emulator->cpu.memory[emulator->cpu.I+1]; 

                break; 
            } 
    case 33:{ 

                //FX55 stocke V0 à VX en mémoire à partir de l'adresse I. 
                Uint8 i=0; 
                for(i=0;i<=b3;i++) 
                { 
                    emulator->cpu.memory[emulator->cpu.I+i]=emulator->cpu.V[i]; 
                } 


                break; 
            } 
    case 34:{ 
                //FX65 remplit V0 à VX avec les valeurs de la mémoire à partir de l'adresse I. 

               
                Uint8 i=0; 

                for(i=0;i<=b3;i++) 
                { 
                  emulator->cpu.V[i]=emulator->cpu.memory[emulator->cpu.I+i]; 
                } 

                break; 
            } 

    default: {//si ça arrive, il y un truc qui cloche 

                    break; 
             } 
    }
    //Pour passer à l’instruction suivante, on incrémente cpu->pc de 2 et pas de 1. En effet, l'opcode étant sur deux octets, l’instruction suivante est à l’adresse cpu->pc + 2.
    emulator->cpu.pc += 2;
}



/**
 * @brief 
 * 
 * @param emulator 
 * @param screen 
 * @param b1 
 * @param b2 
 * @param b3 
 */
void opcode_DXYN(struct s_emulator *emulator, struct s_screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
    struct s_cpu *cpu = &emulator->cpu;
    cpu->V[0xF] = 0;

    for(size_t i = 0; i < b1; i++)
    {
        Uint8 codage = cpu->memory[cpu->I + i];
        Uint8 y = cpu->V[b2] + i;

        for(size_t j = 0; j < 8; j++)
        {
            Uint8 x = cpu->V[b3] + j;
            /* Si le bit vaut 1 et que les coordonnées ne sont pas en dehors de l'écran. */
            if(y < PIXEL_BY_HEIGHT && x < PIXEL_BY_WIDTH && ((codage << j) & 0b10000000))
            {
                if((screen->pixels[x][y] == WHITE))
                    cpu->V[0xF] = 1;
                screen->pixels[x][y] = !screen->pixels[x][y]; /* Change la couleur du bit. */
            }
        }
    }
}





/**
 * @brief Effectue un saut à l’adresse NNN.
 * 
 * @param emulator 
 * @param b1 
 * @param b2 
 * @param b3 
 */
void opcode_1NNN(struct s_emulator *emulator, Uint8 b1, Uint8 b2, Uint8 b3)
{
    emulator->cpu.pc = (b3 << 8) + (b2 << 4) + b1 - 2;
}
/**
 * @brief Exécute le sous-programme à l’adresse NNN.
 * 
 * @param emulator 
 * @param b1 
 * @param b2 
 * @param b3 
 */
void opcode_2NNN(struct s_emulator *emulator, Uint8 b1, Uint8 b2, Uint8 b3)
{
    struct s_cpu *cpu = &emulator->cpu; 
    cpu->jump[cpu->jump_nb] = cpu->pc;
    if(cpu->jump_nb < MAX_JUMP)
        cpu->jump_nb++;

    cpu->pc = (b3 << 8) + (b2 << 4) + b1 - 2;
}

/**
 * @brief Retourne à partir d’un sous-programme.
 * 
 * @param emulator 
 * @param b1 
 * @param b2 
 * @param b3 
 */
void opcode_00EE(struct s_emulator *emulator, Uint8 b1, Uint8 b2, Uint8 b3)
{
    struct s_cpu *cpu = &emulator->cpu;
    if(cpu->jump_nb > 0)
    {
        cpu->jump_nb--;
        cpu->pc = cpu->jump[cpu->jump_nb];
    }
}

/**
 * @brief Saute l’instruction suivante si VX est égal à NN.
 * 
 * @param emulator 
 * @param b1 
 * @param b2 
 * @param b3 
 */
void opcode_3XNN(struct s_emulator *emulator, Uint8 b1, Uint8 b2, Uint8 b3)
{
    //Sauter une instruction revient à ajouter 2 au program counter.
    struct s_cpu *cpu = &emulator->cpu;
    if(cpu->V[b3] == (b2 << 4) + b1)
        cpu->pc += 2;
}

/**
 * @brief Ajoute VY à VX. VF est mis à 1 quand il y a un dépassement de mémoire (carry), et à 0 quand il n’y en pas.
 * 
 * @param emulator 
 * @param b1 
 * @param b2 
 * @param b3 
 */
void opcode_8XY4(struct s_emulator *emulator, Uint8 b1, Uint8 b2, Uint8 b3)
{
    struct s_cpu *cpu = &emulator->cpu;
    cpu->V[0xF] = cpu->V[b2] > 0xFF - cpu->V[b3];
    cpu->V[b3] += cpu->V[b2];
}

/**
 * @brief VX = VY - VX. VF est mis à 0 quand il y a un emprunt et à 1 quand il n’y en a pas.
 * 
 * @param emulator 
 * @param b1 
 * @param b2 
 * @param b3 
 */
void opcode_8XY7(struct s_emulator *emulator, Uint8 b1, Uint8 b2, Uint8 b3)
{
    struct s_cpu *cpu = &emulator->cpu;
    cpu->V[0xF] = cpu->V[b3] <= cpu->V[b2];
    cpu->V[b3] = cpu->V[b2] - cpu->V[b3];
}


double random_double(void)
{
    return rand() / (RAND_MAX + 1.);
}

int rand_int(int min, int max)
{
    return random_double() * (max - min + 1) + min;
}
/**
 * @brief Définit VX à NN AND R avec R un nombre tiré au hasard (entre 0 et 255).
 * 
 */
void opcode_CXNN(struct s_emulator *emulator, Uint8 b1, Uint8 b2, Uint8 b3)
{
    emulator->cpu.V[b3] = rand_int(0, 0xFF) & ((b2 << 4) + b1);
}

/**
 * @brief Stocke dans la mémoire le code décimal représentant VX (dans I, I+1, I+2).
 * 
 * @param emulator 
 * @param b1 
 * @param b2 
 * @param b3 
 */
void opcode_FX33(struct s_emulator *emulator, Uint8 b1, Uint8 b2, Uint8 b3)
{
    struct s_cpu *cpu = &emulator->cpu;
    Uint8 nb = cpu->V[b3];
    cpu->memory[cpu->I + 2] = nb % 10;
    cpu->memory[cpu->I + 1] = (nb/10) % 10;
    cpu->memory[cpu->I] = nb / 100;
}

/**
 * @brief Efface l’écran
 * 
 * @param emulator 
 * @param b1 
 * @param b2 
 * @param b3 
 */
void opcode_00E0(struct s_emulator *emulator, Uint8 b1, Uint8 b2, Uint8 b3)
{
    clear_screen(&emulator->screen);
}

/**
 * @brief Définit I à l’emplacement du caractère stocké dans VX. Les caractères 0-F (en hexadécimal) sont représentés par une police 4x5.
 * (dessin de charractere)
 * 
 * @param b1 
 * @param b2 
 * @param b3 
 */
void opcode_FX29(struct s_emulator *emulator, Uint8 b1, Uint8 b2, Uint8 b3)
{
    struct s_cpu *cpu = &emulator->cpu;
    cpu->I = 5 * cpu->V[b3];
}

