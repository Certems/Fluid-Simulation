#include <iostream>
#include <fstream>
#include <cmath>
#include "manager.h"

Manager* mainManager = new Manager();

Manager::Manager()
{
    nParticles  = 400;
    lSimulation = 200;
    canvasDim.x = 10.0;     //Dimensions of the canvas (in metres)
    canvasDim.y = 10.0;     //Converted to pixels in processing when displayed

    Generate_ParticleArray();
}
Manager::~Manager()
{
    //pass
}


Manager* Manager::GetManager()
{
    /*
    So there is a static, global, copy of manager for all files to use
    */
    return mainManager;
}
void Manager::Generate_ParticleArray()
{
    /*
    Creates particles in a square arrangement and stores them 
    in a 2D array
    */
    float particleSpacing = canvasDim.x/50.0;
    FVector rectDimensions = GetRectDimensions(nParticles);
    FVector posOffset;posOffset.x = particleSpacing*rectDimensions.x/2.0;posOffset.y = particleSpacing*rectDimensions.y/2.0;
    for(int i=0; i<nParticles; i++)
    {
        Particle* newParticle = new Particle(i);
        newParticle->SetPosition(particleSpacing*(i % int(rectDimensions.x)) -posOffset.x, particleSpacing*floor(i / int(rectDimensions.x)) -posOffset.y, 0.0);    //## According to shape given here ##
        newParticle->SetVelocity(0,0,0);
        newParticle->SetAcceleration(0,0,0);

        particles.push_back(*newParticle);
    }
}
void Manager::Calc_SimulationCycles()
{
    /*
    Calculates the motion of particles & stores the position in the particlePositionHistory
    This is repeated for as long as is specified in simulationLength
    Note; You can make this store values every X frames to reduce the frame count and then play at double speed.
        This however will not change the speed the simulation is calculated at
    */
    while(currentSimulationFrame < lSimulation)
    {
        std::cout<<"Frame "<<currentSimulationFrame<<" /"<<lSimulation<<std::endl;
        StorePositionsIntoHistory();    //So it stores the initial position first
        StoreColoursIntoHistory();
        Calc_SimulationStep();
        currentSimulationFrame++;
    }
}
void Manager::Calc_SimulationStep()
{
    /*
    Calculates changes to particles that need to be made
    1. Find forces for each particle
    2. Calculate dynamics from this
    */
    //1
    CalcParticleForces();
    //2
    for(int i=0; i<particles.size(); i++)
    {
        particles.at(i).CalcAcc();
        particles.at(i).CalcVel();
        particles.at(i).CalcPos();
    }
}
void Manager::CalcParticleForces()
{
    /*
    Goes through all particles and applies the relevent forces to them
    . Forces are stored in a list on the particle
    . The particle will them sum them and divide through by its own mass when finding the acceleration
    . This allows an arbitrary number of forces to by applied to any particle individually
    */
    for(int i=0; i<particles.size(); i++)
    {
        //Reset forces
        particles.at(i).forces.clear();

        //Gravity
        FVector force_gravity;
        force_gravity.y = 0.1*g/60.0f;
        particles.at(i).forces.push_back(force_gravity);

        //Pressure
        FVector force_pressureGrad = GetPressureGradientAt(particles.at(i).pos);
        force_pressureGrad.x /= 400.0;force_pressureGrad.y /= 400.0;
        particles.at(i).forces.push_back(force_pressureGrad);

        //Viscosity
        //...
        //Add this once inviscid fluid behaves correctly
        //...

        //Gravity Well
        FVector gWell_source;   //At (0,0)
        FVector force_gWell = GetGravityWell(1.0, gWell_source, particles.at(i).pos);
        particles.at(i).forces.push_back(force_gWell);

        //Gravity Push
        FVector gPush_source;gPush_source.y = Manager::GetManager()->canvasDim.y*0.4;   //At (0,canvasDim.y*0.4)
        FVector force_gPush = GetGravityPush(1.0, gPush_source, particles.at(i).pos);
        particles.at(i).forces.push_back(force_gPush);
    }
}
float Manager::GetKernalAt(Particle cParticle, FVector point)
{
    /*
    Gets the value of the kernal for the ith particle, cParticle, at the 
    position specified, point
    */
   //## THIS TERM NEEDS SERIOUS ADJUSTMENT ##
    float rangeFactor = 5;//171.7;    //Keep positive, so exp power is negative, so we have a gaussian --> this on chosen so at dist=0.0125 ~ 10 pixels the value is 1% of max value (1) --> increase to ~420.5 when doing proper simulation so its influence ends at the edge of the visible boundary given to it

    float dist   = sqrt(pow(cParticle.pos.x -point.x, 2) + pow(cParticle.pos.y -point.y, 2));   //In real units
    float volume = (2.0*3.1415)*(sqrt(3.1415/rangeFactor));
    float kernal = exp(-pow(rangeFactor*dist, 2));                                              //Gaussian => finite area => infinite integral should be fine to normalise
    float kernal_normalised = kernal/volume;
    return kernal_normalised;
}
float Manager::GetDensityAt(FVector point)
{
    /*
    Finds the density by;
    1. Considering the kernal of all particles at position specified
    2. ...
    */
    float kernalTotal = 0.0;
    for(int i=0; i<particles.size(); i++)
    {
        kernalTotal += GetKernalAt(particles.at(i), point);
    }
    return kernalTotal;
}
float Manager::GetPressureAt(FVector point)
{
    /*
    Finds the pressure by;
    1. Considering the kernal of all particles at position specified
    2. ...
    */
    float pressureFactor = 10.0;             //## LIKELY WILL GIVE AN ACCURATE REPRESENTATION ##
    float density = GetDensityAt(point);
    return pressureFactor*density;
}
FVector Manager::GetPressureGradientAt(FVector point)
{
    /*
    Finds the pressure gradient by;
    1. Find the pressure at its centre (can include itself in this calculation)
    2. Travel deltaX & deltaY separately, find the pressure at these points
    3. Use the difference between the central pressure and these stepped pressures, all divided by the step made
    */
    float delta = Manager::GetManager()->canvasDim.x/200.0;   //## MAYBE SMALLER IS REQUIRED, SEE HOW IT GOES
    FVector point_dx;point_dx.x = point.x +delta;point_dx.y = point.y;
    FVector point_dy;point_dy.x = point.x;       point_dy.y = point.y +delta;
    float pressure_dx           = GetPressureAt(point_dx);
    float pressure_dy           = GetPressureAt(point_dy);
    float pressure_central      = GetPressureAt(point);
    FVector pressure_grad;pressure_grad.x = (pressure_central-pressure_dx)/delta;pressure_grad.y = (pressure_central-pressure_dy)/delta;
    return pressure_grad;
}
FVector Manager::GetGravityWell(float strMultiplier, FVector source, FVector point)
{
    /*
    Exerts a force around a source position, such that all forces point inwards to the source 
    and the force decays with distance
    */
    FVector force;
    float dist      = sqrt(pow(source.x -point.x, 2) + pow(source.y -point.y, 2));   //In real units
    float strength  = strMultiplier*(0.05)*exp(-pow(0.6*dist,2));
    FVector unitDir;unitDir.x = (source.x-point.x)/dist;unitDir.y = (source.y-point.y)/dist;
    force.x = unitDir.x*strength;force.y = unitDir.y*strength;
    return force;
}
FVector Manager::GetGravityPush(float strMultiplier, FVector source, FVector point)
{
    /*
    Exerts a force around a source position, such that all forces point outwards to the source 
    and the force decays with distance
    */
    FVector force;
    float dist      = sqrt(pow(source.x -point.x, 2) + pow(source.y -point.y, 2));   //In real units
    float strength  = strMultiplier*(0.05)*exp(-pow(0.6*dist,2));
    FVector unitDir;unitDir.x = (-source.x+point.x)/dist;unitDir.y = (-source.y+point.y)/dist;
    force.x = unitDir.x*strength;force.y = unitDir.y*strength;
    return force;
}
FVector Manager::ColourFromDensity(float density)
{
    /*
    Takes a density value and maps it to a colour
    Low density  => Blue colour
    High density => Red  colour
    */
    FVector newColour;
    float factor = 1.0; //## Remake this ##
    newColour.x = 255.0*(factor);
    newColour.y = 10.0;
    newColour.z = 255.0*(1.0-factor);
    return newColour;
}
void Manager::StorePositionsIntoHistory()
{
    /*
    Takes current position values of data, and stores them in a new list in the particlePositionHistory
    */
    std::vector<FVector> historySet;
    for(int i=0; i<particles.size(); i++)
    {
        FVector newPos;                     //To avoid parse by reference
        newPos.x = particles.at(i).pos.x;   // ### MAY NEED A "new" IN FRONT ###
        newPos.y = particles.at(i).pos.y;   // ### MAY NEED A "new" IN FRONT ###
        historySet.push_back(newPos);
    }
    particlePositionHistory.push_back(historySet);
}
void Manager::StoreColoursIntoHistory()
{
    /*
    Takes current position values of data, and generates the density at those points and stores them in particleColourHistory
    */
    std::vector<FVector> historySet;
    for(int i=0; i<particles.size(); i++)
    {
        float densityData  = 0.0;   //## REDO THIS ##
        FVector colourData = ColourFromDensity(densityData);
        historySet.push_back(colourData);
    }
    particleColourHistory.push_back(historySet);
}
void Manager::Write_PositionData()
{
    /*
    This will write the coords of each fluid particle to a text file.
    The system can then be plotted in processing to visualise the flow.
    */
    std::cout<<"Writing Position Data..."<<std::endl;
    std::ofstream particlePositionData("fluidSimPlotter/particlePositionData.txt");
    for(int j=0; j<particlePositionHistory.size(); j++)                //For each frame
    {
        for(int i=0; i<particlePositionHistory.at(j).size(); i++)      //For each position in that frame
        {
            particlePositionData<<particlePositionHistory.at(j).at(i).x<<","<<particlePositionHistory.at(j).at(i).y<<",";
        }
        particlePositionData<<"\n";
    }
}
void Manager::Write_ColourData()
{
    /*
    This writes data to a file relating to colour, this specifically refers to the 
    pressure at each fluid particle's position.
    This will allow you to see if the system has satisfied incompressability
    */
    std::cout<<"Writing Colour Data..."<<std::endl;
    std::ofstream particleColourData("fluidSimPlotter/particleColourData.txt");
    for(int j=0; j<particleColourHistory.size(); j++)                //For each frame
    {
        for(int i=0; i<particleColourHistory.at(j).size(); i++)      //For each position in that frame
        {
            particleColourData<<particleColourHistory.at(j).at(i).x<<","<<particleColourHistory.at(j).at(i).y<<","<<particleColourHistory.at(j).at(i).z<<",";
        }
        particleColourData<<"\n";
    }
}
FVector Manager::GetRectDimensions(int nParticles)
{
    /*
    Finds the bounding dim of the rect to use
    Chooses an appropriate size
    */
    FVector rectDim;
    int sideLength = ceil(sqrt(nParticles));
    rectDim.x = sideLength;
    rectDim.y = sideLength;
    return rectDim;
}
void Manager::OutputParticles()
{
    /*
    Bug fixing to ensure particles list is working correctly
    */
    std::cout<<"Particles Start;"<<std::endl;  
    for(int i=0; i<particles.size(); i++)
    {
        std::cout<<"("<<particles.at(i).pos.x<<","<<particles.at(i).pos.y<<","<<particles.at(i).pos.z<<"),"<<std::endl;
    }
    std::cout<<std::endl<<"Particles End;"<<std::endl;
}
