# DENGINE - An OpenGL experimental engine

This project is a standalone OpenGL engine that was initially put together for a semestral project of the PGR class at CTU FEL university in Prague.
Later it was incorporated into the I3T project (Interactive Tool for Teaching Transformations) as part of my bachelor thesis
and then later yet again transformed into this standalone project for the PGR2 class.

# How to build and run using CMake

The project can be built using CMake (3.15+) and git

## Downloading dependencies

First dependencies must be downloaded. Dependencies are managed using git submodules.

Dependencies can be downloaded using a prepared CMake script that can be run with:

`cmake -P DownloadDependencies.cmake`

If the repository was cloned from remote origin or its downloaded archive containes the `.git` folder all that's needed to do is run:

`git submodule update --init --recursive `

Either of these commands should download the necessary dependency repositories into the `libs` folder.  
This only needs to be done once before building the project.

Now the CMake build script can be run.

## Building

Preferably the Ninja build system should be used.
In that case make sure Ninja is installed.

Following two commands build the project:

    cmake -G Ninja . -DCMAKE_BUILD_TYPE=<build-type>
    cmake --build . -j<thread-count>

`<thread-count>` := Number of CPU threads to use for building.  
`<build-type>` := Build type, should be `Release` for maximum performance. 
Choices are `Debug`, `RelWithDebInfo`, `Release` (and `MinSizeRel`)

Example (Release, Ninja build system and 8 threads):

    cmake -G Ninja . -DCMAKE_BUILD_TYPE=Release
    cmake --build . -j8

You can also let CMake choose which build system to use automatically by omitting the `-G Ninja` parameter.
	
## Running
After the project is built, the executable binary will be located in the `bin/<build-type>` folder. 
For the Release build type that would be `bin/Release`.