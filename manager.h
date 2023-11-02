#ifndef _MANAGER_
#define _MANAGER_
#include <vector>
#include "misc.h"
#include "particle.h"

/*
The manager controls the flow of the program
it decides when to write data, record data, calculate more steps of the simulation, etc
*/
class Manager
{
    public:
        Manager();
        ~Manager();
    private:
        //pass
    public:
        static constexpr float pixelConversion = 1.0;   //Converts real measurement to pixel measurements --> purely for display in processing
        static constexpr float g = 9.81f;
        int currentSimulationFrame = 0;
        int nParticles;
        int lSimulation;
        FVector canvasPixelDim;
        std::vector<Particle> particles;
        std::vector<std::vector<FVector>> particlePositionHistory;
        std::vector<std::vector<FVector>> particleColourHistory;

        //Initialisation
        static Manager* GetManager();
        void Generate_ParticleArray();

        //Core Loop
        void Calc_SimulationCycles();
        void Calc_SimulationStep();
        void CalcParticleForces();
        FVector ColourFromDensity(float density);
        void StorePositionsIntoHistory();
        void StoreColoursIntoHistory();

        //Finalisation
        void Write_PositionData();
        void Write_ColourData();

        //Other useful functions
        FVector GetRectDimensions(int nParticles);

        //Bug Fixing
        void OutputParticles();
};

#endif