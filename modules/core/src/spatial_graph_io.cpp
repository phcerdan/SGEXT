/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/graph/adj_list_serialize.hpp>
#include <boost/graph/graphviz.hpp>

#include "spatial_graph_io.hpp"

namespace SG {

boost::dynamic_properties get_write_dynamic_properties_sg(GraphType &graph) {
    boost::dynamic_properties dp;
    dp.property("node_id", boost::get(boost::vertex_index, graph));
    dp.property("spatial_node", boost::get(boost::vertex_bundle, graph));
    dp.property("spatial_edge", boost::get(boost::edge_bundle, graph));
    return dp;
}
boost::dynamic_properties get_read_dynamic_properties_sg(GraphType &graph) {
    boost::dynamic_properties dp;
    dp.property("node_id", boost::get(&SpatialNode::id, graph));
    dp.property("spatial_node", boost::get(boost::vertex_bundle, graph));
    dp.property("spatial_edge", boost::get(boost::edge_bundle, graph));
    return dp;
}

void write_graphviz_sg(std::ostream &os, GraphType &graph) {
    auto dp = get_write_dynamic_properties_sg(graph);
    boost::write_graphviz_dp(os, graph, dp);
}
void write_graphviz_sg(const std::string &output_file, GraphType &graph) {
    auto dp = get_write_dynamic_properties_sg(graph);
    std::ofstream ofile(output_file);
    boost::write_graphviz_dp(ofile, graph, dp);
}

void read_graphviz_sg(std::istream &is, GraphType &graph) {
    auto dp = get_read_dynamic_properties_sg(graph);
    boost::read_graphviz(is, graph, dp);
}
void read_graphviz_sg(const std::string &input_file, GraphType &graph) {
    auto dp = get_read_dynamic_properties_sg(graph);
    std::ifstream ifile(input_file);
    if(!ifile.is_open()) {
        throw std::runtime_error("Failed to read input_file: " + input_file + ".");
    }
    boost::read_graphviz(ifile, graph, dp);
}

GraphType read_graphviz_sg(const std::string &input_file) {
    GraphType graph;
    read_graphviz_sg(input_file, graph);
    return graph;
}

// Serialize
void write_serialized_sg(std::ostream &os, const GraphType &graph) {
    boost::archive::text_oarchive arch(os);
    arch << graph;
}
void write_serialized_sg(const std::string &output_file, const GraphType &graph) {
    std::ofstream os(output_file);
    return write_serialized_sg(os, graph);
}

void read_serialized_sg(std::istream &is, GraphType &graph) {
    boost::archive::text_iarchive arch(is);
    arch >> graph;
}
void read_serialized_sg(const std::string &input_file, GraphType &graph) {
    std::ifstream ifile(input_file);
    if(!ifile.is_open()) {
        throw std::runtime_error("Failed to read input_file: " + input_file + ".");
    }
    read_serialized_sg(ifile, graph);
}

GraphType read_serialized_sg(const std::string &input_file) {
    GraphType graph;
    read_serialized_sg(input_file, graph);
    return graph;
}

vertex_to_label_map_t
read_vertex_to_label_map(const std::string &vertex_to_label_map_file) {
    std::vector<size_t> vertex_ids;
    std::vector<size_t> labels;
    vertex_to_label_map_t output;
    // Create an input filestream
    std::ifstream my_file(vertex_to_label_map_file);
    // Make sure the file is open
    if (!my_file.is_open())
        throw std::runtime_error("Could not open file.");
    // Helper vars
    std::string line;
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
            if (val == '#')
                break;
            if (colIdx == 0) {
                vertex_ids.push_back(val);
            } else if (colIdx == 1) {
                labels.push_back(val);
            }
            // If the next token is a comma, ignore it and move on
            if (ss.peek() == ',')
                ss.ignore();

            // Increment the column index
            colIdx++;
        }
    }
    if (vertex_ids.size() != labels.size()) {
        throw std::runtime_error(
                R"("read_vertex_to_label_map: Failure.
Not the same number of vertex_ids and labels. The file is not populated correctly. #vertex_ids: )" +
                std::to_string(vertex_ids.size()) +
                " | #labels: " + std::to_string(labels.size()));
    }
    for (size_t i = 0; i < vertex_ids.size(); ++i) {
        output.emplace(vertex_ids[i], labels[i]);
    }
    return output;
}

void write_vertex_to_label_map(const vertex_to_label_map_t &vertex_to_label_map,
                               const std::string &output_filename) {

    // Create an output filestream object
    std::ofstream my_file(output_filename);
    // Populate the header
    my_file << "# vertex_id , label\n";
    // Write the map
    for (const auto &vertex_label_pair : vertex_to_label_map) {
        my_file << vertex_label_pair.first << ", " << vertex_label_pair.second
                << "\n";
    }
}

void write_edge_to_label_map(const edge_to_label_map_t &edge_to_label_map,
                             const std::string &output_filename) {

    // Create an output filestream object
    std::ofstream my_file(output_filename);
    // Populate the header
    my_file << "# edge_source-target, label\n";
    // Write the map
    for (const auto &edge_label_pair : edge_to_label_map) {
        auto edge_desc = edge_label_pair.first;
        my_file << edge_desc.m_source << "-" << edge_desc.m_target << ", "
                << edge_label_pair.second << "\n";
    }
}

edge_to_label_map_t
read_edge_to_label_map(const std::string &edge_to_label_map_file) {
    std::vector<size_t> edge_sources;
    std::vector<size_t> edge_targets;
    std::vector<size_t> labels;
    edge_to_label_map_t output;
    // Create an input filestream
    std::ifstream my_file(edge_to_label_map_file);
    // Make sure the file is open
    if (!my_file.is_open())
        throw std::runtime_error("Could not open file.");
    // Helper vars
    std::string line;
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
        // Keep track if we are reading the source or the target node of the
        // edge. They are separated by -: i.e 12-23, label
        int colIdx = 0;

        // Extract each integer
        while (ss >> val) {
            // At the time we find a #, skip this line
            if (val == '#')
                break;
            if (colIdx == 0) {
                edge_sources.push_back(val);
            } else if (colIdx == 1) {
                edge_targets.push_back(val);
            } else if (colIdx == 2) {
                labels.push_back(val);
            }
            // If the next token is a dash, ignore it and move on
            if (ss.peek() == '-') {
                ss.ignore();
            }
            // If the next token is a comma, ignore it and move on
            if (ss.peek() == ',')
                ss.ignore();

            // Increment the column index
            colIdx++;
        }
    }
    if (edge_sources.size() != labels.size() ||
        edge_targets.size() != labels.size()) {
        throw std::runtime_error(
                R"("read_edge_to_label_map: Failure.
Not the same number of edge_sources, edge_targets and labels.
The file is not populated correctly. #edge_sources: )" +
                std::to_string(edge_sources.size()) +
                " - #edge_targets: " + std::to_string(edge_targets.size()) +
                " , #labels: " + std::to_string(labels.size()));
    }
    for (size_t i = 0; i < edge_sources.size(); ++i) {
        auto edge_desc = GraphType::edge_descriptor();
        edge_desc.m_source = edge_sources[i];
        edge_desc.m_target = edge_targets[i];
        output.emplace(edge_desc, labels[i]);
    }
    return output;
}

} // namespace SG
