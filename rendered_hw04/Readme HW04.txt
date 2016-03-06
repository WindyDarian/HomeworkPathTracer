Extra credits

-Implemented light source functions for cube, sphere and mesh (also random point
picking in triangle). For mesh surface point picking I first pick a triangle
from area-weighted random of triangles then return a random point on triangle.

-I tried to implement a cartoonish BRDF which divide color into discrete levels
according to dot product of normal and light source but the effect is not good
as expectation: not cartoonish, only somehow sharper than lambert
scene/materials/bxdfs/cartoonbxdf
