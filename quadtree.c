#include <stdbool.h>
#include <stdio.h>
#include <windows.h>

#define MAX_NUM_NODES 2

typedef float real;

real rand_float(real Min, real Max){
    return (((real)(rand()) / (real)(RAND_MAX)) * (Max - Min)) + Min;
}

typedef struct Node {
    real x;
    real y;
} Node;

typedef struct Boundary {
    real x;
    real y;
    real width;
    real height;
} Boundary;

typedef struct Quadtree {
    Node node_array[MAX_NUM_NODES];
    int num_nodes;
    Boundary boundary;
    
    struct Quadtree* northeast;
    struct Quadtree* northwest;
    struct Quadtree* southwest;
    struct Quadtree* southeast;
} Quadtree;

void initquadtree(Quadtree* quadtree, real x, real y, real width, real height){
    quadtree->num_nodes = 0;
    quadtree->boundary.x = x;
    quadtree->boundary.y = y;
    quadtree->boundary.width = width;
    quadtree->boundary.height = height;   
    quadtree->northeast = NULL;
    quadtree->northwest = NULL;
    quadtree->southeast = NULL;
    quadtree->southwest = NULL;
}

bool boundarycontainsnode(Boundary boundary, Node node){
    if (((boundary.x < node.x) && (boundary.x + boundary.width) > node.x) &&
       ((boundary.y < node.y) && (boundary.y + boundary.height) > node.y)){
           return true;
       }
    return false;
}

bool insert(Quadtree* quadtree, Node node){
    if (!boundarycontainsnode(quadtree->boundary, node))
        return false;
        
    if (quadtree->num_nodes < MAX_NUM_NODES){
        quadtree->node_array[quadtree->num_nodes] = node;
        quadtree->num_nodes = quadtree->num_nodes + 1;
        return true;
    }
    
    if (quadtree->northwest == NULL)
        subdivide(quadtree);
        
    if (insert(quadtree->northwest, node))
        return true;
    if (insert(quadtree->northeast, node))
        return true;
    if (insert(quadtree->southwest, node))
        return true;  
    if (insert(quadtree->southeast, node))
        return true;

    return false;
}



void subdivide(Quadtree* quadtree){
    Boundary b = quadtree->boundary;
    quadtree->northeast = (Quadtree*)malloc(sizeof(Quadtree));
    initquadtree(quadtree->northeast, b.x + b.width/2.0, b.y + b.height/2.0, b.width/2.0, b.height/2.0);
    
    quadtree->northwest = (Quadtree*)malloc(sizeof(Quadtree));
    initquadtree(quadtree->northwest, b.x, b.y + b.height/2.0, b.width/2.0, b.height/2.0);
    
    quadtree->southwest = (Quadtree*)malloc(sizeof(Quadtree));
    initquadtree(quadtree->southwest, b.x, b.y, b.width/2.0, b.height/2.0);
    
    quadtree->southeast = (Quadtree*)malloc(sizeof(Quadtree));
    initquadtree(quadtree->southeast, b.x + b.width/2.0, b.y, b.width/2.0, b.height/2.0);
    
}

    
void main(){
    srand((unsigned int)time(NULL));
    Node xy;
    xy.x = 10;
    xy.y = 10;
    
    Quadtree* Q = (Quadtree*)malloc(sizeof(Quadtree));

    initquadtree(Q, 0,0,20,20);
    for (int i=0; i < 30; i++){
        xy.x = rand_float(0,20);
        xy.y = rand_float(0,20);
        insert(Q, xy);
    }
    return;
}