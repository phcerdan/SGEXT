# Image Analysis Pipeline #

   #### Preprocessing. ####
   
1. Bright Equalizing
2. Anisotropic Denoising

    #### Binarization. ####
    
3. Otsu or RegionGrowing
4. Hole Filling

    #### Skeletonization.####
        
5. Isthmus Based Skeletonization

    #### Spatial Graph ####
    
6. Graph From Skeleton.
7. Analyze Statistical Distributions from the Spatial Graph.

    #### [Reconstruct Network] ####
    
8. [Optional]Reconstruct an in-silico network using the statistical distribution obtained.
   
   
## Preprocessing: ##
```bash
export OUTPUT_FOLDER=/home/phc/repository_local/network-graph/results
```

### 1. Bright Equalizing with Wavelets: ###

Using the module ITKIsotropicWavelets for ITK: https://github.com/phcerdan/ITKIsotropicWavelets  
Concretely, using the following script: https://github.com/phcerdan/ITKfilters/blob/master/scripts-cpp/rieszWaveletPhaseAnalysis.cxx  

```bash
rieszWaveletPhaseAnalysis -i ${INPUT_IMAGE} -o ${OUTPUT_FOLDER} -w ${WAVELET_TYPE} -l ${LEVELS} -b ${BANDS}
```

**Parameters:**  
* _W: Wavelet Type {Simoncelli, Held, Vow, Shannon}  
* _L: Levels (usually max levels depending on the size of the image)  
* _B: Bands (4, just a trade off between improvement of quality and RAM usage)  
* Optional: ApplySoftThreshold (default = false)

**Results:** 
1. Actin
```bash
export INPUT_FOLDER=${OUTPUT_FOLDER}
export INPUT_STRING=Actin19_11jun13_2_Series2
export INPUT_EXTENSION=nrrd
export OUTPUT_EXTENSION=nrrd
export INPUT_IMAGE=${INPUT_FOLDER}/${INPUT_STRING}.${INPUT_EXTENSION}
export WAVELET_TYPE=Simoncelli
export LEVELS=4
export BANDS=4
```
2. Pectin
3. Carrageenan
4. Collagen

```bash
export OUTPUT_WAVELET_STRING=${INPUT_STRING}\
_W${WAVELET_TYPE}_L${LEVELS}_B${BANDS}
export OUTPUT_WAVELET=${OUTPUT_FOLDER}/${OUTPUT_WAVELET_STRING}.${OUTPUT_EXTENSION}
```


### 2. Anisotropic Denoising ###
Using [itk::CurvatureAnisotropicDiffusionImageFilter](https://itk.org/Doxygen/html/classitk_1_1CurvatureAnisotropicDiffusionImageFilter.html).  
Anisotropic denoising conserves information of edges. It doesn't oversmooth the details.
Using the script: https://github.com/phcerdan/ITKfilters/blob/master/scripts-python/denoise_input.py

It requires python itk packages.

```bash
python denoise_input.py ${OUTPUT_WAVELET} ${OUTPUT_FOLDER} ${DENOISE_ITER} ${DENOISE_CONDUCTANCE}
```

**Parameters:**  
* _t: timeStep: Defaults to 0.0625 for 3D images (for numerical stability)
* _N: iterations
* _c: conductance

**Results:**
1. Actin
```bash
export DENOISE_ITERATIONS=20
export DENOISE_CONDUCTANCE=2.0
export DENOISE_TIMESTEP=0.0625 #default_value
```
2. Pectin
3. Carrageenan
4. Collagen

```bash
export OUTPUT_DENOISE_STRING=${OUTPUT_WAVELET_STRING}\
_AnisDenoise_t${DENOISE_TIMESTEP}\
_N${DENOISE_ITERATIONS}\
_c${DENOISE_CONDUCTANCE}
export OUTPUT_DENOISE=${OUTPUT_FOLDER}/${OUTPUT_DENOISE_STRING}.${OUTPUT_EXTENSION}
```

## Binarization ##
### 3. Region Growing Binarization ###
Using [itk::ConnectedThresholdImageFilter](https://itk.org/Doxygen/html/classitk_1_1ConnectedThresholdImageFilter.html).  
With the script: https://github.com/phcerdan/ITKfilters/blob/master/scripts-cpp/regionGrowingSegmentation.cxx

```bash
regionGrowingSegmentation -i ${OUTPUT_DENOISE} -o ${OUTPUT_FOLDER} -l ${BIN_RG_LOWER} -u ${BIN_RG_UPPER} -p ${BIN_SAFE_PERCENTAGE}
```

**Parameters:**  
* _l: lowerThreshold: Requires user input/iteration
* _u: upperThreshold: Requires user input/iteration
* _p: safeBinaryPercentage: Create initial seeds based on a binary threshold

**Results:**
1. Actin
```bash
export BIN_RG_LOWER=0.0
export BIN_RG_UPPER=5000
export BIN_SAFE_PERCENTAGE=0.2
```
2. Pectin
3. Carrageenan
4. Collagen

```bash
export OUTPUT_BINARY_STRING=${OUTPUT_DENOISE_STRING}\
_SegmentRG_l${BIN_RG_LOWER}\
_u${BIN_RG_UPPER}\
_p${BIN_SAFE_PERCENTAGE}
export OUTPUT_BINARY=${OUTPUT_FOLDER}/${OUTPUT_BINARY_STRING}.${OUTPUT_EXTENSION}
```

### 4. Binary Hole Filling ###
Holes are created in the binarization process. These holes (areas of OFF pixels surrounded by ON pixels) have an important effect in the following skeletonization step because they change the topology of the network we want to extract. To solve it, we aggresively fill these holes.

Using [itk::VotingBinaryIterativeHoleFillingImageFilter](https://itk.org/Doxygen/html/classitk_1_1VotingBinaryIterativeHoleFillingImageFilter.html) with the script: https://github.com/phcerdan/ITKfilters/blob/master/scripts-python/binary_denoise_3d_fillholes_iterative.py

```bash

python binary_denoise_3d_fillholes_iterative.py ${OUTPUT_BINARY} ${OUTPUT_FOLDER} \
${HOLE_MAJORITY} ${HOLE_RADIUS} ${HOLE_ITERATIONS}
```

**Parameters:**  
* _M: Majority: Majority is the number of ON pixels in the neighborhood of an OFF pixel, to switch it into ON.
By default majority = 1, this means that an off pixel will be turned on if in the neighborhood (set by radius) there are at least 50% + 1 pixels ON.
If radius = 1,1,1, neighborhood size will be 3x3 = 9 pixels.
if 5 pixels around an OFF pixel are ON, then it will be switched.
* _R: Neighbour Radius. Number of neighbors to check. R=1 in 3D is a 3x3x3 neighborhood.
* _N: Iterations: Number of iterations before stopping, the algorithms also stop if no changes are generated after last iteration.

Recommended: Majority=3, Radius=1, Iterations = infinity

**Results:**
1. Actin
```bash
export HOLE_MAJORITY=3
export HOLE_RADIUS=1
export HOLE_ITERATIONS=10000
```
2. Pectin
3. Carrageenan
4. Collagen

```bash
export OUTPUT_HOLE_STRING=${OUTPUT_BINARY_STRING}\
_Hole_M${HOLE_MAJORITY}_R${HOLE_RADIUS}_N${HOLE_ITERATIONS}
export OUTPUT_HOLE=${OUTPUT_FOLDER}/${OUTPUT_HOLE_STRING}.${OUTPUT_EXTENSION}
```

## Skeletonization ##
### 5. Isthmus based asymmetric thinning ###
Using a recent contribution to DGtal: [PR](https://github.com/DGtal-team/DGtal/pull/1147), or [branch](https://github.com/phcerdan/DGtal/tree/voxelcomplex-with-object-graph) based on the work from [Asymmetric parallel 3D thinning scheme and algorithms based on isthmuses](https://hal.archives-ouvertes.fr/hal-01217974/document).

Using the c++ script `thin` from https://github.com/phcerdan/object_to_spatial_graph

```bash
export SKEL_SELECT=dmax
export SKEL_TYPE=1isthmus
export SKEL_PERSISTENCE=2
thin --input ${OUTPUT_SEGMENTATION} -o ${OUTPUT_FOLDER} \
  --select ${SKEL_SELECT} --skel ${SKEL_TYPE} \
  -p ${PERSISTENCE} --foreground white
```

**Parameters:**  
* _Skel: Select what voxels to keep in the skeleton.

Options: `{1isthmus, isthmus, end, ulti}`

*1isthmus*: Keeps the one isthmuses.

*isthmus*: Keeps the 1 and 2 isthmuses.

*end*: Keeps the end points.

*ulti*: Ultimate skeleton, it just conserves topology, but doesn't keep any other property. Any solid object without holes (sphere, lines, etc) will be reduced to a single point.

* _Select: Due to the asymmetric nature of the algorithm, it has to choose from a set of voxels, this parameter control the way or the order of the selection.

Options: `{dmax, first, random}`

*dmax*: Use a distance map, to choose the voxel with highest value, meaning that it will select a voxel in the median axis, ie, centered.

*first*: the first voxel ordered in a lexicographical way based on its position.

* _Persistence: Persitence parameter, a way to trim short and spurious branches.

Recommended: Skel=1isthmus, Select=dmax, Persistence=2

**Results:**
1. Actin
```bash
export SKEL_TYPE=1isthmus
export SKEL_SELECT=dmax
export SKEL_PERSISTENCE=2
```
2. Pectin
3. Carrageenan
4. Collagen

```bash
export OUTPUT_SKEL_STRING=${OUTPUT_HOLE_STRING}\
_SKEL_${SKEL_SELECT}_${SKEL_TYPE}_p${SKEL_PERSISTENCE}
export OUTPUT_SKEL=${OUTPUT_FOLDER}/${OUTPUT_SKEL_STRING}.${OUTPUT_EXTENSION}
```

## Spatial Graph ##
### 6. Graph from Skeleton ###
Using in-house developed open source c++ scripts from https://github.com/phcerdan/object_to_spatial_graph.

In particular `analyze_graph`.
The input to this script is a skeletonized image (for example after use of `thin`, and exporting the resulting skeleton into an image).

The input image is converted to a `DGtal::Object`, which can be treated as a `boost::graph`. Each pixel in the image corresponds to a vertex, and depending on the topology selected (we use the maximal connectivity for 3D: `DT26_6`), every pixel is connected to the neighbor pixels through an edge.

We reduce this raw graph into a `SpatialGraph`, where vertices of degree 2, those belonging to chains or branches are not treated as vertices, but as `edge_points` of `SpatialEdges`, connecting vertices with degree different than 2. The rest of vertices are kept as `SpatialNodes`, just adding its spatial position in the image.

The transfrom from the raw Object graph to the more useful SpatialGraph uses a `depth first search visit`, starting from end-points vertices (degree=1), it follows the connected neighbors (chain-nodes of degree 2) until it reaches another end-point or another vertex with degree 3 or more. When this happen, two SpatialNodes are added to the new SpatialGraph (the source where we started and the end node), and one SpatialEdge, containing all the visited 2-degree pixels.

We keep visiting the Object graph with the DFS algorithm, where the visited nodes are marked to avoid re-visiting them. After one-degree nodes, we start with any non-visited junction nodes (degree > 2). And to finish, we search for any self-loops, circular structures where all nodes have degree 2.
Because the graph doesn't allow self-loops, edges where source and target are the same node, we encode the self-loop into two nodes, the first is the original vertex, and the second one is created in the median of the SpatialEdge. So these self loops are two nodes with two parallel edges of about the same number of `edge_points` between them.

The DFS algorithms is quite fast thanks to its marking of already visited nodes.
There are two filters that we can apply to the process that have an impact in the generated SpatialGraph. 

First, we have the option `--removeExtraEdges` or `-c`, using [SG::remove_extra_edges](https://github.com/phcerdan/object_to_spatial_graph/blob/master/src/include/remove_extra_edges.hpp). This option is applied before the dfs reduction has been performed.

```cpp
/**
 * Objects in DGtal with a 26_6 topology consider as adjacent vertices those
 * in diagonals. We are interested in keeping that topology, but it generates
 * spurious edges that confuses further analysis.
 * We remove the "diagonal" edges when there are shorter ones. Better an example:
 *
 * o                  o
 * |\                 |
 * o-o      ----->    o-o
 * |/                 |
 * o                  o
 *
 * We are not interested in the diagonal edges, and better keep the center
 * node as the only 3-degree node.
 *
 * See related tests for further details.
 *
 * @param sg input spatial graph to reduce.
 *
 * @return boolean, true if any edge has been removed
 * false otherwhise.
 */
```

Second, we have the option `--mergeThreeConnectedNodes` or `-m`, using [SG::merge_three_connected_nodes](https://github.com/phcerdan/object_to_spatial_graph/blob/master/src/include/merge_nodes.hpp). This option is applied after the dfs reduction, when there are populated SpatialEdges.

```cpp
/**
 * Some nodes that are connected between them could be merged.
 * Transforming three nodes with degree 3, to one node with degree 3
 * and adding the old nodes into the corresponding spatial edges.
 *
 * Note: if two of those nodes are connected between them with an
 * extra parallel edge, the merge doesn't happen, example:
 *
 *     (a)
 *      o--
 *(b) o/|  |
 *     \o--
 *     (c)
 *
 * See related tests for further details.
 *
 * @param sg input spatial graph to reduce.
 *
 * @return number of nodes merged/cleared.
 */
```

The output of the script `analyze_graph` is able to generate three items:

- The adjacency list representing the SpatialGraph, with the associated SpatialNodes and SpatialEdges in graphviz (.dot) format.

```
graph G {
0 [spatial_node="93 510 52"];
1 [spatial_node="95 509 52"];
2 [spatial_node="262 507 52"];
3 [spatial_node="266 506 49"];
4 [spatial_node="212 506 52"];
5 [spatial_node="222 507 44"];

...
0--1  [spatial_edge="[{94 510 52}]"];
2--3  [spatial_edge="[{263 506 51},{264 506 51},{265 506 50}]"];
4--5  [spatial_edge="[{213 507 51},{214 508 51},{215 509 50},{216 509 49},{216 510 48},
{217 511 47},{218 510 46},{218 510 45},{219 509 44},{220 508 44},{221 507 45}]"];
6--7  [spatial_edge="[{437 504 52}]"];
...
}
```

- The `graph_data` files, with all the selected [graph_properties](https://github.com/phcerdan/object_to_spatial_graph/blob/master/src/include/compute_graph_properties.hpp) computed for the graph. This is `.txt` file with `degrees`,`end_to_end_distances`, `contour_lengths`, `angles`, `cosines`.

```
# degrees
0 1 1 1 1 4 3 5 ...
# ete_distances
5.09901951359278449 5.83095189484530074 4.12310562561766059 10.09950493836207741 ...
# angles
1.91014729370219882 1.80846294383527728 1.66127049710297503 2.16303105171734655 ...
# cosines
-0.33287514296784121 -0.23543547789870936 -0.09035079029052515 -0.55821655598023012 ...
# contour_lengths
5.56047793231506748 6.24264068711928477 4.46410161513775439 13.85300667219901349 ...
```

- The `histograms` from the graph_data. With different number of bins, or width.

```
# degrees: L0:centers of bins, L1:counts, L2:breaks
       0.000000000        1.000000000        2.000000000        3.000000000
                 0              16867                641              46173
      -0.500000000        0.500000000        1.500000000        2.500000000
# ete_distances: L0:centers of bins, L1:counts, L2:breaks
       0.500000000        1.500000000        2.500000000        3.500000000
                 0              27783               9955              12460
       0.000000000        1.000000000        2.000000000        3.000000000
# angles: L0:centers of bins, L1:counts, L2:breaks
       0.062831853        0.188495559        0.314159265        0.439822972
               606                276                531                746
       0.000000000        0.125663706        0.251327412        0.376991118
# cosines: L0:centers of bins, L1:counts, L2:breaks
      -0.960000000       -0.880000000       -0.800000000       -0.720000000
             13984              10652              12139              11056
      -1.000000000       -0.920000000       -0.840000000       -0.760000000
# contour_lengths: L0:centers of bins, L1:counts, L2:breaks
       0.500000000        1.500000000        2.500000000        3.500000000
                 0              27168               7732               9120
       0.000000000        1.000000000        2.000000000        3.000000000
```

```bash
export GRAPH_IGNORE_SHORT_EDGES=1
export GRAPH_SPACING="5.05050745877353E-07 5.05050745877353E-07 1.00708103855232E-06"
analyze_graph --input ${OUTPUT_SKELETONIZATION} \
 --exportReducedGraph ${OUTPUT_FOLDER} --exportData ${GRAPH_DATA_FOLDER}
 --reduceGraph --removeExtraEdges  --mergeThreeConnectedEdges \
 --ignoreAngleBetweenParallelEdges \
 --ignoreEdgesShorterThan ${GRAPH_IGNORE_SHORT_EDGES} \
 --spacing ${GRAPH_SPACING}
```

### 7. Analyze Statistical Distributions from the Spatial Graph ###
With the `graph_properties` data and the associated `histograms` we can start comparing
the topology of graphs for different samples.
In this case we compare biopolymers from different scales and microscopy techniques.
From proteins using confocal microscopy (Actin and Collagen), to polysaccharides
using transmission electron microscopy --TEM-- (Pectin, Carrageenan).

We compare the statistical distributions of its graph properties, and check if there is any universality shared between these biopolymer networks.