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

input_filename="/path/to/binary_image.nrrd" # or any format that ITK can read
out_folder="/path/to/output_folder" # or any format that ITK can read

sgext.scripts.thin(input=input_filename,
        out_folder=out_folder,
        foreground="black",
        skel_type="end",
        select_type="first",
        persistence=2,
        visualize=False,
        verbose=True)
"

## Build dependencies
This project depends on Boost, DGtal, and optionally VTK and ITK. Also TBB is used for the parallelSTL if c++17 is available.

To handle all these depedencies, a subproject in `./dependencies` has been set up. Using `CMake` `ExternalProject` to download and build
all the dependencies.

```bash
mkdir build-sgext-dependencies; cd build-sgext-dependencies;
cmake ../SGEXT-src/dependencies -DOUTPUT_BUILD_DIR="." -DWITH_TBB:BOOL=OFF -DNUM_CORES=12
```

## Build

Then pass the selected `OUTPUT_BUILD_DIR` to SGEXT with the option `-DDEPENDENCIES_BUILD_DIR`
```bash
mkdir build-sgext; cd build-sgext
cmake ../SGEXT-src -DCMAKE_BUILD_TYPE=RelWithDebInfo -DDEPENDENCIES_BUILD_DIR="../build-sgext-dependencies"
```

Or you can also pass each depedency independently:
- `-DBOOST_ROOT=/path/to/boost` or `-DBOOST_DIR=/path/to/boost/lib/cmake/Boost-x.y.z`
- `-DDGtal_DIR=/path/to/DGtal-build` or `/path/to/DGtal/lib/cmake`
- `-DWITH_VTK:BOOL=ON` `-DVTK_DIR:FILEPATH=/path/to/VTK-build` or `/path/to/VTK/lib/cmake`
- `-DWITH_ITK:BOOL=ON` `-DITK_DIR:FILEPATH=/path/to/ITK-build` or `/path/to/ITK/lib/cmake`

```
cmake -DBUILD_TYPE:STRING=Release -DWITH_ITK:BOOL=ON -DITK_DIR:PATH=your_path_to_build_dir_ITK ../src
```

## Docker
First build the container with all the dependencies, [Dockerfile-base](https://github.com/phcerdan/SGEXT/blob/master/Dockerfile-base).

```bash
cd SGEXT-src
docker build -f ./dependencies/docker/Dockerfile-base -t sgext/base .
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

## Dockcross (manylinux2014)

Build base (dependencies) first:
```bash
cd SGEXT-src
docker build -f ./dependencies/docker/Dockerfile-dockcross-manylinux2014-base . -t phcerdan/sgext-linux-base
```
Dockerfile using dockcross to compile SGEXT:
```bash
docker build -f ./deploy/docker/Dockerfile-dockcross-manylinux2014 . -t phcerdan/sgext-linux-base
```

Dockerfile using dockcross, scikit-build and auditwheel to create python wheels:

```bash
docker build -f ./deploy/docker/Dockerfile-dockcross-manylinux2014-wheel . -t phcerdan/sgext-linux-wheel
```

To copy the wheels generated after auditwheel from the image:
```bash
docker cp $(docker create phcerdan/sgext-linux-wheel):/work/SGEXT-src/deploy/wheelhouse/ /tmp
```

And to upload them to pypi (showing test.pypi)
```bash
python3 -m twine upload --repository-url https://test.pypi.org/legacy/ /tmp/wheelhouse/* --verbose
```


These images have also been uploaded to [docker-hub](https://hub.docker.com/repository/docker/phcerdan/sgext-linux-base)
```bash
docker pull phcerdan/sgext-linux-base
docker pull phcerdan/sgext-linux
docker pull phcerdan/sgext-linux-wheel
```

### Azure pipelines

- [Azure Pipelines](https://dev.azure.com/phcerdan/SGEXT)

```bash
workon sgext-build
pip install azure-cli
az extension add --name azure-devops
```

Configure a [personal access tokens](https://docs.microsoft.com/en-us/azure/devops/organizations/accounts/use-personal-access-tokens-to-authenticate?view=azure-devops&viewFallbackFrom=vsts&tabs=preview-page#create-personal-access-tokens-to-authenticate-access)
```bash
az devops login
```

Get the buildId from the URL of the build: `https://dev.azure.com/phcerdan/SGEXT/_build/results?buildId=181`

And use the script `./deploy/scripts/download_azure_artifacts.sh $build_id` to download all the wheels (defaults to `/tmp/dist`)

## Contributors

- Pablo Hernandez-Cerdan

[1]: Couprie and Bertrand, “Asymmetric Parallel 3D Thinning Scheme and Algorithms Based on Isthmuses.” Pattern Recognition Letters. June,  2016. DOI:10.1016/j.patrec.2015.03.014
