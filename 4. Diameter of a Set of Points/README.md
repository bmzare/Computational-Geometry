# Diameter of a Set of 2D Points


# Duality-based Algorithm

Algorithm steps:
1. Discover the convex hull of points in the primal plane.
2. Determine the upper and lower convex envelopes in the dual plane.
3. Identify antipodal pairs.
4. Compute the distance between antipodal pairs and return the maximum distance, with a

complexity order of O(n log n)

Examples (20 and 200 points):


[//]: # (<p style="display: flex; justify-content: space-around ;">)

  <img src="https://raw.githubusercontent.com/bmzare/Computational-Geometry/main/4.%20Diameter%20of%20a%20Set%20of%20Points/Duality-Based%20Algorithm/asset/Diameter-1.png" style="width: fit-content;" alt="Image 1">
  <img src="https://raw.githubusercontent.com/bmzare/Computational-Geometry/main/4.%20Diameter%20of%20a%20Set%20of%20Points/Duality-Based%20Algorithm/asset/Diameter-2.png" style="width: fit-content;" alt="Image 2">

[//]: # (</p>)