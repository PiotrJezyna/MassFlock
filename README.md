# Mass Flock
![](/Documentation/FlockBig.gif)

# Overview

This is a small project demonstrating the implementation of a classic flocking algorithm by [Craig Reynolds](https://www.red3d.com/cwr/boids/) using [Mass Entity](https://docs.unrealengine.com/5.1/en-US/overview-of-mass-entity-in-unreal-engine/), a framework for data-oriented calculations available in Unreal Engine 5. 

It is capable of simulating a flock of thousands of agents (up to 15k entities within 16.66 ms on Intel i3-12100KF). Is it a lot? There are a few plugins or other methods likely to simulate more agents in a flock, like Compute Shader or Particle System. However, they offer very limited possibilities of expanding agents with more complex behaviors. On the other hand, Mass Entity has a number of plugins with subsystems that allow us to add any specific functionality we can think of. Keeping that in mind, I'd say the result achieved with Mass Entity is at least impressive.

# How it works

![](/Documentation/FlockSmall.gif)

Flock Entity Template consists of some of the basic Traits built into the plugin and custom Traits responsible for movement and flock simulation.

Three essential Traits created for this project are Flock Cohesion, Flock Alignment, and Flock Avoidance, each responsible for one of the three principles at the core of the flocking algorithm. A place where all the magic behind flocking happens is [FlockProcessor](/Source/MassFlock/FlockProcessor/FlockProcessor.cpp) responsible for calculating forces working on each agent according to all three principles. Since each agent's movement relies on its neighbors, a key challenge in implementing this algorithm is finding entities close to each other. The method used in this project is borrowed from AvoidanceProcessor provided in Mass AI and uses HashGrid to speed up finding entities within a given range, implementation can be seen [here](/Source/MassFlock/FlockUtilities/FlockUtilities.h).

[Flock Movement](https://github.com/PiotrJezyna/MassFlock/blob/main/Source/MassFlock/FlockMovement/FlockMovementProcessor.cpp) is responsible for the actual movement of an agent. It also rotates agents towards the direction they move in.

[Flock Bounds](https://github.com/PiotrJezyna/MassFlock/blob/main/Source/MassFlock/FlockBounds/FlockBoundsProcessor.cpp) is responsible for keeping agents within a given area. The area can be easily setup in editor with [FlockBoundVolume](/Source/MassFlock/FlockBounds/FlockBoundsVolume.cpp) Actor. It uses [FlockBoundsSubsystem](/Source/MassFlock/FlockBounds/FlockBoundsSubsystem.h) as an interface between FlockBoundsVolume and FlockBoundsProcessor. 

Traits built into Mass Entity or its derivatives used in this project are:
- Visualization - no comments required
- LODCollector -  determines the LOD that a given entity should have based on its distance to viewers and to the camera frustum.
- Navigation Obstacle - helps out with the detection of the agent's neighbors using HashGrid.
- Velocity Randomizer - sets up random velocity for each agent when they spawn.
- SimulationLOD - determines whether the simulation will be updated in this frame based on LOD.

The last one is pretty interesting and worth adding a few words. SimulationLOD allows us to enable Variable Tick Rate, which means that agents can tick at a different rate based on their current LOD. This feature is supported by each processor created in this project. Reducing Tick Rate of entities that are outside of the screen or far from the camera to once per 33.33 ms resulted in ~100% boost in performance.
