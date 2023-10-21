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
float Particle::GetSpreadEffect(FVector point)
{
    /*
    Gets the scalar influence this particle has at a given point
    The area affects a zone around the particle
    The value returned is between 0 and 1, where 1 occurs at the centre

    distanceLimit = Specifies distance you want to satify the percentile limit
    distanceLimit = Specifies what percentage of the functions max value (1 here) you want at the distanceLimit (e.g allows you to set when you want the function to decay to 'nothing')
    
    ###
    ###
    ###
    ###
    ###
    ###
    ## <<<<<-----------------

    Needs to have reaction forces
    Also needs finer tuning of density multiplier

    Grad direction may also be wrong
    magnitude of force seems off too at certain places

    In general needs smoother & more purposeful reactions

    ###
    ###
    ###
    ###
    ###
    ###
    ###
    
    */
    float distanceLimit    = 0.025*Manager::GetManager()->canvasPixelDim.x;  //** Adjust these values for influence changes
    float percentileLimit  = 0.005;                                          //**
    float radialCoefficent = log(percentileLimit) / (pow(distanceLimit,2));     //Must be negative for a decaying function; closer to 0 => larger range of influence

    float dist = std::sqrt(pow(point.x-pos.x,2) + pow(point.y-pos.y,2));        //From the particle centre to the point
    //return exp(radialCoefficent*(pow(dist,2)));     //Smooth
    return exp(radialCoefficent*abs(dist));         //Spikey
}
FVector Particle::GetPressureGradient()
{
    /*
    Finds the direction the particle must move to oppose the pressure gradient (reduce pressure => reduce density)
    */
    //Find how density changes in each direction (x,y)
    float delta_step = 1.0;    //## NEEDS TO BE PROPORTIONAL TO CANVAS AREA SIZE ##
    FVector adjustedPos_X;adjustedPos_X.x = pos.x +delta_step;adjustedPos_X.y = pos.y;
    FVector adjustedPos_Y;adjustedPos_Y.x = pos.x;            adjustedPos_Y.y = pos.y +delta_step;
    float pressure_initial    = Manager::GetManager()->GetSpreadValue(pos, ID);          //Pressure at the particle currently
    float pressure_adjusted_X = Manager::GetManager()->GetSpreadValue(adjustedPos_X, ID); //Pressure at the new position from an X adjustment
    float pressure_adjusted_Y = Manager::GetManager()->GetSpreadValue(adjustedPos_Y, ID); //Pressure at the new position from a Y adjustment
    FVector pressureGradient;
    pressureGradient.x = pressure_initial -pressure_adjusted_X;
    pressureGradient.y = pressure_initial -pressure_adjusted_Y;
    //Give normalised unit vector for direction it must move
    float gradientMag = std::sqrt(pow(pressureGradient.x,2) + pow(pressureGradient.y,2));
    FVector uPressureGradient;uPressureGradient.x = pressureGradient.x;uPressureGradient.y = pressureGradient.y;
    if(gradientMag != 0.0){
        uPressureGradient.x *= 1.0/gradientMag;uPressureGradient.y *= 1.0/gradientMag;}
    else{
        uPressureGradient.x *= 0.0;uPressureGradient.y *= 0.0;}
    return uPressureGradient;
}
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
    FVector dim = Manager::GetManager()->canvasPixelDim;
    //If out of X bounds
    if(pos.x < -dim.x/2.0){
        vel.x = -vel.x;                         //Flip X velocity
        pos.x = std::max(-dim.x/2.0 +1, (-dim.x/2.0 -pos.x) -dim.x/2.0); //Move into frame
    }
    if(pos.x > dim.x/2.0){
        vel.x = -vel.x;                         //Flip X velocity
        pos.x = std::min(dim.x/2.0 -1, (dim.x/2.0 -pos.x) +dim.x/2.0);  //Move into frame
    }
    //If out of Y bounds
    if(pos.y < -dim.y/2.0){
        vel.y = -vel.y;                         //Flip Y velocity
        pos.y = std::max(-dim.y/2.0 +1, (-dim.y/2.0 -pos.x) -dim.y/2.0); //Move into frame
    }
    if(pos.y > dim.y/2.0){
        vel.y = -vel.y;                         //Flip Y velocity
        pos.y = std::min(dim.y/2.0 -1, (dim.y/2.0 -pos.x) +dim.y/2.0);  //Move into frame
    }
}