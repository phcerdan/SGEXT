#!/usr/bin/env bash
echo "DATA_FOLDER=${DATA_FOLDER}"
# INPUT_LABEL should be set to the high graph!
echo "INPUT_LABEL=${INPUT_LABEL}"
echo "BINARY_FOLDER=${BINARY_FOLDER}"
export LOW_GRAPH_LABEL=$(echo "$INPUT_LABEL" | sed 's/high/low/g'); echo "LOW_GRAPH_LABEL: $LOW_GRAPH_LABEL"
export HIGH_GRAPH_LABEL=$(echo "$INPUT_LABEL" | sed 's/low/high/g'); echo "HIGH_GRAPH_LABEL: $HIGH_GRAPH_LABEL"

echo "VISUALIZE high" && \
$BINARY_FOLDER/visualize_graph_and_image -s $DATA_FOLDER/results/${HIGH_GRAPH_LABEL}_FILLED_SKEL.nrrd \
-g $DATA_FOLDER/results/${HIGH_GRAPH_LABEL}_FILLED_SKEL_REDUCED.txt -u & \
echo "VISUALIZE low" && \
$BINARY_FOLDER/visualize_graph_and_image -s $DATA_FOLDER/results/${LOW_GRAPH_LABEL}_FILLED_SKEL.nrrd \
-g $DATA_FOLDER/results/${LOW_GRAPH_LABEL}_FILLED_SKEL_REDUCED.txt -u &
merged_graph=$DATA_FOLDER/results/${HIGH_GRAPH_LABEL}_FILLED_SKEL_REDUCED_MERGED.txt
if [ -f ${merged_graph} ]; then
   echo "VISUALIZE merged" && \
      $BINARY_FOLDER/visualize_graph_and_image -s $DATA_FOLDER/results/${LOW_GRAPH_LABEL}_FILLED_SKEL.nrrd \
      -g ${merged_graph} -u
fi

