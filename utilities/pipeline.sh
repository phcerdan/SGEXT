#!/usr/bin/env bash
# DATA_FOLDER=~/Data
# INPUT_LABEL=file_name_without_extension
# BINARY_FOLDER=~/repository_local/object_to_spatial_graph/build-debug/cpp-scripts
# Z option disabled the verbose output of the filenames
# Fill holes
# Compute Distance Map
# Thin with DMAP, 1isthmus and 0 persistence
# Create a serialized graph
$BINARY_FOLDER/fill_holes_binary -i $DATA_FOLDER/${INPUT_LABEL}.nrrd -o $DATA_FOLDER/results -v -z && \
$BINARY_FOLDER/create_distance_map -i $DATA_FOLDER/results/${INPUT_LABEL}_FILLED.nrrd -o $DATA_FOLDER/results -v && \
$BINARY_FOLDER/thin -i $DATA_FOLDER/results/${INPUT_LABEL}_FILLED.nrrd -o $DATA_FOLDER/results \
    -s 1isthmus -c dmax -d ${DATA_FOLDER}/results/${INPUT_LABEL}_FILLED_DMAP.nrrd -v -z && \
$BINARY_FOLDER/analyze_graph -i $DATA_FOLDER/results/${INPUT_LABEL}_FILLED_SKEL.nrrd -o $DATA_FOLDER/results -u $DATA_FOLDER/results \
   --reduceGraph --removeExtraEdges -m -v -z


