#include "core.h"
#include <SDL2/SDL.h>

// gcc -o app1 app1.c `sdl2-config --cflags --libs`

void printff(char* statement){
	printf("%s", statement);
	fflush(stdout);
}

int HandleEvent(SDL_Event *Event)
{
    int running = 1;
    switch (Event->type)
    {
        case SDL_QUIT:
        {
            printf("SDL_QUIT\n");fflush(stdout);
            running = 0;
        } break;
        
        case SDL_MOUSEBUTTONDOWN:
        {
            if (Event->button.button == SDL_BUTTON_LEFT)
            {
                printff("Left button pressed\n");
            }
			
			if (Event->button.button == SDL_BUTTON_RIGHT)
			{
				printff("Right button pressed\n");
			}
        } break;
    }
    return running;    
}

int main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *Window = SDL_CreateWindow("app1",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        640,
        480,
        SDL_WINDOW_RESIZABLE);

    if(Window)
    {
        // Create a "Renderer" for our window.
        SDL_Renderer *Renderer = SDL_CreateRenderer(Window,-1,0);
        if (Renderer)
        {
            SDL_SetRenderDrawColor(Renderer, 255, 255, 0, 255);
            SDL_RenderClear(Renderer);
			SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
			SDL_Rect rect;
			rect.x = 50;
			rect.y = 50;
			rect.w = 35;
			rect.h = 35;
			SDL_RenderFillRect(Renderer, &rect);
			SDL_RenderPresent(Renderer);
            int running = 1;
            while(running == 1)
            {
                SDL_Event Event;
                while(SDL_PollEvent(&Event))
                {
                    running = HandleEvent(&Event);
                }
            }
        }
    }
    SDL_Quit();
    return 0;
}