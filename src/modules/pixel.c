#include "./../headers/pixel.h"

/**
 * @brief Set the pixels object
 * 
 * @param screen 
 */
void set_pixels(struct s_screen *screen)
{
    for(size_t i = 0; i < PIXEL_BY_WIDTH; i++)
        for(size_t j = 0; j < PIXEL_BY_HEIGHT; j++)
            screen->pixels[i][j] = (i % (j + 1)) != 0;
}

/**
 * @brief Efface l'écran (met tout les pixels en noir)
 * 
 * @param screen 
 */
void clear_screen(struct s_screen *screen)
{
    memset(screen->pixels, BLACK, sizeof(screen->pixels));
}
/**
 * Initialise l'ecran (initialise le window et le render)
 * 
 * @param screen 
 * @return int 
 */
int initialize_screen(struct s_screen *screen)
{
    screen->w = SDL_CreateWindow("Emulateur Chip-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                WIDTH, HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if(NULL == screen->w)
    {
        fprintf(stderr, "Error SDL_CreateWindow: %s.\n", SDL_GetError());
        return -1;
    }
    screen->r = SDL_CreateRenderer(screen->w, -1, SDL_RENDERER_ACCELERATED);
    if(NULL == screen->r)
    {
        fprintf(stderr, "Error SDL_CreateRenderer: %s.\n", SDL_GetError());
        SDL_DestroyWindow(screen->w);
        return -2;
    }
    clear_screen(screen);
    screen->pixel_height = PIXEL_DIM;
    screen->pixel_width = PIXEL_DIM;
    return 0;
}

/**
 * @brief Libere la memoire allouer avant de fermer lapplication
 * 
 * @param screen 
 */
void destroy_screen(struct s_screen *screen)
{
    SDL_DestroyRenderer(screen->r);
    SDL_DestroyWindow(screen->w);
}

/**
 * @brief Met à jour l'ecran
 * 
 * @param screen 
 */
void update_screen(struct s_screen *screen)
{
    //au lieu de tout redessiner, on met lecran en noir puis on ne place que les pixels blancs
    SDL_SetRenderDrawColor(screen->r, 0, 0, 0, 255);
    SDL_RenderClear(screen->r);
    SDL_SetRenderDrawColor(screen->r, 255, 255, 255, 255);
    for(size_t i = 0; i < PIXEL_BY_WIDTH; i++)
    {
        for(size_t j = 0; j < PIXEL_BY_HEIGHT; j++)
        {
            if(screen->pixels[i][j] == WHITE)
            {
                SDL_Rect pixel_rect = {
                    screen->pixel_width * i, screen->pixel_height * j,
                    screen->pixel_width, screen->pixel_height
                };
                SDL_RenderFillRect(screen->r, &pixel_rect);
            }
        }
    }
    SDL_RenderPresent(screen->r);
}