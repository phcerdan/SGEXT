#!/usr/bin/env bash
echo "DATA_FOLDER=${DATA_FOLDER}"
echo "INPUT_LABEL=${INPUT_LABEL}"
echo "BINARY_FOLDER=${BINARY_FOLDER}"

echo "VISUALIZE low" && \
$BINARY_FOLDER/visualize_graph_and_image -s $DATA_FOLDER/results/${INPUT_LABEL}_FILLED_SKEL.nrrd \
-g $DATA_FOLDER/results/${INPUT_LABEL}_FILLED_SKEL_REDUCED.txt -u & \
echo "VISUALIZE high" && \
INPUT_LABEL=$(echo "$INPUT_LABEL" | sed 's/low/high/g'); echo $INPUT_LABEL && \
$BINARY_FOLDER/visualize_graph_and_image -s $DATA_FOLDER/results/${INPUT_LABEL}_FILLED_SKEL.nrrd \
-g $DATA_FOLDER/results/${INPUT_LABEL}_FILLED_SKEL_REDUCED.txt -u &

