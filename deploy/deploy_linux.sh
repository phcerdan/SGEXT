#!/usr/bin/env bash

script_dir=$(cd $(dirname $0) || exit 1; pwd)

pushd $script_dir/..
mkdir -p $script_dir/dist
docker build -f ./deploy/docker/Dockerfile-dockcross-manylinux2014-wheel . -t phcerdan/sgext-linux-wheel
docker cp $(docker create phcerdan/wolfram_model-linux:latest):/work/dist $script_dir/dist
popd
