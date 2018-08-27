JELLO CUBE

------------------------------------------------------------------------------
Concept:

Implementation of multiple 3D mass-spring systems in C++ to simulate jello movement and collision with rigid bounding box

------------------------------------------------------------------------------
Summary:

- Start with implementing 1D and 2D mass-spring systems.
- The program reads the world file and configures the system.
- Based on the integrator input, Euler or RK4 methods are called.
- The acceleration is calculated in physics.cpp.
- For each type of spring - bend, shear and structural 
the extension is calculated.
- The corresponding force and damping is calculated.
- Another type of diagonal springs are also added for extra stability 
(along diagonal of each grid cube).
- A separate method uses trilinear interpolation to calculate the external
force due to force field on a grid point.
- Collision is detected with the bounding box dimensions within an epsilon 
range and corresponsing collision force is applied with damping.
- Since we apply a force after penetration, it acts on the cube after
penetration has taken place. To avoid that and make it look like the jello cube 
bounces on just touching the surface we have an epsilon value.
- The video attached is run on a custom world file (final.w)
- The simulation does not blow up for any of the given input world files for
a decent amount of time.

------------------------------------------------------------------------------
Controls:

ESC: exit application
v: switch wireframe/triangle mode
s: display structural springs on/off
h: display shear springs on/off
b: display bend springs on/off
space: save the current screen to a file, filename index increments automatically 
p: pause on/off
z: camera zoom in
x: camera zoom out
right mouse button + move mouse: camera control 
e: reset camera to default position