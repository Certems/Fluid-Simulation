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
        static constexpr float g = 9.81f;
        int currentSimulationFrame = 0;
        int nParticles;
        int lSimulation;
        FVector canvasDim;
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
        float GetKernalAt(Particle cParticle, FVector point);
        float GetDensityAt(FVector point);
        float GetPressureAt(FVector point);
        FVector GetPressureGradientAt(FVector point);
        FVector GetGravityWell(float strMultiplier, FVector source, FVector point);
        FVector GetGravityPush(float strMultiplier, FVector source, FVector point);
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