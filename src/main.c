#include "./modules/s_emulator.c"

int main(int argc, char *argv[])
{
    int status = -1;
    struct s_emulator emulator;
    //srand(time(NULL));
    if(0 == initialize_emulator(&emulator))
    {
        if(0 == load_rom(&emulator.cpu, "./roms/Maze.ch8"))
        {
            status = 0;
            emulate(&emulator);
        }
        destroy_emulator(&emulator);
    }
    return status;
}