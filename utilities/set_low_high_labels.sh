#!/usr/bin/env bash
echo "BINARY_FOLDER=${BINARY_FOLDER}"
export LOW_GRAPH_LABEL=$(echo "$INPUT_LABEL" | sed 's/high/low/g'); echo "LOW_GRAPH_LABEL: $LOW_GRAPH_LABEL"
export HIGH_GRAPH_LABEL=$(echo "$INPUT_LABEL" | sed 's/low/high/g'); echo "HIGH_GRAPH_LABEL: $HIGH_GRAPH_LABEL"
