# SGEXT: CONTRIBUTING

### Get autocompletion for members with `rlcompleter`
IPython can autocomplete modules and function wrapped with pybind11 but not members. See [issue](https://github.com/pybind/pybind11/issues/945).
We can use `rlcompleter` in Unix-like enviroments to help ipython with this:
```python
try:
    # Get TAB autocompletion in Unix-like OS's.
    import rlcompleter
except ImportError:
    pass
```

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
cmake ../SGEXT-src -DCMAKE_BUILD_TYPE=RelWithDebInfo -DDEPENDENCIES_BUILD_DIR="../build-sgext-dependencies" -DBoost_USE_STATIC_LIBS=ON
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
