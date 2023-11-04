#include <iostream>
#include <cmath>
#include "particle.h"
#include "manager.h"

Particle::Particle(int ID_value)
{
    ID = ID_value;  //The index in the particles vector array that this number occurs at
}
Particle::~Particle()
{
    //pass
}



void Particle::SetPosition(float x, float y, float z)
{
    /*
    Sets vector to exact values in a short-hand way
    */
    pos.x = x; //## Careful with references here ##
    pos.y = y;
    pos.z = z;
}
void Particle::SetVelocity(float x, float y, float z)
{
    /*
    Sets vector to exact values in a short-hand way
    */
    vel.x = x; //## Careful with references here ##
    vel.y = y;
    vel.z = z;
}
void Particle::SetAcceleration(float x, float y, float z)
{
    /*
    Sets vector to exact values in a short-hand way
    */
    acc.x = x; //## Careful with references here ##
    acc.y = y;
    acc.z = z;
}

//...

void Particle::CalcAcc()
{
    acc.x = 0.0;    //Inialise acceleration
    acc.y = 0.0;
    for(int i=0; i<forces.size(); i++){
        acc.x += forces.at(i).x/mass;
        acc.y += forces.at(i).y/mass;
    }
}
void Particle::CalcVel()
{
    vel.x += acc.x;
    vel.y += acc.y;
    vel.x *= 0.9;   //Friction
    vel.y *= 0.9;   //
}
void Particle::CalcPos()
{
    pos.x += vel.x;
    pos.y += vel.y;
    CalcRebound();
}
void Particle::CalcRebound()
{
    /*
    Calculates the rebound of particles if they collide with walls
    Note; The centre of the canvas is treated as the origin
    */
    FVector dim = Manager::GetManager()->canvasDim;
    //If out of X bounds
    if(pos.x < -dim.x/2.0){
        vel.x = -vel.x;                         //Flip X velocity
        pos.x = std::max(-dim.x/2.0 +dim.x/400.0, (-dim.x/2.0 -pos.x) -dim.x/2.0); //Move into frame
    }
    if(pos.x > dim.x/2.0){
        vel.x = -vel.x;                         //Flip X velocity
        pos.x = std::min(dim.x/2.0 -dim.x/400.0, (dim.x/2.0 -pos.x) +dim.x/2.0);  //Move into frame
    }
    //If out of Y bounds
    if(pos.y < -dim.y/2.0){
        vel.y = -vel.y;                         //Flip Y velocity
        pos.y = std::max(-dim.y/2.0 +dim.y/400.0, (-dim.y/2.0 -pos.x) -dim.y/2.0); //Move into frame
    }
    if(pos.y > dim.y/2.0){
        vel.y = -vel.y;                         //Flip Y velocity
        pos.y = std::min(dim.y/2.0 -dim.y/400.0, (dim.y/2.0 -pos.x) +dim.y/2.0);  //Move into frame
    }
}