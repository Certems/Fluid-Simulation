# Fluid-Simulation
A fluid simulation calculated in C++ and animated in Processing. It uses pressure gradients as forces along with other conditions for incompressibility to achieve the effect.

It should be noted that in the manager file, the gravity can be disabled (by hashing out the "push_back()" line for "force_gravity" in "CalcParticlesForces()") to 
showcase gas-like behaviour in a container. This tends to work better than the gravity case as particles are not so strongly forced into the bounding walls.