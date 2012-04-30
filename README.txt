CSC 462 Assignment 4: External Objects
Richard Carter
2012/04/08
************ NOTE TO SELF: SEARCH FOR TODOS, DO THEM ******************************

== System Requirements
Windows:
  Windows 7, Vista, XP
  Visual Studio 2010 or the Microsoft Visual C++ 2010 Redistributable
  glut32.dll (or GLUT for Win32 (can be found at http://user.xmission.com/~nate/glut.html))
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
Windows:
  Double-click Minigolf.exe.
Windows - Visual Studio 2010:
  In Visual Studio, set the Minigolf project as the startup project, then run.
Mac/Linux:
  The binaries are placed in the project directory. `chmod +x Minigolf` if
  necessary, and then `./Minigolf` (or double-click the Minigolf binary).


== Arguments
Minigolf [GLUT_args] [input_filename]

GLUT_args:
  Optional arguments to pass to GLUT. See the docs for glutInit.
input_filename:
  Path to the hole definition file to load.


== Usage
Click and drag the rotation control to rotate (orbit) around the hole.
Use the arrow keys to translate the hole along the XZ plane.
Use the quotation (") and question mark (?) keys to raise and lower the hole.
Press the escape key or click the Quit button to exit.

When the arrow is spinning, click the left mouse button to select direction.
When the arrow is expanding and contracting, click the left mouse button to
select speed and fire the ball.

To start a new game, click the New game button. Enter the filename of the
course or hole file to load, and then check the boxes and enter names to
enable other players.

The current player and par is displayed on the right sidebar.

At the end of every hole, the scorecard will appear. At the end of a course,
the highscores will appear and entries will have been added for any players
who achieved a new high score. Up to 10 high scores are stored at once.

== Known bugs/shortcomings
Only static object transformations are supported.
