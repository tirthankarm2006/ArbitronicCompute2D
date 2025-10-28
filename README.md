# ArbitronicCompute2D
An C++ Application to play with Compute Shaders to perform some 2D and 3D drawings or simulations

# About
It is a simple application where you can simply play with GLSL compute shader codes that you write. You can create various type of uniform variavles that you can set from the editor, like integer, float, vectors, integer vectors, float vectors. You can also change the work-group size which will change the window size as each pixel corresponds to one work-group. You can create various animations using a pre-defined time variable and other pre-defined variables. You can also provide your own custom Vertex and Fragment shader for rendering(still experimental). Simply create a file and inside write a void function 'Compute' and then what you code inside the function depends on what you want to make and your shader programming skills.
You are provided with a simple shader code inside data/Shaders that you can see.

Some sample codes and the executable are given in inside the EXE_Demos.

# Building
Windows:-
 You can simply clone the repository and run the Windows batch file, if you are on Windows, it will simply generate the Visual Studio solution file which you can open on Visual Studio.
 By default it creates Visual Studio 2019 solution file, but if are using some other Visual Studio versions, simply edit the batch file and replace with either vs2017 or vs2022, which ever version you are running.

Linux or any other OS:-
 You can build with some other library as well like CMAKE. Refer to the premake files from the individual library directory to see the configurations and include folder.

# Future plans
->Not delete and re-assign the unchanged shader variables when shader is recompiled

->Adding support more shader variables, like bool_vec2, bool_vec3, bool_vec4, double, double_vec2, double_vec3, double_vec4, byte, short, unsigned_byte, unsigned_short

->Multiple render passes through multiple compute shader passes

->Removal of the Terminal Assertions in Release build

->Support of Command Line arguments for running application in Debug and Release explicitly from Command Line

->Making more pre-implimented functions like a random value generation function or some other pre-implimented variables

->Will allow user to set a non-empty image2D on the window and apply various effects to it

->Suport of multiple image2D variables(empty or non-empty)
