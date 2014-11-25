# Path Tracer SS

<p align="center">
<img src="https://raw.githubusercontent.com/steveschwarcz/PathTracerSS/master/SteveRay/Renders/render-Dragon100.png" alt="Render"/>
</p>

This is a simple Path Tracer that I made, using a various papers quoted within the code and from "Computer Graphics: Principles and Practice" by  Hughes, Van Dam, et. al.

It's not compeltely polished, but it does feature:


* Togglable ray tracing and path tracing
* Importance sampled Lambert and Phong shaders
* .obj file support
* BVH excelleration structure, using sorted morton codes for a z-order curve
* Multithreading through OpenMP
* Stratified sampling
* Area and point lights

Vector math was handled through the Eigen library.  http://eigen.tuxfamily.org/index.php?title=Main_Page

There's still a lot more I'd like to do with it, but this was really only a proof of concept for myself.  I'm working on a better, faster, and more polished renderer written in CUDA, which can be found [here](https://github.com/steveschwarcz/Cuda-Path-Tracer-SS), and that will have more features in addition to additional polish.

## Usage

Takes 2 optional command line arguments:
	1. Renderer ("path" or "ray"): Path tracer or ray tracer
	2. numRays (<int>): Number of rays per pixel

In order to specify numRays, renderer must be set first.  Output is in image.ppm, located in the solution folder.

When I have time, I will add a lot more configurability, such as the ability to specify different models at run time.  At the moment, I'm saving that for the CUDA version.

NOTE: Models used must have normals associated with each vertex.