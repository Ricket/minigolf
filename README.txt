CSC 462 Assignment 1: Minigolf Rendering
Richard Carter
2012/01/27


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
Click and drag the left mouse button to rotate (orbit) around the hole.
Use the arrow keys to translate the hole along the XZ plane.
Use the quotation (") and question mark (?) keys to raise and lower the hole.
Press the escape key to exit.
