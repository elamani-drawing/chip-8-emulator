#ifndef PIXEL_H 
#define PIXEL_H

#include <SDL2/SDL.h>
//couleur a lecrant
#define BLACK SDL_FALSE
#define WHITE SDL_TRUE
//taille de lecran
#define PIXEL_BY_WIDTH 64
#define PIXEL_BY_HEIGHT 32
//dimension dun pixel
#define PIXEL_DIM 8
//dimension de l'ecran
#define WIDTH  PIXEL_BY_WIDTH*PIXEL_DIM 
#define HEIGHT PIXEL_BY_HEIGHT*PIXEL_DIM

typedef SDL_bool s_pixel;

/**
 * L’écran est une fenêtre et une matrice de pixels.
 * 
 */
struct s_screen
{
    SDL_Window *w;//fenetre
    SDL_Renderer *r;//ecran
    s_pixel pixels[PIXEL_BY_WIDTH][PIXEL_BY_HEIGHT];//tableau de pixel
    //hauter et largeur de pixel
    Uint32 pixel_height;
    Uint32 pixel_width;
};

//methodes

void clear_screen(struct s_screen *screen);
int initialize_screen(struct s_screen *screen);
void destroy_screen(struct s_screen *screen);
void update_screen(struct s_screen *screen);
void set_pixels(struct s_screen *screen);

#endif