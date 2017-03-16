Introduction: 
The purpose of this document is to outline the architecture and technical details that inform the design and implementation of a physics engine that will eventually be capable of narrow and broad phase collision detection and resolution for some common primitives like cubes, sphere and capsules. If time permits, a 3D fluid simulation with SPH using grid partitioning.  

The core of the engine is built around the Observer pattern. See link below for more on that.

http://gameprogrammingpatterns.com/observer.html

[![Capture.png](https://s29.postimg.org/y40sescpz/Capture.png)](https://postimg.org/image/rqbpbj7tv/)

Seen above, two cubes colliding with collision detection returning contact point and penetration normal.
Also there is the Minkowski difference being rendered, along with the simplex being used in EPA and the closest faces on the two cubes. 

The UI widgets are for editing object properties (location, rotation etc) and change debug settings.