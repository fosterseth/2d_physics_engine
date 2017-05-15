#include "core.h"

void main(int argc, char** argv){
    Vector vector = initVector(1, 2);
    printVector(&vector);
    printf("inv: ");
    invert(&vector);
    printVector(&vector);
    printf("mag: ");
    normalize(&vector);
    printf("norm: ");
    printVector(&vector);
    printf("magnorm: %f\n", magnitude(&vector));
    
    Vector v1 = initVector(1,2);
    Vector v2 = initVector(3,4);
    vectorAdd(&v1,&v2);
    printVector(&v1);    
}
