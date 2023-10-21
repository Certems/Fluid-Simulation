#include <iostream>
#include <string>
#include "manager.h"

/*
This program should create a system of fluids as specified, then calculate the motion 
of these fluid particles through some time, recording their positions at each step of 
motion. This data will then be saved to a document and displayed in processing.

Conventions;
. The naming convention for new class is to have an upper case starting the name, but a lower case in the file name
*/

int main()
{
    std::cout << "Program Start" <<std::endl;

    //Manager::GetManager()->OutputParticles();         //## Bug Fixing ##
    Manager::GetManager()->Calc_SimulationCycles();
    Manager::GetManager()->Write_PositionData();
    Manager::GetManager()->Write_ColourData();

    std::cout << "Program End" <<std::endl;
    return 0;
}