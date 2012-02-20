CSC 462 Assignment 2: Ball Physics
Richard Carter
2012/02/19


== System Requirements
Windows:
  Windows 7, Vista, XP
  Visual Studio
  GLUT for Win32 (can be found at http://user.xmission.com/~nate/glut.html)
Mac/Linux:
  CMake
  CMake-compatible build tools (e.g. GCC and GNU Make)
All:
  OpenGL libraries


== Compiling
Windows:
  Open the Visual Studio solution file (.sln) in /vs2010. Build the solution.
Mac/Linux:
  Run `cmake .` to generate the Makefile. Run `make` to build the project.


== Running
Windows - command line:
  Open a command line (cmd.exe) and run Minigolf.exe (in the project folder).
Windows - Visual Studio 2010:
  In Visual Studio, set the Minigolf project as the startup project, then run.
Mac/Linux:
  The binaries are placed in the project directory. `chmod +x Minigolf` if
  necessary, and then `./Minigolf` (or double-click the Minigolf binary).


== Arguments
Minigolf [GLUT_args] input_filename

GLUT_args:
  Optional arguments to pass to GLUT. See the docs for glutInit.
  Example: ./Minigolf -geometry 500x500 hole.00.db
    This would open Minigolf with a size of 500x500 and load hole.00.db.
input_filename:
  Path to the hole definition file to load.


== Usage
Click and drag the right mouse button to rotate (orbit) around the hole.
Use the arrow keys to translate the hole along the XZ plane.
Use the quotation (") and question mark (?) keys to raise and lower the hole.
Press the escape key to exit.

When the arrow is spinning, click the left mouse button to select direction.
When the arrow is expanding and contracting, click the left mouse button to
select speed and fire the ball.

== Known bugs/shortcomings
After some number of hits, and depending on your machine architecture, the ball
will sometimes end up disappearing below the top of a tile. Or, the ball will
not correctly collide with the edge of two tiles and the program will enter an
infinite loop. This bug is due to floating point inaccuracy and to my 
implementation of the ball's position. I did not have time to fix it (as that
would require adjusting every bit of ball interaction code for a new coordinate
system) but intend on doing so before the next assignment submission.

Gravity is not implemented. I intend on remedying this by the next assignment
submission.
