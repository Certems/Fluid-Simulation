#include <iostream>
#include <fstream>
#include <cmath>
#include "manager.h"

Manager* mainManager = new Manager();

Manager::Manager()
{
    nParticles  = 169;
    lSimulation = 900;
    particleFormation = "Rectangle";

    canvasPixelDim.x = 800;     //Dimensions of the canvas (in pixels) that processing will use, so edge collision can be calculated
    canvasPixelDim.y = 800;     // --> Can be translated into arbitrary units within the program

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
    Creates particles in given arrangement and stores them 
    in a 2D array
    */
    float particleSpacing = 40.0;
    if(true)    //Make it check for shapeFormation here
    {
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

    for pressure;
    ########################################################################
    ### COULD SPEED UP BY FINDING N3 PAIRS SO HALF CALCULATIONS REQUIRED ###
    ########################################################################
    1. Create NxN grid (N=particle number)
    2. Shows all forces each particle has on each other
    */
    for(int i=0; i<particles.size(); i++)
    {
        //Reset forces
        particles.at(i).forces.clear();

        //Gravity
        FVector force_gravity;
        force_gravity.y = 0.001*g;
        particles.at(i).forces.push_back(force_gravity);

        //Pressure
        FVector pressureGrad = particles.at(i).GetPressureGradient();
        float pressureMag    = particles.at(i).GetPressureMagnitude();
        FVector force_pressure;force_pressure.x = pressureMag*pressureGrad.x;force_pressure.y = pressureMag*pressureGrad.y;
        particles.at(i).forces.push_back(force_pressure);
    }
}
float Manager::GetSpreadValue(FVector point, int ignoreIndex)
{
    /*
    Evaluated for all particles at that point
    ignoreIndex = index to disregard in summation (usually when evaluating at a particle's exact position), set to -1 if you want all contributions

    ### CAN JUST IGNORE PARTICLES FURTHER THAN SOME DISTANCE, MAY SAVE COMPUTING TIME ###
    */
    //Sum spreading coefficents
    float spreadingFactor  = 0.0;
    for(int i=0; i<particles.size(); i++)
    {
        if(i != ignoreIndex){
            spreadingFactor += particles.at(i).GetSpreadEffect(point);}
    }
    return spreadingFactor;
}
FVector Manager::ColourFromDensity(float density)
{
    /*
    Takes a density value and maps it to a colour
    Low density  => Blue colour
    High density => Red  colour
    */
    FVector newColour;
    float factor = atan(density) / (3.142/2.0);
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
        float densityData  = GetSpreadValue(particles.at(i).pos, i);
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
