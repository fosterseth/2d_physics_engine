#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdarg.h>
#include <windows.h>
#include "core.h"

#define MS_PER_UPDATE 16
#define S_PER_UPDATE 0.016f
#define SCREEN_X 1920
#define SCREEN_Y 1080

#define VELOCITY_X 0.0f
#define VELOCITY_Y 0.0f

#define ACCELERATION_X 0.0f
#define ACCELERATION_Y 0.0f

#define N 5000
#define RECT_SIZE 5

#define ACC_RANGE 100.0f

bool ShouldQuit = false;


typedef struct objectRect {
    Particle particle;
    SDL_Rect rect;
    bool active;
    SDL_Color color;
} objectRect;

//bool is_colliding(objectRect* o1, objectRect* o2){
//    if o1->particle->
//}

void detect_collision(){
    for (int i=0; i<N; i++){
        for (int j=0; j<N; j++){
            
        }
    }
}

SDL_Renderer *Renderer;

objectRect rect_array[N];

void process_input(SDL_Event *Event)
{
    switch (Event->type)
    {
        case SDL_QUIT:
        {
            printf("SDL_QUIT\n");
            ShouldQuit = true;
        } break;
        
        case SDL_MOUSEBUTTONDOWN:
        {
            if (Event->button.button == SDL_BUTTON_LEFT)
            {
                printf("Left button pressed\n");
                display_max_velocity();
            }
			
			if (Event->button.button == SDL_BUTTON_RIGHT)
			{
				printf("Right button pressed\n");
                reset_position();
			}
        } break;
    }    
}

real rand_float(real Min, real Max){
    return (((real)(rand()) / (real)(RAND_MAX)) * (Max - Min)) + Min;
}

void reset_position(){
    set_particle_props();
//    for (int i = 0; i < N; i++){
//        setVector(&rect_array[i].particle.position, SCREEN_X / 2, SCREEN_Y / 2);
//    }
}

void display_max_velocity(){
    real max_velocity_sq = -1.0f;
    real tmp;
    int max_i = 0;
    for (int i = 0; i<N; i++){
        tmp = squareMagnitude(&rect_array[i].particle.velocity);
        if (tmp > max_velocity_sq){
            max_velocity_sq = tmp;
            max_i = i;
        }
    }
//    rect_array[max_i].color.b = 0;
    printf("max velocity %f\n", magnitude(&rect_array[max_i].particle.velocity));
}


void update(){
    for (int i = 0; i<N; i++){
        integrate(&rect_array[i].particle, S_PER_UPDATE);
    }
}

void render(){
    SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
    SDL_RenderClear(Renderer);
    for (int i = 0; i<N; i++){
        SDL_SetRenderDrawColor(Renderer,
                                rect_array[i].color.r,
                                rect_array[i].color.g,
                                rect_array[i].color.b,
                                rect_array[i].color.a);
        rect_array[i].rect.x = rect_array[i].particle.position.x;
        rect_array[i].rect.y = rect_array[i].particle.position.y;
        SDL_RenderDrawRect(Renderer, &rect_array[i].rect);
    }
    SDL_RenderPresent(Renderer);
}

void set_particle_props(){
    real y = 500.0f;
    real x = 500.0f;

    real max_mag = 1000.0f;
    for (int i = 0; i<N; i++){
        setVector(&rect_array[i].particle.position, SCREEN_X / 2, SCREEN_Y / 2);
        Vector velocity = initVector(rand_float(-x, x), rand_float(-y, y));
        normalize(&velocity);
        Vector acceleration = velocity;
        invert(&acceleration);
        scalarMult(&acceleration, 500.0f);
        real random_scalar = rand_float(max_mag/2.0f, max_mag);
        scalarMult(&velocity, random_scalar);
        rect_array[i].particle.velocity = velocity;
        rect_array[i].particle.acceleration = acceleration;
        rect_array[i].rect.w = RECT_SIZE;
        rect_array[i].rect.h = RECT_SIZE;
        rect_array[i].color.a = 255;
        real prop_max_mag = random_scalar / max_mag;
        rect_array[i].color.r = (int)(prop_max_mag * 255.0f);
        rect_array[i].color.g = 0;
        rect_array[i].color.b = 255;
    }
}

void create_rects(){
    for (int i = 0; i<N; i++){
        rect_array[i].particle  = initParticle();
        rect_array[i].rect.w = RECT_SIZE;
        rect_array[i].rect.h = RECT_SIZE;
        rect_array[i].color.a = 255;
        rect_array[i].color.r = 255;
        rect_array[i].color.g = 255;
        rect_array[i].color.b = 255;
    }
}

int main(int argc, char *argv[]){
    srand((unsigned int)time(NULL));

	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window *Window = SDL_CreateWindow(
				"Particle Physics Engine",
				SDL_WINDOWPOS_UNDEFINED,
				SDL_WINDOWPOS_UNDEFINED,
				SCREEN_X,
				SCREEN_Y,
				SDL_WINDOW_RESIZABLE);
				
                
    int numrender = 0;
    int numupdate = 0;
    
    unsigned int previous = SDL_GetTicks();
    unsigned int beginApp = previous;
    unsigned int lag = 0;
	if (Window){
		Renderer = SDL_CreateRenderer(
			Window,
			-1,
			0);
		if (Renderer){
            SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
            SDL_RenderClear(Renderer);
            SDL_RenderPresent(Renderer);
			
			SDL_Event Event;
            

            
            create_rects();
            set_particle_props();

			while (!ShouldQuit){
			
				unsigned int current = SDL_GetTicks();
				unsigned int elapsed = current - previous;
				previous = current;
				lag += elapsed;

				
				while (SDL_PollEvent(&Event) && !ShouldQuit){
                    process_input(&Event);
                }

				while (lag >= MS_PER_UPDATE)
				{
					update();
					numupdate += 1;
					lag -= MS_PER_UPDATE;
				}

				render();
				numrender += 1;
			}
		}
	}
	printf("numrender %d\n", numrender);
	printf("numupdate %d\n", numupdate);
	unsigned int endApp = SDL_GetTicks();
	printf("app life %f\n", (endApp - beginApp)/1000.0f);
    SDL_DestroyRenderer(Renderer);
    Renderer = NULL;
    SDL_DestroyWindow(Window);
    Window = NULL;
	SDL_Quit();
	return 0;
}