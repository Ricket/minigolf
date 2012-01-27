CSC 462 Assignment 1: Minigolf Rendering
Richard Carter
2012/01/27


== System Requirements
Windows:
  Windows 7, Vista, XP
  Visual Studio
Mac/Linux:
  CMake
  CMake-compatible build tools (e.g. GCC and GNU Make)
All:
  OpenGL and GLUT libraries


== Compiling
Windows:
  Open the Visual Studio project file. Build the project.
Mac/Linux:
  Run `cmake .` to generate the Makefile. Run `make` to build the project.


== Running
Windows:
  In Visual Studio, run the project.
Mac/Linux:
  The binaries are placed in the project directory. `chmod +x Minigolf` if
  necessary, and then `./Minigolf` (or double-click the Minigolf binary).


== Parameters
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
