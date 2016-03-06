-Extra credits I have done:
SPLIT_SAH
Image-Wide Stratified Sampling
see the bottom of this file

-Note:
Use the menu to toggle render settings, remember to
  1) reload the scene file when switching splitting method for BVH
  2) bbox for triangles is not visible by default, toggle in View menu
      (if enabled it may crash on dragon.obj)

I didn't make BoundingBox itself drawable because I think it is more efficient
to use it as a pure calculation helper class (like Ray), I added a
BoundingBoxFrame as a drawable class to hold only these bounding boxes
needs to be drawn


-Recorded Data:
I7-4720HQ Windows 10 x64
All without TBB enabled by default, Debug Build (I had some problem with Release
build when timing my process although solved now but it is a pain to render them
all again)

Original data is in milliseconds.

Render Times:

1x1 without BVH
    Wahoo.xml:
      Uniform: 356.379 sec
    many_spheres.xml:
      Uniform: 2.717 sec

1x1 with BVH (SPLIT_SAH):
    Wahoo.xml:
      Uniform: 2.533 sec (0.507 seconds with TBB)
      Random: 2.542 sec
      Stratified: 2.569 sec
      Image-Wide Stratified: 2.603 sec (+ 0.530 generation)
    many_spheres.xml:
      Uniform: 2.890 sec
      Random: 2.950 sec
      Stratified: 2.952 sec
      Image-Wide Stratified: 2.917 sec (+ 0.566 sample generation)
    glass_dragon.xml:
      Uniform: 11.052 sec

1x1 with BVH (SPLIT_EQUAL_COUNTS):
    Wahoo.xml:
      Uniform: 3.112 sec
    many_spheres.xml:
      Uniform: 2.868 sec
    glass_dragon.xml:
      Uniform: 14.345 sec

2x2 with BVH (SPLIT_SAH)
    Wahoo.xml:
      Uniform: 9.939 sec
      Random: 10.300 sec
      Stratified: 10.118 sec
      Image-wide Stratified: 9.771 sec (+ 2.112 sample generation)
    many_spheres.xml:
      Uniform: 11.460 sec
      Random: 11.592 sec
      Stratified: 11.459 sec
      Image-wide Stratified: 11.259 sec (+ 2.033 sample generation)

4x4 with BVH (SPLIT_SAH):
    Wahoo.xml:
      Uniform: 38.012 sec (or 7.612s with TBB)
             (49.121 sec (or 9.470s with TBB) when SPLIT_EQUAL_COUNTS)
      Random: 40.090 sec
      Stratified: 40.366 sec
      Image-wide Stratified: 39.494 sec (+ 11.153 sample generation)
    many_spheres.xml:
      Uniform: 44.781 sec (45.87 sec when SPLIT_EQUAL_COUNTS)
      Random: 45.479 sec
      Stratified: 45.268 sec
      Image-wide Stratified: 45.637 sec (+ 11.168 sample generation)
    glass_dragons.xml:
      Image-wide Stratified: 171.208 sec (+ 11.202 sec) (SPLIT_SAH)
      Image-wide Stratified: 33.707 sec (+ 10.972 sec) (SPLIT_SAH + TBB)
      Image-wide Stratified: 221.524 sec (+ 11.315 sec) (SPLIT_EQUAL_COUNTS)

Extra credits:

1.SAH:

raytracing/bvhnode.h

Referenced p217 of physically based rendering 2nd edition
I divide each node into several buckets and estimate costs of intersection test
when splitting from each bucket then choose the minimum one as the split plane.
estimated cost for one side
    = surface area(relative) * object count (traversal cost ignored)


2.Image-Wide Stratified Sampling:

raytracing/samplers/imagewidestratifiedsampler.h

min_distance for n*n sampler as 1/(2n),
MIN_TRIES for each sample is 4, return point with max distance to other all points
for each sample among the 4~100 tries

(If the 100th(MAX_TRIES) attempt still have a distance to all other samples
greater than min_distance then the point with the max distance in the 100 tries
 will be returned)

To calculate the min distance to all other samples in the image:
 I constructed the sample grid in column major order from top-left to
 bottom-right, so for each sample I only needs to check distance with points
 satisfying one of these requirements:
  1) samples in the current pixel;
  2) samples in the right side (<min_distance to right edge) of the left pixel;
  3) samples in the bottom side (<min_distance to right edge) of the upper pixel;
  4) samples in the bottom-right of the upper-left pixel
  5) samples in the top-right of the down-left pixel


Image-Wide Stratified Sampler creation costs:
   1x1 around 0.55 sec
   2x2 around 2.05 sec
   4x4 around 11.1 sec

for up to 4x4 AA, this method is fine for now, I may use a K-D Tree to store and
compare points or store sample points in a data file as improvement when larger
resolution or AA level is needed; or to generate when rendering and delete
sample blocks no longer needed in real time to save memory.
