# OpenCL-ConvexHull

This repository contains the implementation of MergeHullCPU and MergeHullGPUA algorithms, aswell as the port of https://timiskhakov.github.io/posts/computing-the-convex-hull-on-gpu QuickHull algorithm to OpenCL.

MergeHull example:
![This is an image](https://github.com/pfagurel/OpenCL-ConvexHull/blob/main/Merge-hull%20example.PNG)

# Graham Merge update
Graham Merge is an alternative to the initial merge algorithm. It allows to remove the hypothesis that the points are initially sorted. 
For it to work, a small tweak has to be done to the JM algorithm in order to initially find the leftmost point.



# Structure

The project is divided into 3 branches. VsProject contains the actual algorithms, with a few tweaks it is possible to import them as a library for your project.
Interface contains the implementation of a graphical application which allows to illustrate the algorithms. main represents the general structure, it is what was used to compile the aplication with the algorithms.

## Building for Windows

The aplication was built using Godotengine 3.3.4 https://godotengine.org/ with GDNative and C++ OpenCL.

A guide on how to configure GDNative can be found here: https://docs.godotengine.org/en/stable/tutorials/plugins/gdnative/gdnative-cpp-example.html.

For VisualStudio 2019 : https://youtu.be/voXapBgDdts

OpenCL can be installed using vcpkg https://vcpkg.io and then configured in Visual Studio 2019.
