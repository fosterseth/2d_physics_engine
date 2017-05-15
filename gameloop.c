#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include "core.h"

#define MS_PER_UPDATE 16
#define S_PER_UPDATE 0.016f
#define SCREEN_X 1000
#define SCREEN_Y 500
#define SIM_SCALE 1.0f // meters per pixel

#define VELOCITY_X 0.0f
#define VELOCITY_Y 0.0f

#define ACCELERATION_X 0.0f
#define ACCELERATION_Y 0.0f

#define SPRING_CONSTANT 20.0f
#define REST_LENGTH 100.0f

FILE* fp;

void printff(const char* format, ... ){ // printf and then flush to stdout
    va_list arguments; // object of arguments, including "format"
    va_start(arguments, format); // start arglist after the "format" argument
	vprintf(format, arguments);
	fflush(stdout);
    va_end(arguments);
}

void dumpParticle(Particle* particle){
	fprintf(fp, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n",
		particle->position.x,
		particle->position.y,
		particle->velocity.x,
		particle->velocity.y,
		particle->acceleration.x,
		particle->acceleration.y,
		particle->forceAccum.x,
		particle->forceAccum.y,
		particle->damping,
		particle->inverseMass);
}

void printVector(Vector vector){
	printff("\tx %f\ty %f\n", vector.x, vector.y);
}

bool ShouldQuit = false;

int sim2screen(real unit){
    //return (int) (unit / SIM_SCALE);
    return (int) unit;
}

real screen2sim(int unit){
    // printff("unit %d\n", unit);
    // printff("unit scaled %f\n", unit * SIM_SCALE);
    return unit * SIM_SCALE;
}

typedef struct objectRect {
    Particle particle;
    SDL_Rect rect;
    bool active;
} objectRect;

SDL_Renderer *Renderer;
objectRect object;

Particle particle_center;

void forceSpring(Particle* particle1, Particle* particle2){
	Vector force = retVectorSub(&(particle2->position), &(particle1->position));
	real mag = magnitude(&force);
	mag = 20.0f;
	//mag = real_abs(mag - REST_LENGTH) * SPRING_CONSTANT;
    // printff("mag\n\t%f\n", mag);
	normalize(&force);
	scalarMult(&force, mag);
	addForce(particle1, &force);
}

void processInput(SDL_Event *Event)
{
    switch (Event->type)
    {
        case SDL_QUIT:
        {
            printff("SDL_QUIT\n");
            ShouldQuit = true;
        } break;
        
        case SDL_MOUSEBUTTONDOWN:
        {
            if (Event->button.button == SDL_BUTTON_LEFT)
            {
                printff("Left button pressed\n");
                object.active = true;
				// setVector(&(object.particle.position),
							// screen2sim(Event->button.x),
							// screen2sim(Event->button.y));
                setVector(&(object.particle.position), 750.0f, 10.0f); 
				setVector(&(object.particle.velocity), VELOCITY_X, VELOCITY_Y);
				
                
            }
			
			if (Event->button.button == SDL_BUTTON_RIGHT)
			{
				printff("Right button pressed\n");
				ShouldQuit = true;
			}
        } break;
    }    
}

void update(){
	//printff("update\n");
    if (object.active){
		forceSpring(&(object.particle), &(particle_center));
	    dumpParticle(&(object.particle));
        printff("forceAccum\n");
		printVector(object.particle.forceAccum);
		integrate(&(object.particle), S_PER_UPDATE);
		//printff("position\n");
		//printVector(object.particle.position);
		printff("velocity\n");
		printVector(object.particle.velocity);
        object.rect.x = sim2screen(object.particle.position.x);
        object.rect.y = sim2screen(object.particle.position.y);
    }
	return;
}

void render(){
	//printff("render\n");
    SDL_SetRenderDrawColor(Renderer, 255, 155, 0, 255);
    SDL_RenderClear(Renderer);
    if (object.active){
        SDL_SetRenderDrawColor(Renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(Renderer, &(object.rect));
    }
    //SDL_RenderClear(Renderer);
    /* Clear the entire screen to our selected color. */
    //SDL_RenderClear(Renderer);

    /* Up until now everything was drawn behind the scenes.
       This will show the new, red contents of the window. */
    SDL_RenderPresent(Renderer);
	return;
}

int main(int argc, char *argv[]){
	SDL_Init(SDL_INIT_VIDEO);
	unsigned int previous = SDL_GetTicks();
	unsigned int lag = 0;

	SDL_Window *Window = SDL_CreateWindow(
				"Particle Physics Engine",
				SDL_WINDOWPOS_UNDEFINED,
				SDL_WINDOWPOS_UNDEFINED,
				SCREEN_X,
				SCREEN_Y,
				SDL_WINDOW_RESIZABLE);
				
	int numrender = 0;
	int numupdate = 0;
    
    object.particle = initParticle();
    object.rect.h = 15;
    object.rect.w = 15;
	setMass(&(object.particle), 1.0f/15.0f);
	setVector(&(object.particle.acceleration),
				ACCELERATION_X,
				ACCELERATION_Y);
    object.active = false;
    
	
	particle_center = initParticle();
	setVector(&(particle_center.position),
				500.0f,
				10.0f);
			  
	fp = fopen("c:\\users\\sbf\\desktop\\dump.txt", "w");
	fprintf(fp, "pos_x,pos_y,vel_x,vel_y,acc_x,acc_y,accum_x,accum_y,damping,mass\n");
	unsigned int beginApp;
	if (Window){
		

		Renderer = SDL_CreateRenderer(
			Window,
			-1,
			0);
		if (Renderer){
			SDL_SetRenderDrawColor(Renderer, 255, 155, 0, 255);
			SDL_RenderClear(Renderer);
			SDL_RenderPresent(Renderer);
			
			SDL_Event Event;

			beginApp = SDL_GetTicks();

			while (!ShouldQuit){
			
				unsigned int current = SDL_GetTicks();
				unsigned int elapsed = current - previous;
				previous = current;
				lag += elapsed;

				
				while (SDL_PollEvent(&Event) && !ShouldQuit){
                    processInput(&Event);
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
	printff("numrender %d\n", numrender);
	printff("numupdate %d\n", numupdate);
	unsigned int endApp = SDL_GetTicks();
	printff("app life %f\n", (endApp - beginApp)/1000.0f);
	SDL_Quit();
	
	fclose(fp);
	return 0;
}