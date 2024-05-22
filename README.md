![WhatsApp Image 2024-05-18 at 18 25 29](https://github.com/Mochu1999/Buoyancy-Simulation-2D/assets/87195384/41df818b-8ffb-430a-89cb-ad2f60e191c1)

Carlos Molano 2024

////////////////
WORK IN PROGRESS
////////////////

3D Buoyancy simulation on its own real-time graphics engine.
It generates a free surface via Fourier series and the polyhedron that acts as a floating object. A complex algorithm calculates the exact wetted surfaces, which are the intersections of the polyhedron with the free surface.
The exact algorithm is my own and is thoroughly explained in WettedSurface.cpp. I approximate the average complexity of it at O(n log n).
It's also worth mentioning the triangulation algorithm in Polygons.cpp, another of my own algorithms, which uses an efficient sweep line method where the complexity is limited by the sorting algorithm of the vertices.
The engine is built with OpenGL. The 3D version doesn't use GLM. Instead, I have developed my own algorithms for everything, from the camera matrices to the quaternions. It also uses a templated format for vec2 and vec3 structures (which you can locate in common.hpp).
The polyhedra are stored in binary files created in BinariesManager.hpp. In the 3D version, the CAD feature is not yet finished. In the 2D version, you have a CAD made by me where you can create polygons and save them in a separate binary file, among other features that you can check in KeyMouseInputs.hpp.

![image](https://github.com/Mochu1999/Buoyancy-Simulation-2D/assets/87195384/40d9d3eb-73ae-4fe4-b01e-bc9f088bbddb)

The 2D version has its own algorithm to create text with the help of FreeType. You can check it in text.h. It also has its own data algorithm to represent graphics that you can find in data.hpp.
There are lots of auxiliary functions not described here that you can check in their header files.

THE SOLUTION LACKS EXTERNAL DEPENDENCIES. If for any reason you want to run this unfinished project, you'll need to configure the GLFW, GLEW, and FreeType dependencies on your own.

![image](https://github.com/Mochu1999/Buoyancy-Simulation-2D/assets/87195384/3f18692f-31f3-4132-bfce-73102d80ac66)
