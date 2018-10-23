# SGEXT-scripts

**S**patial**G**raph **Ext**ractor.

Provides scripts to generate a thin/skeletonized image from binary images (segmentations).
A thin image is a one pixel wide image, conserving the same topology (same number of holes, and shapes) than the input binary image.

A distance map can be used for the thin image to be in the centerline of the input.

The thinning algorithm used was contributed by the author to the DGtal library, based on the work of Couprie and Bertrand [1]

The thin output can also be converted to a Spatial Graph, this is a regular graph, an adjacency list holding the nodes and edges, plus all the geometrical information. In the case of a nodes/vertices, a spatial node with 3D location. In edges, a spatial edge, a data structure with a consecutive list of points connecting the nodes.
 
Using histo.hpp from: https://github.com/phcerdan/histo-header
SHA: 556ada3ff79c0180a0cbec36ff29a30da5acb367

## Contributors

- Pablo Hernandez-Cerdan
- Megan Chambers

[1]: Couprie and Bertrand, “Asymmetric Parallel 3D Thinning Scheme and Algorithms Based on Isthmuses.” Pattern Recognition Letters. June,  2016. DOI:10.1016/j.patrec.2015.03.014
