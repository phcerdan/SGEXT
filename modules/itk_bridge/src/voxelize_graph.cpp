/* ********************************************************************
 * Copyright (C) 2020 Pablo Hernandez-Cerdan.
 *
 * This file is part of SGEXT: http://github.com/phcerdan/sgext.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * *******************************************************************/

#include "voxelize_graph.hpp"

#include <fstream>
#include <iostream>

namespace SG {

BinaryImageType::Pointer
voxelize_graph(const GraphType &graph,
               const BinaryImageType::Pointer &reference_image,
               const vertex_to_label_map_t &vertex_to_label_map,
               const edge_to_label_map_t &edge_to_label_map,
               const bool &graph_positions_are_in_physical_space) {
    BinaryImageType::Pointer voxelized_image = BinaryImageType::New();
    // Create empty image with same parameters than reference_image
    voxelized_image->SetRegions(reference_image->GetLargestPossibleRegion());
    voxelized_image->SetSpacing(reference_image->GetSpacing());
    voxelized_image->SetOrigin(reference_image->GetOrigin());
    voxelized_image->SetDirection(reference_image->GetDirection());
    voxelized_image->Allocate();
    voxelized_image->FillBuffer(0);

    // Flag to output warning about labels equal to zero will be lost in the
    // background.
    bool any_label_is_zero = false;

    GraphType::vertex_iterator vi, vi_end;
    std::tie(vi, vi_end) = boost::vertices(graph);
    for (; vi != vi_end; ++vi) {
        const auto vertex = *vi;
        // if found node in map, populate voxel with value
        const auto find_node_label = vertex_to_label_map.find(vertex);
        if (find_node_label != vertex_to_label_map.cend()) {
            const auto &label = find_node_label->second;
            if (label == 0) {
                any_label_is_zero = true;
            }
            const auto &position = graph[vertex].pos;
            const auto itk_index =
                    graph_position_to_image_index<BinaryImageType>(
                            position, reference_image,
                            graph_positions_are_in_physical_space);
            voxelized_image->SetPixel(itk_index, label);
        }
    }

    GraphType::edge_iterator ei, ei_end;
    std::tie(ei, ei_end) = boost::edges(graph);
    for (; ei != ei_end; ++ei) {
        const auto edge = *ei;
        // if found edge in map, populate voxel with value
        const auto find_edge_label = edge_to_label_map.find(edge);
        if (find_edge_label != edge_to_label_map.cend()) {
            const auto &label = find_edge_label->second;
            if (label == 0) {
                any_label_is_zero = true;
            }
            // Populate all edge points voxels with this label
            const auto &edge_points = graph[edge].edge_points;
            for (const auto &ep : edge_points) {
                const auto itk_index =
                        graph_position_to_image_index<BinaryImageType>(
                                ep, reference_image,
                                graph_positions_are_in_physical_space);
                voxelized_image->SetPixel(itk_index, label);
            }
        }
    }

    if (any_label_is_zero) {
        std::cerr << "Warning in voxelize_graph: the maps have one or more "
                     "labels equal to zero, these will be lost in the "
                     "background of the image. Ignore this warning if expected."
                  << std::endl;
    }

    return voxelized_image;
}

edge_to_label_map_t create_edge_to_label_map_from_vertex_to_label_map(
        const GraphType &graph,
        const vertex_to_label_map_t &vertex_to_label_map,
        const edge_label_function_t &edge_label_func) {
    auto edge_to_label_map = edge_to_label_map_t();
    GraphType::edge_iterator ei, ei_end;
    std::tie(ei, ei_end) = boost::edges(graph);
    for (; ei != ei_end; ++ei) {
        const auto edge = *ei;
        const auto source = boost::source(edge, graph);
        const auto target = boost::target(edge, graph);
        const auto find_source_label = vertex_to_label_map.find(source);
        const auto find_target_label = vertex_to_label_map.find(target);
        // Only populate edge label if source and target label exists.
        if (find_source_label != vertex_to_label_map.cend() &&
            find_target_label != vertex_to_label_map.cend()) {
            const auto source_label = find_source_label->second;
            const auto target_label = find_target_label->second;
            const auto edge_label = edge_label_func(source_label, target_label);
            edge_to_label_map.emplace(edge, edge_label);
        }
    }
    return edge_to_label_map;
}

edge_to_label_map_t create_edge_to_label_map_from_vertex_to_label_map_using_max(
        const GraphType &graph,
        const vertex_to_label_map_t &vertex_to_label_map) {
    const auto edge_function = [](const size_t &source_label,
                                  const size_t &target_label) {
        return std::max<size_t>(source_label, target_label);
    };
    return create_edge_to_label_map_from_vertex_to_label_map(
            graph, vertex_to_label_map, edge_function);
}

} // end namespace SG
