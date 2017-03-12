Introduction: 
The purpose of this document is to outline the architecture and technical details that inform the design and implementation of a physics engine that will eventually be capable of narrow and broad phase collision detection and resolution for some common primitives like cubes, sphere and capsules. If time permits, a 3D fluid simulation with SPH using grid partitioning.  

Structure of the engine: 

The core of the engine is built around the Observer pattern. 

The fundamental idea behind the Observer pattern is that a distinction can be made between objects that perform events, and objects that have reason to be notified of those events.
  
When combined with the methodology used in Unreal Engine, where all classes in the engine and all functionality is derived from a class that overrides the base abstract object of C++, this allowed me to arrive at an engine structure that ?theoretically?, should allow any class or object to communicate with any other object. 

The method by which all objects gain the ability to communicate with each other, is to implement an ‘?Observer' interface.

The accompanying class to ‘?Observer’ is ‘?Subject’. 

To keep it simple, any Object that wishes to send an event must own the Subject for that event. This is an abstraction to even further decouple an Object and the Events it wishes to send out. The current plan, is to divide events is into a list of default events that all Objects might own (Init, Load, Tick, Unload, Exit) and custom events which are defined by the programmer at compile time to achieve specific functions.  
?
Any Observer that wishes to be notified to events when this Subject occurs, can register for these events, and the Subject is responsible for delivering the notification to the Observer, and not the Object which originated this event. This is a separation of concerns.