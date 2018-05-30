#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdarg.h>
#include <windows.h>
#include "core.h"
#include "quadtree.h"

#define MS_PER_UPDATE 16
#define S_PER_UPDATE 0.016f
#define SCREEN_X 1280
#define SCREEN_Y 720

#define VELOCITY_X 0.0f
#define VELOCITY_Y 0.0f

#define ACCELERATION_X 0.0f
#define ACCELERATION_Y 0.0f

#define DAMPING 0.5f

#define N 3000
#define RECT_SIZE 10
#define RECT_SIZE_BIG 40
#define QTQUERY_REGIONSIZE 20

#define ACC_RANGE 100.0f
#define VEL 100.0f
#define VELMAX 500.0f
#define VEL2 50000.0f

#define R2 252.0f
#define G2 92.0f
#define B2 125.0f

#define R1 106.0f
#define G1 130.0f
#define B1 251.0f

unsigned int previous_cursor_time;

bool ShouldQuit = false;


typedef struct objectRect {
    Particle particle;
    SDL_Rect rect;
    SDL_Color color;
    bool active;
    bool can_move;
    bool in_collision;
} objectRect;

SDL_Renderer* Renderer;

objectRect rect_array[N];
objectRect* cursor_rect;
Vector previous_cursor_position;

Particle* boundary;

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
                detect_collision_with_point(Event->button.x, Event->button.y);
            }
			
			if (Event->button.button == SDL_BUTTON_RIGHT)
			{
				printf("Right button pressed\n");
                reset_position();
			}
        } break;

        case SDL_MOUSEBUTTONUP:
        {
            if (Event->button.button == SDL_BUTTON_LEFT)
            {
                printf("Left button up\n");
                if (cursor_rect){
                    make_cursor_rect_default();
                    set_color_rgb(&cursor_rect->color, 0, 0, 255);
                    cursor_rect->particle.inverseMass = 1.0f;
                    cursor_rect = NULL;
                }
            }
            
        }


//        case SDL_MOUSEMOTION:
//        {
//            if (Event->motion.state == SDL_BUTTON_LMASK){
//                cursor_rect->rect.x = Event->motion.x;
//                cursor_rect->rect.y = Event->motion.y;
//                printf("Left button moving and b1 down\n");
//            }
//        }
    }    
}




void set_color_rgb(SDL_Color* color, Uint8 r, Uint8 g, Uint8 b){
    color->r = r;
    color->g = g;
    color->b = b;
}


void set_color_rgb_gradient(SDL_Color* color, real percent){
    color->r = (Uint8)(R1 + percent * (R2 - R1));
    color->g = (Uint8)(G1 + percent * (G2 - G1));
    color->b = (Uint8)(B1 + percent * (B2 - B1));
}

bool is_colliding(objectRect* o1, objectRect* o2){
    int x1 = o1->rect.x;
    int x2 = o1->rect.x + o1->rect.w;
    int x3 = o2->rect.x;
    int x4 = o2->rect.x + o2->rect.w;
    
    int y1 = o1->rect.y;
    int y2 = o1->rect.y + o1->rect.h;
    int y3 = o2->rect.y;
    int y4 = o2->rect.y + o2->rect.h;
    
    if ((x1 <= x4) && (x2 >= x3) && (y1 <= y4) && (y2 >= y3))
        return true;

    return false;
}

void set_rect_colors_by_state(){
    real magV;
    for (int i=0; i<N; i++){
        magV = squareMagnitude(&rect_array[i].particle.velocity);
        magV = magV / VEL2;
        if (magV > 1.0f)
            magV = 1.0f;
        set_color_rgb_gradient(&rect_array[i].color, magV);
    }
}

void drawquadtree(Quadtree* quadtree){
    if (quadtree == NULL)
        return;
    SDL_SetRenderDrawColor(Renderer, 255, 0, 0, 0);
    int x1 = quadtree->x;
    int x2 = quadtree->x + quadtree->qwidth;
    int y1 = quadtree->y;
    int y2 = quadtree->y + quadtree->qheight;
    SDL_RenderDrawLine(Renderer, x1, y1, x1, y2);
    SDL_RenderDrawLine(Renderer, x2, y1, x2, y2);
    SDL_RenderDrawLine(Renderer, x1, y1, x2, y1);
    SDL_RenderDrawLine(Renderer, x1, y2, x2, y2);
    
    drawquadtree(quadtree->northeast);
    drawquadtree(quadtree->northwest);
    drawquadtree(quadtree->southeast);
    drawquadtree(quadtree->southwest);

}


void detect_collision_with_particle2(){
    int i, j, z, idx;


    for (i=0; i<N; i++){
        for (j=0; j<N; j++){
            if (i!=j){
                if (is_colliding(&rect_array[i], &rect_array[j])){
                    calculateSeparatingVelocity(&rect_array[i].particle, &rect_array[j].particle);
                    if (cursor_rect == &rect_array[i])
                        rect_array[i].particle.separatingVelocity = -40.0f;
                    resolveVelocity(&rect_array[i].particle, &rect_array[j].particle);
                }
            }
        }
    }
}


void detect_collision_with_particle(){
    Quadtree* Q = (Quadtree*)malloc(sizeof(Quadtree));
    initquadtree(Q, 0,0,SCREEN_X,SCREEN_Y);
    Node node;
    Region R;
    int i, j, z, idx;
    for (i=0;i<N; i++){
       node.x = rect_array[i].rect.x;
       node.y = rect_array[i].rect.y;
       node.data = NULL;
       node.tag = i;
       insert(Q, node);
       set_color_rgb(&rect_array[i].color, 0, 0, 255);
    }
    drawquadtree(Q);
    // ==== ENABLE FOR CURSOR_RECT REGION HIGHLIGHT ====
//    if (cursor_rect){
//        idx = 0;
//        Region R;
//        R.x = cursor_rect->rect.x - QTQUERY_REGIONSIZE;
//        R.y = cursor_rect->rect.y - QTQUERY_REGIONSIZE;
//        R.rheight = QTQUERY_REGIONSIZE;
//        R.rwidth = QTQUERY_REGIONSIZE;
//        SDL_Rect regionrect;
//        regionrect.x = R.x;
//        regionrect.y = R.y;
//        regionrect.h = R.rheight;
//        regionrect.w = R.rwidth;
//        SDL_SetRenderDrawColor(Renderer, 255, 255, 0, 0);
//        SDL_RenderDrawRect(Renderer, &regionrect);
//        Node* node_array[MAX_QUERY_NODES] = {NULL};
//        queryquadtree(Q, R, node_array, &idx);
//        for (j=0; j<MAX_QUERY_NODES; j++){
//            if (node_array[j]){
//                z = node_array[j]->tag;
//                set_color_rgb(&rect_array[z].color, 0, 255, 0);
//            }
//        }
//    }
//   ======================================================

    for (i=0; i<N; i++){
        idx = 0;
        R.x = rect_array[i].rect.x - QTQUERY_REGIONSIZE;
        R.y = rect_array[i].rect.y - QTQUERY_REGIONSIZE;
        R.rheight = rect_array[i].rect.h + QTQUERY_REGIONSIZE + QTQUERY_REGIONSIZE;
        R.rwidth = rect_array[i].rect.w + QTQUERY_REGIONSIZE + QTQUERY_REGIONSIZE;
//        if (cursor_rect == &rect_array[i]){
//            SDL_Rect regionrect;
//            regionrect.x = R.x;
//            regionrect.y = R.y;
//            regionrect.h = R.rheight;
//            regionrect.w = R.rwidth;
//            SDL_SetRenderDrawColor(Renderer, 255, 255, 0, 0);
//            SDL_RenderDrawRect(Renderer, &regionrect);
//        }
        Node* node_array[MAX_QUERY_NODES] = {NULL};
        queryquadtree(Q, R, node_array, &idx);
        for (j=0; j<MAX_QUERY_NODES; j++){
            if (node_array[j]){
                z = node_array[j]->tag;
                if (&rect_array[i] == cursor_rect){
                    set_color_rgb(&rect_array[z].color, 0, 255, 0);
                }
                
                if (i!=z){
                    if (is_colliding(&rect_array[i], &rect_array[z])){
                        calculateSeparatingVelocity(&rect_array[i].particle, &rect_array[z].particle);
//                        if (cursor_rect == &rect_array[i])
//                            rect_array[i].particle.separatingVelocity = -40.0f;
                        resolveVelocity(&rect_array[i].particle, &rect_array[z].particle);
                    }
                }
            } 
        }
    }
    clearquadtree(Q);
}

void detect_collision_with_boundary(){
    for (int i = 0; i<N; i++){
        bool collision_detected = false;
        // check collision with right wall
        if (rect_array[i].rect.x + rect_array[i].rect.w > SCREEN_X){
            setVector(&rect_array[i].particle.contactNormal, -1, 0);
            collision_detected = true;
        }
        
        // check collision with bottom wall
        if (rect_array[i].rect.y + rect_array[i].rect.h > SCREEN_Y){
            setVector(&rect_array[i].particle.contactNormal, 0, -1);
            collision_detected = true;
        }
        
        // check collision with left wall
        if (rect_array[i].rect.x < 0){
            setVector(&rect_array[i].particle.contactNormal, 1, 0);
            collision_detected = true;
        }
        
        // check collision with top wall
        if (rect_array[i].rect.y < 0){
            setVector(&rect_array[i].particle.contactNormal, 0, 1);
            collision_detected = true;
        }
        
        if (collision_detected){
            rect_array[i].particle.separatingVelocity = retScalarProduct(&rect_array[i].particle.velocity, &rect_array[i].particle.contactNormal);
            resolveVelocity(&rect_array[i].particle, boundary);
        }
    }
}

void make_cursor_rect_big(){
    cursor_rect->rect.w = RECT_SIZE_BIG;
    cursor_rect->rect.h = RECT_SIZE_BIG;
}

void make_cursor_rect_default(){
    cursor_rect->rect.w = RECT_SIZE;
    cursor_rect->rect.h = RECT_SIZE;
}

void detect_collision_with_point(int x, int y){
    SDL_Point point;
    point.x = x;
    point.y = y;
    for (int i = 0; i < N; i++){
        if (SDL_PointInRect(&point,&rect_array[i].rect)){
            printf("collison with point detected\n");
            cursor_rect = &rect_array[i];
            make_cursor_rect_big();
            set_color_rgb(&cursor_rect->color, 0, 0, 255);
            cursor_rect->particle.inverseMass = 0.0f;
            setVector(&cursor_rect->particle.velocity, 0.0f, 0.0f);
            setVector(&cursor_rect->particle.acceleration, 0.0f, 0.0f);
            previous_cursor_position = rect_array[i].particle.position;
            break;
        }
    }
}

real rand_float(real Min, real Max){
    return (((real)(rand()) / (real)(RAND_MAX)) * (Max - Min)) + Min;
}

void reset_position(){
    set_particle_props();
}

void update(){
    for (int i = 0; i<N; i++){
        integrate(&rect_array[i].particle, S_PER_UPDATE);
    }
}

void set_rect_xy(){
    for (int i = 0; i<N; i++){
        rect_array[i].rect.x = rect_array[i].particle.position.x;
        rect_array[i].rect.y = rect_array[i].particle.position.y;
    }
    
    int x, y;
    if (cursor_rect){
        SDL_GetMouseState(&x, &y);
        cursor_rect->rect.x = x;
        cursor_rect->rect.y = y;
        cursor_rect->particle.position.x = cursor_rect->rect.x;
        cursor_rect->particle.position.y = cursor_rect->rect.y;
    }
}

void render(){
    if (cursor_rect){
        unsigned int current_cursor_time = SDL_GetTicks();
        if (current_cursor_time > previous_cursor_time + 100){
            previous_cursor_time = current_cursor_time;
            Vector tmp = retVectorSub(&cursor_rect->particle.position, &previous_cursor_position);
            scalarMult(&tmp, 1000.0f/100.0f);
            cursor_rect->particle.velocity = tmp;
            previous_cursor_position = cursor_rect->particle.position;
            printf("cursor_rect vel :  %f   %f\n", cursor_rect->particle.velocity.x, cursor_rect->particle.velocity.y);
        }
    }
    SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
    SDL_RenderClear(Renderer);
    detect_collision_with_particle();
    detect_collision_with_boundary();
//    set_rect_colors_by_state();
    set_rect_xy();
    
    for (int i = 0; i<N; i++){
        SDL_SetRenderDrawColor(Renderer,
                                rect_array[i].color.r,
                                rect_array[i].color.g,
                                rect_array[i].color.b,
                                rect_array[i].color.a);
        SDL_RenderDrawRect(Renderer, &rect_array[i].rect);
    }
    SDL_RenderPresent(Renderer);  
}

void set_particle_props(){
//    setVector(&rect_array[0].particle.position, SCREEN_X/2, SCREEN_Y/2);
//    setVector(&rect_array[1].particle.position, SCREEN_X/2, SCREEN_Y);
//    setVector(&rect_array[1].particle.velocity, 0.0f, -200.0f);

    for (int i = 0; i<N; i++){
        setVector(&rect_array[i].particle.position, rand_float(50, SCREEN_X), rand_float(50, SCREEN_Y));
        setVector(&rect_array[i].particle.velocity, rand_float(-VEL, VEL), rand_float(-VEL, VEL));
//        setVector(&rect_array[i].particle.acceleration, 0.0f, 30.0f);
        rect_array[i].particle.damping = DAMPING;

        rect_array[i].rect.w = RECT_SIZE;
        rect_array[i].rect.h = RECT_SIZE;
        rect_array[i].color.a = 255;
        rect_array[i].color.r = 0;
        rect_array[i].color.g = 0;
        rect_array[i].color.b = 255;
    }
}

void create_rects(){
    boundary = NULL;
    cursor_rect = NULL;
//    for (int i = 0; i<4; i++){
//        rect_array[i].particle  = initParticle();
//        rect_array[i].particle.inverseMass = 0;
//        rect_array[i].can_move = false;
//        rect_array[i].color.a = 255;
//        rect_array[i].color.r = 255;
//        rect_array[i].color.g = 255;
//        rect_array[i].color.b = 255;
//        rect_array[i].rect.w = SCREEN_X;
//        rect_array[i].rect.h = SCREEN_Y;
//    }
//    setVector(&rect_array[0].particle.position, 0, -SCREEN_Y);
//    setVector(&rect_array[1].particle.position, -SCREEN_X, 0);
//    setVector(&rect_array[2].particle.position, SCREEN_X, 0);
//    setVector(&rect_array[3].particle.position, 0, SCREEN_Y);

    
    for (int i = 0; i<N; i++){
        rect_array[i].particle  = initParticle();
        rect_array[i].rect.w = RECT_SIZE;
        rect_array[i].rect.h = RECT_SIZE;
        rect_array[i].color.a = 255;
        rect_array[i].color.r = 255;
        rect_array[i].color.g = 255;
        rect_array[i].color.b = 255;
        rect_array[i].can_move = true;
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
            render();

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
    printf("FPS %f\n", (float)numrender/((endApp - beginApp)/1000.0f));
    SDL_DestroyRenderer(Renderer);
    Renderer = NULL;
    SDL_DestroyWindow(Window);
    Window = NULL;
	SDL_Quit();
	return 0;
}