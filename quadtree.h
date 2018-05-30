#define MAX_NUM_NODES 20
#define MAX_QUERY_NODES 200

typedef float real;

typedef struct Node {
    real x;
    real y;
    int tag;
    void* data;
} Node;


typedef struct Region {
    real x;
    real y;
    real rwidth;
    real rheight;
} Region;


typedef struct Quadtree {
    Node node_array[MAX_NUM_NODES];
    int num_nodes;
    real x;
    real y;
    real qwidth;
    real qheight;
    
    struct Quadtree* northeast;
    struct Quadtree* northwest;
    struct Quadtree* southwest;
    struct Quadtree* southeast;
} Quadtree;


void initquadtree(Quadtree* quadtree, real x, real y, real qwidth, real qheight){
    quadtree->num_nodes = 0;
    quadtree->x = x;
    quadtree->y = y;
    quadtree->qwidth = qwidth;
    quadtree->qheight = qheight;   
    quadtree->northeast = NULL;
    quadtree->northwest = NULL;
    quadtree->southeast = NULL;
    quadtree->southwest = NULL;
}


bool quadcontainsnode(Quadtree* quadtree, Node node){
    if (((quadtree->x <= node.x) && (quadtree->x + quadtree->qwidth) >= node.x) &&
       ((quadtree->y <= node.y) && (quadtree->y + quadtree->qheight) >= node.y)){
           return true;
       }
    return false;
}


bool insert(Quadtree* quadtree, Node node){
    if (!quadcontainsnode(quadtree, node))
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
    Quadtree q = *quadtree;
    quadtree->northeast = (Quadtree*)malloc(sizeof(Quadtree));
    initquadtree(quadtree->northeast, q.x + q.qwidth/2.0, q.y + q.qheight/2.0, q.qwidth/2.0, q.qheight/2.0);
    
    quadtree->northwest = (Quadtree*)malloc(sizeof(Quadtree));
    initquadtree(quadtree->northwest, q.x, q.y + q.qheight/2.0, q.qwidth/2.0, q.qheight/2.0);
    
    quadtree->southwest = (Quadtree*)malloc(sizeof(Quadtree));
    initquadtree(quadtree->southwest, q.x, q.y, q.qwidth/2.0, q.qheight/2.0);
    
    quadtree->southeast = (Quadtree*)malloc(sizeof(Quadtree));
    initquadtree(quadtree->southeast, q.x + q.qwidth/2.0, q.y, q.qwidth/2.0, q.qheight/2.0);
    
}


bool quadintersectsregion(Quadtree* quadtree, Region region){
    int x1 = quadtree->x;
    int x2 = quadtree->x + quadtree->qwidth;
    int x3 = region.x;
    int x4 = region.x + region.rwidth;
    
    int y1 = quadtree->y;
    int y2 = quadtree->y + quadtree->qheight;
    int y3 = region.y;
    int y4 = region.y + region.rheight;
    
    if ((x1 <= x4) && (x2 >= x3) && (y1 <= y4) && (y2 >= y3)){
        return true;
    }
    else {
        return false;
    }
}

void freequadtree(Quadtree* quadtree){
    if (quadtree == NULL)
        return;
        
    freequadtree(quadtree->northeast);
    freequadtree(quadtree->northwest);
    freequadtree(quadtree->southwest);
    freequadtree(quadtree->southeast);
    
    free(quadtree);
}


void clearquadtree(Quadtree* quadtree){
    initquadtree(quadtree, 0, 0, 0, 0);
    
}

void queryquadtree(Quadtree* quadtree, Region region, Node* node_array[], int* idx){
    if (quadtree == NULL)
        return;
    if (idx == NULL)
        return;
    if (quadintersectsregion(quadtree, region)){
        for (int i=0; i<quadtree->num_nodes; i++){
            node_array[*idx] = &(quadtree->node_array[i]);
            *idx = *idx + 1;
            if (*idx > MAX_QUERY_NODES){
                idx = NULL;
                break;
            }

        }
        queryquadtree(quadtree->northeast, region, node_array, idx);
        queryquadtree(quadtree->northwest, region, node_array, idx);
        queryquadtree(quadtree->southwest, region, node_array, idx);
        queryquadtree(quadtree->southeast, region, node_array, idx);
    }
}
