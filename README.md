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

## [Azure Pipelines](https://dev.azure.com/phcerdan/SGEXT)

## Docker
First build the container with all the dependencies, [Dockerfile-base](https://github.com/phcerdan/SGEXT/blob/master/Dockerfile-base).

```bash
docker build -f Dockerfile-base -t sgext/base .
```

Then use the [Dockerfile](https://github.com/phcerdan/SGEXT/blob/master/Dockerfile) that copies your local source folder with this repository into the container.

```bash
docker build -f Dockerfile -t sgext/scripts .
```

You can then test it with `docker run sgext/scripts thin --help`

- To mount a data folder inside the container, run `docker run -v <data-dir>:/data sgext/base ls /data`
- The `-v` option binds `<data-dir>` on your machine to `/data` inside the docker container. Please read this [page](https://docs.docker.com/storage/bind-mounts/) for more details on bind mounts.

```bash
`docker run -v <data-dir>:/data sgext/scripts thin --help`
```

## Build
Build ITK
Build DGtal, with a version with this patch included: (1.0 should be enough)
https://github.com/DGtal-team/DGtal/pull/1369

```bash
cd your_favourite_folder
mkdir DGtal; cd DGtal
git clone https://github.com/DGtal-team/DGtal src
#(optional): cd src; hub checkout https://github.com/DGtal-team/DGtal
mkdir build; cd build;
cmake -DBUILD_TYPE:STRING=Release -DBUILD_EXAMPLES:BOOL=OFF -DBUILD_TESTING:BOOL=OFF -DWITH_ITK:BOOL=ON -DITK_DIR:PATH=your_path_to_build_dir_ITK ../src
```

I do have scripts doing the thinning (and post-processing graph analysis) here:
https://github.com/phcerdan/SGEXT

They add a boost dependency.

So install boost (any non-extremely old version should be ok), using package manager for example.
And build the scripts

```
cd your_favourite_folder
mkdir SGEXT; cd SGEXT
git clone https://github.com/phcerdan/SGEXT src
mkdir build; cd build
cmake -DBUILD_TYPE:STRING=Release -DDGtal_DIR:PATH=/path/DGtal/build -DITK_DIR:PATH=your_path_to_build_dir_ITK ../src
# if Boost is not the system path, also -DBoost_DIR:PATH=...
```


## Usage
The scripts are in folder `cpp-scripts`
the inputImage to these scripts is a label/binary image.
all the scripts provide a --help or -h option for guidance.

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
