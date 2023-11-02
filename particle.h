#ifndef _PARTICLE_
#define _PARTICLE_
#include "misc.h"
#include <vector>

class Particle
{
    public:
        Particle(int ID_value);
        ~Particle();
    private:
        //pass
    public:
        int ID;
        const float mass = 0.001;   //So total mass ~ 1kg
        FVector pos;
        FVector vel;
        FVector acc;
        std::vector<FVector> forces;

        void SetPosition(float x, float y, float z);
        void SetVelocity(float x, float y, float z);
        void SetAcceleration(float x, float y, float z);

        //...

        void CalcAcc();
        void CalcVel();
        void CalcPos();
        void CalcRebound();
};

#endif