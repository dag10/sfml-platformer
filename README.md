SFML Platformer
===

Description
---

An incomplete 2D side-scrolling platform game I worked on from 2010-2011 using [SFML](http://www.sfml-dev.org) 1.6. I'm no longer actively developing it.

It's pretty modular, so it shouldn't be too difficult to make your own singleplayer/multiplayer game modification. Active development is dead, however. Plus there's a few bugs and memory leaks, but overall it's fairly solid.

#### For more information and screenshots, check out my related [blog entries](http://minipenguin.com/?cat=32).

![Screenshot](http://minipenguin.com/wp-content/uploads/2011/06/plat_2.png).

Compiling Instructions
---

### Linux

( Warning: Build instructions may no longer work.)

First, go here: http://www.sfml-dev.org/tutorials/1.6/start-linux.php
Go to the section "Compiling SFML (for advanced users)", and make sure you have all packages required installed.
As the site suggests, you can run 'sudo apt-get build-dep libsfml'.
Then make sure you install the package libsfml-dev by running 'sudo apt-get install libsfml-dev'.
Finally, run 'compile_linux.sh'. It is an incredibly crude "make script" I created. I'm terrible at making makefiles.

### Windows

For windows development, I use Code::Blocks. I suggest you do the same.
I included two Code::Blocks project files (.cbp) for both the Client and the Server (Platformer_Client.cbp and Platformer_Server.cbp respectively).
Please go into "Settings > Compiler and debugger... > Toolchain executables" and set the Compiler's installation directory to where MinGW is installed.
Please use the latest MinGW version. Go here to download an installer for the latest MinGW version:

http://sourceforge.net/projects/mingw/files/
(Click on "Download mingw-get-inst")
Please make sure to select something along the lines of "Most recent version/revision" when installing.

### OS X

For compiling on OSX I use Xcode 4. Just use the included Xcode project to get you started.
The Xcode project has two targets/schemes: Client and Server.
However, unless you have the SFML framework installed in /Library/Frameworks, you will need to
edit the targets to include the necessary libraries.
Additional instructions can be found here:
http://www.sfml-dev.org/tutorials/1.6/start-osx.php

License
---

GNU GPL v3
See 'COPYING'
