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

#include "tree_generation.hpp"
#include "create_vertex_to_radius_map.hpp"
#include "tree_generation_visitor.hpp"
#include "filter_spatial_graph.hpp"
#include <fstream>
#include <iostream>

namespace SG {

VertexGenerationMap
tree_generation(const GraphType &graph,
                const typename FloatImageType::Pointer &distance_map_image,
                const bool spatial_nodes_position_are_in_physical_space,
                const double &decrease_radius_ratio_to_increase_generation,
                const double &keep_generation_if_angle_less_than,
                const double &increase_generation_if_angle_greater_than,
                const size_t &num_of_edge_points_to_compute_angle,
                const std::vector<GraphType::vertex_descriptor> &input_roots,
                const VertexGenerationMap &input_fixed_generation_map,
                const bool verbose) {
    using vertex_descriptor = GraphType::vertex_descriptor;
    // Start the visit at the root
    // As a first approximation, we select as root the vertex with largest
    // radius
    const auto vertex_to_radius_map = create_vertex_to_radius_map(
            distance_map_image, graph,
            spatial_nodes_position_are_in_physical_space, verbose);
    // Manage roots
    if(verbose) {
        std::cout << "Provided input_roots: [ ";
        for (const auto & root : input_roots) {
            std::cout << root << ", " << std::endl;
        }
        std::cout << "]" << std::endl;
    }

    std::vector<vertex_descriptor> final_root_nodes;
    if(input_roots.empty()) {
        const auto component_graphs = SG::filter_component_graphs(graph);
        const auto num_of_components = component_graphs.size();
        if(verbose) {
            std::cout << "vertices_with_largest_radius per graph component "
                "(graph componentes where largest radius is 1 are ignored):" << std::endl;
        }
        for (size_t comp_index = 0; comp_index < num_of_components; comp_index++) {
            const auto & comp_graph  = component_graphs[comp_index];
            const auto comp_vertex_to_radius_map = create_vertex_to_radius_map(
                    distance_map_image, comp_graph,
                    spatial_nodes_position_are_in_physical_space, verbose);
            using vertex_radius_pair = std::pair<GraphType::vertex_descriptor, double>;
            const auto max_radius_element = std::max_element(
                    std::cbegin(comp_vertex_to_radius_map), std::cend(comp_vertex_to_radius_map),
                    [](const vertex_radius_pair &lhs, const vertex_radius_pair &rhs) {
                        return lhs.second < rhs.second;
                    });

            const auto vertex_with_largest_radius = max_radius_element->first;
            const auto & largest_radius_per_component = max_radius_element->second;
            if(largest_radius_per_component > 1.0) {
                final_root_nodes.push_back(vertex_with_largest_radius);
                if (verbose) {
                    std::cout << " - component_index: " << comp_index
                        << " -> vertex_with_largest_radius: "
                        << vertex_with_largest_radius
                        << " with radius: "
                        << largest_radius_per_component
                        << std::endl;
                }
            }
        }
    } else {
        // TODO: If multiple roots, we should check that they belong to
        // disconnected_components. See SG::filter_component_graph O(E+V)
        final_root_nodes = input_roots;
    }

    VertexGenerationMap vertex_to_generation_map;
    std::unordered_map<vertex_descriptor, size_t>
        vertex_to_distance_from_root_map;
    const auto total_vertices = boost::num_vertices(graph);
    for(const auto & root : final_root_nodes) {
        // Check root exists in graph
        if(root > total_vertices - 1) {
            throw std::runtime_error("Root '" + std::to_string(root) +
                    "' is not in the graph.");
        }

        // Set the generation of root to one (not zero to help with voxelization)
        vertex_to_generation_map.emplace(root, 1);
        // Set the distance from root to zero in the root node.
        vertex_to_distance_from_root_map.emplace(root, 0);
    }

    // Populate the output with the provided input_fixed_generation_map to fix
    // those generations.
    if(verbose) {
        std::cout << "input_fix_generation_map:" << std::endl;
    }
    for (const auto &input_fix_generation : input_fixed_generation_map) {
        vertex_to_generation_map.emplace(input_fix_generation.first,
                input_fix_generation.second);
        if(verbose) {
            std::cout <<  input_fix_generation.first << ": " <<
                          input_fix_generation.second << std::endl;
        }
    }
    // Store anomalies, targets with bigger radius than its source.
    TreeGenerationVisitor<GraphType>::VertexAnomalies vertex_anomalies;
    // Start the visit from root
    TreeGenerationVisitor<GraphType> visitor(
            vertex_to_generation_map, distance_map_image, vertex_to_radius_map,
            vertex_to_distance_from_root_map,
            decrease_radius_ratio_to_increase_generation,
            keep_generation_if_angle_less_than,
            increase_generation_if_angle_greater_than,
            num_of_edge_points_to_compute_angle,
            spatial_nodes_position_are_in_physical_space, vertex_anomalies,
            verbose);

    using ColorMap = std::map<vertex_descriptor, boost::default_color_type>;
    ColorMap colorMap;
    boost::associative_property_map<ColorMap> propColorMap(colorMap);
    boost::queue<vertex_descriptor> Q; // buffer for bfs
    for(const auto & root: final_root_nodes) {
        if(verbose) {
            std::cout << "Start visit for component with root: " <<
                root << std::endl;
        }
        boost::breadth_first_visit(graph, root, Q, visitor, propColorMap);
    }

    // Print anomalies
    if (verbose) {
        std::cout << "Anomalies:" << std::endl;
        std::cout << "vertex : radius" << std::endl;
        for (const auto &vertex_anomaly : visitor.m_vertex_anomalies) {
            std::cout << vertex_anomaly.first << ": " << vertex_anomaly.second
                      << std::endl;
        }
        std::cout << "Anomalies vertex list:" << std::endl;
        for (const auto &vertex_anomaly : visitor.m_vertex_anomalies) {
            std::cout << vertex_anomaly.first << ", ";
        }
        std::cout << std::endl;
    }
    return vertex_to_generation_map;
}

VertexGenerationMap read_vertex_to_generation_map(
        const std::string &input_fixed_generation_map_file) {
    std::vector<size_t> vertex_ids;
    std::vector<size_t> generations;
    VertexGenerationMap output;
    // Create an input filestream
    std::ifstream my_file(input_fixed_generation_map_file);
    // Make sure the file is open
    if (!my_file.is_open()) {
        throw std::runtime_error("Could not open file");
    }
    // Helper vars
    std::string line, colname;
    int val;
    // Skip the first line (header).
    if (my_file.good()) {
        std::getline(my_file, line);
    }

    // Read data, line by line
    while (std::getline(my_file, line)) {
        // Create a stringstream of the current line
        std::stringstream ss(line);

        // Keep track of the current column index
        int colIdx = 0;

        // Extract each integer
        while (ss >> val) {
            // At the time we find a #, skip this line
            if (val == '#') {
                break;
            }
            if (colIdx == 0) {
                vertex_ids.push_back(val);
            } else if (colIdx == 1) {
                generations.push_back(val);
            }
            // If the next token is a comma, ignore it and move on
            if (ss.peek() == ',') {
                ss.ignore();
            }

            // Increment the column index
            colIdx++;
        }
    }
    if (vertex_ids.size() != generations.size()) {
        throw std::runtime_error(
                R"("read_vertex_to_generation_map: Failure.
Not the same number of vertex_ids and generations. The file is not populated correctly. #vertex_ids: )" +
                std::to_string(vertex_ids.size()) +
                " | #generations: " + std::to_string(generations.size()));
    }
    for (size_t i = 0; i < vertex_ids.size(); ++i) {
        output.emplace(vertex_ids[i], generations[i]);
    }
    return output;
}

void write_vertex_to_generation_map(
        const VertexGenerationMap &vertex_to_generation_map,
        const std::string &output_filename) {

    // Create an output filestream object
    std::ofstream myFile(output_filename);
    // Populate the header
    myFile << "# vertex_id , generation\n";
    // Write the map
    for (const auto &vertex_generation_pair : vertex_to_generation_map) {
        myFile << vertex_generation_pair.first << ", "
               << vertex_generation_pair.second << "\n";
    }
}

} // end namespace SG
