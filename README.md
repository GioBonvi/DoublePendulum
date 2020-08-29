# Double Pendulum

This project collects a series of C++ classes, binaries and Python scripts revolving around the study of [double pendulum systems][double pendulum wiki].  
It can be considered sort of a diary of my C++ learning experience: if you are interested I wrote an longer explaination [down here](#comments).

If you are skilled in C++ and want to drop a comment or even better some advice regarding any piece of code please open an issue: I'll be eager to hear and learn from you.

## Setup

The project was developed and tested in a Linux environment (Ubuntu on WSL2), so the instructions refer to a Ubuntu environment.  
The tools/libraries required are:

- `GNU Make` (highly recommended to run the build process)
- `g++` is the default compiler and support for C++17 is required
- `png++` library (required to render the fractal image)

On a modern Ubuntu installation all the dependecies can be installed with: `sudo apt install build-essentials libpng++-dev`

## Main classes

### DoublePendulum

This class provides the basic functions to define the physical parameters of the system, its equation of motion as well as the tools to solve it (`calcNextState()`) and to extract information about the system at any state (`getCartesianCoordinates()`, `getEnergy()`, `getTextOutput()`).

#### `SimpleDoublePendulum` and `CompoundDoublePendulum`

These classes are implementations of `DoublePendulum` describing systems with slightly different mass distribution:

 - `SimpleDoublePendulum` is a classic pendulum with the masses concentrated at the extremity of the rod
 - `CompoundDoublePendulum` is pendulum composed by bars with continuous mass distribution along the rod

These two obviously have similar, but not identical, dynamics.

#### `StateVector`

This class implements named access to a std::vector representing the state variables of the system.

It seemed like a good idea at the beginning since I was not sure if I would only work on a double pendulum system or if I would expand this to other systems, thus requiring a variable number of state variables. If I had to write it now I would probably use a struct (and possibly will change it in the future).

### Fractal

#### `Fractal`

This class uses a `DoublePendulum` to generate a fractal image. The fractal is generated evaluating the number of steps after which one of the two rods "flip" (passing through the vertical upwards position corresponding to -pi in the global reference system) varying the initial conditions (angle of the two rods).

This class only provides the functions to evaluate the data for any given initial condition and pendulum system: the actual data collection is managed by the `Grid` classes.

#### `UniformGrid`

This class takes a fractal and a rectangular domain for the intial conditions, it discretizes the domain in a grid of uniform side length and evaluates the data at the grid nodes.

This is not very efficient since many points of the domain will never meet the "flip" condition, which is only detected by simulating the motion of the system up to the maximum number of steps prescribed, resulting in many computation cycles "wasted" on relatively unintersting parts of the image.

### Fractal/Adaptive

#### `AdaptiveGrid`

This class takes a fractal and a square domain for the intial conditions, it divides the domain in sub-regions using the `DataPoint` and `DataRegion` classes, evaluating the center point of each sub-region and assigning a priority value to the region based on size of the subregions and uniformity in the values of the subregions (larger, less uniform regions have higher priority).  
This lets the program focus more on "more interesting" sections of the image, while neglecting more uniform regions.

## Origin, purpose and future

This project actually started with a friend of mine, a physics student, who I helped writing a simple program in C++ to numerically solve the dynamics of a double pendulum system for one of her exams.

C++ was actually the first language I ever wrote a program in, but while it introduced me to the basics of programming (loops, if statements, a few things about OOP...) I quickly moved on to more beginner friendly languages and never really _learned_ it.

The task woke my desire to fill this gap and the result was a very interesting learning experience.  
These are some of the concepts I tried to work on and the places where I used them:

 - Setup, organization and build process for a slightly complex project (`Makefile`)
 - Various containers from the STL (`multiset` in `AdaptiveGrid`, `vector` and `array` pretty much everywhere)
 - OOP: classes, inheritance, abstract classes (`DoublePendulum` and its derived classes)
 - Operator overload (`StateVector` class)
 - Smart pointers
 - Memory vs performance optimization (`Fractal`, `UniformGrid` and `AdaptiveGrid` classes)
 - Multithreading (`UniformGrid` and `AdaptiveGrid`)
 - Image manipulation and external libraries (`libpng++` in `UniformGrid` and `AdaptiveGrid`)

I still want to expand this project, both to learn new things and to improve the results (especially the visualization of the fractal).  
Some of the possible features to work on:

 - Fractal rendering in C++
    - [png++][png++ website]
    - [cimg][cimg website]
    - [Magick++][magick++ website]
 - GUI with Qt
 - live display of the fractal (navigable, even?)
    - [inspiration][fractal c++ video]

[double pendulum wiki]: https://en.wikipedia.org/wiki/Double_pendulum
[png++ website]: https://www.nongnu.org/pngpp/
[cimg website]: https://cimg.eu/
[magick++ website]: https://imagemagick.org/Magick++/
[fractal c++ video]: https://www.youtube.com/watch?v=PBvLs88hvJ8