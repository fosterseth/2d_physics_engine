/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2014 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */
/* gcc -o hmh sdl_handmade.c `sdl2-config --cflags --libs` */
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
bool HandleEvent(SDL_Event *Event)
{
    bool ShouldQuit = false;
 
    switch(Event->type)
    {
        case SDL_QUIT:
        {
            SDL_Log("SDL_QUIT\n");
            ShouldQuit = true;
        } break;

        /*case SDL_WINDOWEVENT:
        {
            switch(Event->window.event)
            {
                case SDL_WINDOWEVENT_RESIZED:
                {
                    SDL_Log("SDL_WINDOWEVENT_RESIZED (%d, %d)\n", Event->window.data1, Event->window.data2);
                } break;

                case SDL_WINDOWEVENT_FOCUS_GAINED:
                {
                    SDL_Log("SDL_WINDOWEVENT_FOCUS_GAINED\n");
                } break;

                case SDL_WINDOWEVENT_EXPOSED:
                {
                    SDL_Window *Window = SDL_GetWindowFromID(Event->window.windowID);
                    SDL_Renderer *Renderer = SDL_GetRenderer(Window);
                    static bool IsWhite = true;
                    if (IsWhite == true)
                    {
                        SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
                        IsWhite = false;
                    }
                    else
                    {
                        SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
                        IsWhite = true;
                    }
                    SDL_RenderClear(Renderer);
                    SDL_RenderPresent(Renderer);
                } break;
            }
        } break;*/
    }
    
    return(ShouldQuit);
}

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    // Create our window.
    SDL_Window *Window = SDL_CreateWindow(
        "Particle Physics Engine",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        640,
        480,
        SDL_WINDOW_RESIZABLE);
        
    if(Window)
    {
        // Create a "Renderer" for our window.
        SDL_Renderer *Renderer = SDL_CreateRenderer(
            Window,
            -1,
            0);
            
        if (Renderer)
        {
            SDL_Rect *rect;
            rect->x = 0;
            rect->y = 0;
            rect->w = 100;
            rect->h = 100;
            
            SDL_SetRenderDrawColor(Renderer, 255, 255, 0, 255);
            SDL_RenderClear(Renderer);
            SDL_SetRenderDrawColor(Renderer, 255, 0, 0, 255);
            SDL_RenderFillRect(Renderer, rect);
            //SDL_RenderClear(Renderer);
            /* Clear the entire screen to our selected color. */
            //SDL_RenderClear(Renderer);

            /* Up until now everything was drawn behind the scenes.
               This will show the new, red contents of the window. */
            SDL_RenderPresent(Renderer);

            /* Give us time to see the window. */
            //SDL_Delay(5000);

            for(;;)
            {
                SDL_Event Event;
                SDL_WaitEvent(&Event);
                if (HandleEvent(&Event))
                {
                    break;
                }
            }
        }
    }
    
    SDL_Quit();
    return(0);
}
