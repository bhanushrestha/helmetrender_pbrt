# helmetrender_pbrt
PBRT project to render helmet visor at different lighting conditions.

To run the render file

# Install PBRT
##  Basic steps for installing PBRT on windows
### Get the files ready:
1. Install Visual Studio Community 2019 (version 16.11) (include “Desktop development
with C++” package during installation) from
https://visualstudio.microsoft.com/vs/older-downloads/
2. Install CMake (version 3.24.1) from https://cmake.org/download/
3. Get GIT Bash from https://git-scm.com/downloads
4. (recommended) get Visual Studio Code from https://code.visualstudio.com/
5. Download the PBRT repository using GIT
PBRT repository on github: https://github.com/mmp/pbrt-v3
a. Create a directory where you want the 

### PBRT installation
b. From this directory, open GIT Bash (you can do this by right click -> “git bash
here”)
c. Type the following command in the GIT console
git clone --recursive https://github.com/mmp/pbrt-v3.git
d. Do not download the PBRT repository from the web as zip. Make sure to
download the PBRT repository using git recursive, this ensures that all sub
modules are downloaded properly.
Installing PBRT:
6. Create a folder named “build” inside your PBRT-V3 directory.
7. Open cmake-gui, set the source folder (“Where is the source code”) to your PBRT-
V3 directory
8. Set the path for “where to build the binaries” to the build folder you created in step 6.
9. Click on “configure” and choose “Visual Studio 16 2019” the code generator, and x64
as optional platform for generator, and check Use default native compilers.
10. After the configuration is done, click on “generate”. After this step, you should be able
to find “PBRT-V3.sln” file in the build directory.
11. Go to the build folder and open PBRT-V3.sln in visual studio
12. Change to “release” mode
13. Click “build” -> “build solution”
After the process is done, you should find pbrt.exe and several other files now in the folder
...\pbrt-v3\build\Release
## After the installation:
14. In VS Code, click “file” -> “open folder” and choose the path to your release folder
15. To render, open a new terminal (make sure you use cmd and not powershell), type
the following command
“ pbrt <filename.pbrt> ” and hit enter
16. To convert the rendered .exr file to png, use the following command
“ imgtool convert <filename.exr> <filename.png>”

# To run the render

pbrt background.pbrt
