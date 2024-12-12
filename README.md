# Bean

### Group Members: Eleanor Park, Logan Hines, Annika Singh, Apoorva Talwalkar

## Design Choices:

In our project, we utilized our Raytracer from projects 3 and 4 as a base, and added on additional features to render a simulation of The Bean (Cloud Gate) in Chicago, Illinois. To add mesh support, we utilized the TinyOBJLoader, which read the vertices of our OBJ file and converted them into points and normals. To allow for mesh rendering, we implemented triangle rendering, and used those triangles to make up the shape of the bean.

Additionally, we added multiple new features to our Raytracer. Firstly, we added ambient occlusion. Using a hemisphere centered on the surface normal of the current ray, we sampled multiple points, tracking the fraction of rays that do not intersect with any scene geometry. This allowed us to measure how much ambient light actually reaches any specific point. We chose to boost this number artificially since our scene only had one light source, and we wanted to balance the visibility of the scene with the efficiency of our rendering.

We also added support for soft shadows. We did this using randomly-generated samples across a specific rectangle in the scene for our point and spot lights. For every sample that is sent out, we keep track of how many are shadowed, and then use that ratio to determine the brightness of the point. For directional lights, the soft shadow implementation randomly samples light direction from a much smaller range, but the rest of the logic remains the same. 

Another feature we added was motion blur support. This is implemented by generating a random time value for each ray, which introduces temporal variance across samples. The random time values shift the objects in motion (spheres, cones) across a direction defined in their corresponding header files. Doing this simulates the effect of motion captured by camera. To improve the quality of the render, particularly for motion blur and soft shadows, we also implemented adaptive supersampling. Our raytracer first starts with 2 samples, compares the calculated pixel value for the samples, and if the difference exceeds a user-defined threshold (in raytracer.h), it supersamples n times (which is also defined in raytracer.h).

Finally, we implemented the BVH acceleration structure which checks the intersection of a ray with a unit cube before checking against a sphere, cylinder, or cone to reduce the number of costly calculations done.

## Contributions/Credits

Bean OBJ File: https://sketchfab.com/3d-models/chicago-bean-f7869191be7940aab889e1d81cd514d1 

TinyOBJLoader (Used to add mesh support): https://github.com/tinyobjloader/tinyobjloader

## Known Bugs

None.
