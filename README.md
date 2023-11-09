[![Build Status](https://dev.azure.com/phcerdan/SGEXT/_apis/build/status/phcerdan.SGEXT?branchName=master)](https://dev.azure.com/phcerdan/SGEXT/_build/latest?definitionId=2&branchName=master)

# SGEXT

**S**patial**G**raph **Ext**ractor.

Library with utilities to handle graphs extracted from binary images.

Provides scripts to generate a thin/skeletonized image from binary images (segmentations).
A thin image is a one pixel wide image, conserving the same topology (same number of holes, and shapes) than the input binary image.

A distance map can be used for the thin image to be in the centerline of the input.

The thinning algorithm used was contributed by the author to the DGtal library, based on the work of Couprie and Bertrand [1]

The thin output can also be converted to a Spatial Graph, this is a regular graph, an adjacency list holding the nodes and edges, plus all the geometrical information. In the case of a nodes/vertices, a spatial node with 3D location. In edges, a spatial edge, a data structure with a consecutive list of points connecting the nodes.

Using histo.hpp from: https://github.com/phcerdan/histo-header
SHA: 556ada3ff79c0180a0cbec36ff29a30da5acb367

## Python

SGEXT is wrapped to python using pybind11, and uploaded regularly to pypi for all platforms (Linux, Windows, MacOS)
and multiple `python` version (from `3.5` to latest) using azure-pipelines.

```
pip install sgext
```

```python
import sgext
# Read image from file into a sgext_image
input_filename="/path/to/binary_image.nrrd" # or any format that ITK can read
sgext_image = sgext.itk.read_as_binary(input_filename)
# Or from a numpy array:
sgext_image = sgext.itk.IUC3P()
sgext_image.from_pyarray(mask)
# Or convert from an existing ITK image via the numpy bridge:
sgext_image = sgext.itk.IUC3P()
sgext_image.from_pyarray(itk.GetArrayFromImage(itk_image))
thin_image = sgext.scripts.thin(input=sgext_image,
                   tables_folder= sgext.tables_folder,
                   skel_type="end",
                   select_type="first",
                   persistence=2,
                   visualize=False,
                   verbose=True
                   )
thin_filename ="/path/to/thin_image.nrrd"
sgext.itk.write(thin_image, thin_filename)
```

## Usage
The C++ scripts are in folder `cpp-scripts`.
The inputImage to these scripts is a label/binary image or sometimes a float grayscale image.
All the scripts provide a --help or -h option for guidance.

### Distance Map
Create a distance map using DGtal most precise way with Lp metric. The output is a float/double image with pixels storing the distance to the background (heavy image).
```bash
create_distance_map \
    -i inputImage.nrrd \
    -o outputFolder \
    -v # verbose flag --recommended--
```

In verbose mode, the output would be:
```bash
New Block [Create Distance Map]
EndBlock [Create Distance Map] (108686 ms)
Time elapsed: 108
```

the outputFolder will be populated with `inputImage_DMAP.nrrd` (heavy image).

### Thinning
Using VoxelComplex in DGtal, based on Bertrand and Couprie research in digital topology. Ensures topology consistency,
and implements a way to perform a prunning on the branches based on local information.
```bash
thin \
    -i inputImage.nrrd \
    -o outputFolder \
    -s 1isthmus \
    -c dmax \ # Requires distance map, ensures centrality of the thinning
    -d distanceMapImage.nrrd \
#    -p 2 \ Optional persistence, useful for noisy images, default to 0.
    -v # verbose flag --recommended--
```

### Get radius of vesselnes
The distance map can also be used as a really good approximation to vesselnes radius. In order to get this information
for our skeletonized image we can use the script mask_distance_map_with_thin_image

```bash
mask_distance_map_with_thin_image \
    -i inputDistanceMapImage.nrrd \
    -m inputSkeletonizedImage.nrrd \
    -o outputFolder \
    -v # verbose flag --recommended--
```

This will generate an image `skeletonizedImage_DMAP_MASKED` in the output folder

### Graph processing
To convert the skeletonized image into a graph we used boost graph. The following script

```bash
analyze_graph \
    -i inputSkeletonizedImage.nrrd
    -o outputFolder \ # This will generate a .dot file with all the graph information
    -r \ # reduce graph, convert chain nodes (degree 2) into edge points.
    -c \ # removeExtraEdges (remove edges created because full connectivity)
    -m \ # mergeThreeConnectedNodes
    -v
```
More options are possible, use `--help` for details.


## Contributors

- Pablo Hernandez-Cerdan

[1]: Couprie and Bertrand, “Asymmetric Parallel 3D Thinning Scheme and Algorithms Based on Isthmuses.” Pattern Recognition Letters. June,  2016. DOI:10.1016/j.patrec.2015.03.014
