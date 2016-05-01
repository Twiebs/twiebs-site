%title(Writing a new Game Engine)
%date(2016,01,08)
%permalink(2016-01-08)

A year ago I started writing a 3D game engine.  Today I started writing a new 3D game engine from scratch using none of the code from the previous incardination.
This is because the original engine which I had dubbed "Raptor" was written to be very modular and generic.
The problem: too much effort was expended on organizing the code rather than solving the difficult technical problems involved with 3D game engine programming.
Another issue with the initial engine was that there was no defined goal of what exactly I wanted to achieve with the project.
At one point I had written a level editor without any intent to use it just though that it would be cool.

##What was wrong

- C++ templates slowed down compile times
- Focused on too much on design and architecture rather than solving problems
- Written to be very generic rather than attack specific problems directly

The goals I have going into this project are more clearly defined then "just because".
Although the primary reasons for creating this engine are to state my curiosity about all aspects of game engine technologies I intend to work towards a tech demo with gameplay.
The plan is to create a small RPG style engine with a 3rd person combat system.

##The Plan

- Procedurally Generated Terrain
- Collision Detection / Physics Engine
- OpenGL / Possibly Vulcan renderer
	- primary deferred rendering
	- Cascaded shadow maps for large outdoor scenes
	- SSAO
	- secondary forward pass for transparency
	- Procedural Animation system
- 3rd Person player controls with RPG elements
- Runtime hot loading of C++ dlls
- Asset hot reloading
- Audio mixer
