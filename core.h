#include <stdio.h>
#include <math.h>

#define real_pow powf
#define real_abs fabsf

typedef float real;

typedef struct Vector {
    real x;
    real y;
} Vector;

Vector initVector(real x, real y){
    Vector vector;
    vector.x = x;
    vector.y = y;
    return vector;
}

void setVector(Vector* vector, real x, real y){
	vector->x = x;
	vector->y = y;
}

void invert(Vector* vector){
    vector->x = -vector->x;
    vector->y = -vector->y;
}

real magnitude(Vector* vector){
    return sqrtf(vector->x * vector->x + vector->y * vector->y);
}

real squareMagnitude(Vector* vector){
    return vector->x * vector->x + vector->y * vector->y;
}

void normalize(Vector* vector){
    real l = magnitude(vector);
    if (l > 0){
        vector->x *= ((real) 1)/l;
        vector->y *= ((real) 1)/l;
    }
}

void scalarMult(Vector* vector, real value){
    vector->x *= value;
    vector->y *= value;
}

Vector retScalarMult(Vector* vector, real value){
    return initVector(vector->x * value, vector->y * value);
}

void vectorAdd(Vector* vector1, Vector* vector2){
    vector1->x += vector2->x;
    vector1->y += vector2->y;
}

Vector retVectorAdd(Vector* vector1, Vector* vector2){
    return initVector(vector1->x + vector2->x, vector1->y + vector2->y);
}

void vectorSub(Vector* vector1, Vector* vector2){
    vector1->x -= vector2->x;
    vector1->y -= vector2->y;
}

Vector retVectorSub(Vector* vector1, Vector* vector2){
    return initVector(vector1->x - vector2->x, vector1->y - vector2->y);
}

void addScaledVector(Vector* vector1, Vector* vector2, real scale){
    vector1->x += vector2->x * scale;
    vector1->y += vector2->y * scale;
}

Vector retComponentVector(Vector* vector1, Vector* vector2){
    return initVector(vector1->x * vector2->x, vector1->y * vector2->y);
}

void componentVector(Vector* vector1, Vector* vector2){
    vector1->x *= vector2->x;
    vector1->y *= vector2->y;
}

real retScalarProduct(Vector* vector1, Vector* vector2){
	return vector1->x * vector2->x + vector1->y * vector2->y;
}

typedef struct Particle{
	Vector position;
	Vector velocity;
	Vector acceleration;
	Vector forceAccum;
    Vector contactNormal;
    real separatingVelocity;
	real damping;
	real inverseMass;
} Particle;

Particle initParticle(){
    Particle particle;
    particle.position = initVector(0.0f, 0.0f);
    particle.velocity = initVector(0.0f, 0.0f);
    particle.acceleration = initVector(0.0f, 0.0f);
    particle.forceAccum = initVector(0.0f, 0.0f);
    particle.contactNormal = initVector(0.0f, 0.0f);
    particle.separatingVelocity = 0.0f;
    particle.damping = 0.998f;
    particle.inverseMass = 1.0f;
    return particle;
}

void setMass(Particle* particle, real mass){
	particle->inverseMass = 1.0f/mass;
}

void addForce(Particle* particle, Vector* force){
	vectorAdd(&(particle->forceAccum), force);
}

void integrate(Particle* particle, real duration){
    if (particle->inverseMass <= 0.0f) return;
    // update linear position
    addScaledVector(&(particle->position), &(particle->velocity), duration);

	// accumulate forces
    Vector resultingAcc = particle->acceleration;
    addScaledVector(&resultingAcc, &(particle->forceAccum), particle->inverseMass);
	
    // update linear velocity
    addScaledVector(&(particle->velocity), &resultingAcc, duration);
    
    // Impose drag
    scalarMult(&(particle->velocity), real_pow(particle->damping, duration));
	
	// reset forceAccum
	setVector(&(particle->forceAccum), 0.0f, 0.0f);
    
}

void calculateSeparatingVelocity(Particle* particle1, Particle* particle2){
    Vector relativeVelocity = retVectorSub(&particle1->velocity, &particle2->velocity);
    Vector contactNormal = retVectorSub(&particle1->position, &particle2->position);
    normalize(&contactNormal);
    particle1->contactNormal = contactNormal;
    particle1->separatingVelocity = retScalarProduct(&relativeVelocity, &contactNormal);
}

void resolveVelocity(Particle* particle1, Particle* particle2){
    
    if (particle1->separatingVelocity > 0)
        return;
    
    real newSepVelocity = -particle1->separatingVelocity * 1;
    real deltaVelocity = newSepVelocity - particle1->separatingVelocity;
    
    real totalInverseMass = particle1->inverseMass;
    
    if (particle2)
        totalInverseMass += particle2->inverseMass;
    
    if (totalInverseMass <= 0)
        return;
        
    real impulse = deltaVelocity / totalInverseMass;
    Vector impulsePerIMass = retScalarMult(&particle1->contactNormal, impulse);
    
    addScaledVector(&particle1->velocity, &impulsePerIMass, particle1->inverseMass);
    
    if (particle2)
        addScaledVector(&particle2->velocity, &impulsePerIMass, -particle2->inverseMass);
    
}