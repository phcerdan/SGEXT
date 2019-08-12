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

#include <chrono>
#include <iostream>
#include <unordered_map>
// boost::program_options
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
// graph
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/graph_concepts.hpp>
#include <boost/graph/graphviz.hpp>

// Boost Filesystem
#include <boost/filesystem.hpp>

// Reduce graph via dfs:
#include "add_graph_peninsulas.hpp"
#include "compare_graphs.hpp"
#include "serialize_spatial_graph.hpp"
#include "spatial_graph.hpp"
#include "spatial_graph_io.hpp"
#include "spatial_graph_utilities.hpp"

#include "extend_low_info_graph.hpp"
#include "get_vtk_points_from_graph.hpp"
#include "graph_points_locator.hpp"

#ifdef VISUALIZE
#include "visualize_spatial_graph.hpp"
// #include "itkViewImage.h"
#endif

using namespace std;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

void export_graph_merge_low_high_info(
        SG::GraphType
                &output_g, // write_serialized_graph --graph cannot be const--
        const std::string folder_path,
        const fs::path &output_filename_path,
        bool useSerialized,
        bool verbose,
        const std::string &graph_title) {

    const fs::path output_folder_path{folder_path};
    if (!fs::exists(output_folder_path)) {
        throw std::runtime_error("output folder doesn't exist : " +
                                 output_folder_path.string());
    }

    if (!useSerialized) {
        fs::path output_full_path =
                output_folder_path /
                fs::path(output_filename_path.string() + ".dot");
        SG::write_graphviz_sg(output_full_path.string(), output_g);
        if (verbose)
            std::cout << graph_title + ": graph (graphviz) output stored in: "
                      << output_full_path.string() << std::endl;
    } else {
        fs::path output_full_path =
                output_folder_path /
                fs::path(output_filename_path.string() + ".txt");
        SG::write_serialized_sg(output_full_path.string(), output_g);
        if (verbose)
            std::cout << graph_title + ": graph (serialize) output stored in: "
                      << output_full_path.string() << std::endl;
    }
};

int main(int argc, char *const argv[]) {
    /*-------------- Parse command line -----------------------------*/
    po::options_description opt_desc("Allowed options are: ");
    opt_desc.add_options()("help,h", "display this message.");
    opt_desc.add_options()("highInfoGraph,i", po::value<string>()->required(),
                           "Input high info graph.");
    opt_desc.add_options()("lowInfoGraph,l", po::value<string>()->required(),
                           "Input low info graph .");
    opt_desc.add_options()("useSerialized,u",
                           po::bool_switch()->default_value(false),
                           "Use stored serialized graphs. If off, it will "
                           "require .dot graphviz files.");
    opt_desc.add_options()(
            "exportExtendedLowInfoGraph,e", po::value<string>()->required(),
            "Write .dot file with the extended low info spatial graph or "
            ".txt file if --useSerialized is on.");
    opt_desc.add_options()(
            "exportMergedGraph,o", po::value<string>()->required(),
            "Write .dot file with the final graph (with pensinsulas added) or "
            ".txt file if --useSerialized is on.");
    opt_desc.add_options()(
            "computePeninsulas,p", po::bool_switch()->default_value(false),
            "Perform the full analysis adding isolated components with "
            "zero or one touching point with the extensted graph.");
#ifdef VISUALIZE
    opt_desc.add_options()(
            "visualize,t", po::bool_switch()->default_value(false),
            "Visualize. Requires VISUALIZE option enabled at build.");
#endif
    opt_desc.add_options()(
            "radius,r", po::value<double>()->default_value(4.0),
            "Radius to use in the extend_low_info_graph visitor.");
    opt_desc.add_options()("verbose,v", po::bool_switch()->default_value(false),
                           "verbose output.");

    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, opt_desc), vm);
        if (vm.count("help") || argc <= 1) {
            std::cout << "Basic usage:\n" << opt_desc << "\n";
            return EXIT_SUCCESS;
        }
        po::notify(vm);
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    string filenameHigh = vm["highInfoGraph"].as<string>();
    string filenameLow = vm["lowInfoGraph"].as<string>();
    double radius = vm["radius"].as<double>();
    bool verbose = vm["verbose"].as<bool>();
    if (verbose) {
        std::cout << "Filename High Info Graph: " << filenameHigh << std::endl;
        std::cout << "Filename Low Info Graph: " << filenameLow << std::endl;
    }
    bool exportExtendedLowInfoGraph = vm.count("exportExtendedLowInfoGraph");
    bool exportMergedGraph = vm.count("exportMergedGraph");
    bool useSerialized = vm["useSerialized"].as<bool>();
    bool computePeninsulas = vm["computePeninsulas"].as<bool>();

#ifdef VISUALIZE
    bool visualize = vm["visualize"].as<bool>();
#endif
    // Get filenameHigh without extension (and without folders).
    const fs::path input_stem = fs::path(filenameHigh).stem();
    const fs::path output_file_path_extended =
            fs::path(input_stem.string() + "_EXTENDED");
    const fs::path output_file_path_merged =
            fs::path(input_stem.string() + "_MERGED");

    SG::GraphType g0;     // lowGraph
    SG::GraphType g1;     // highGraph
    if (!useSerialized) { // read graphviz
        SG::read_graphviz_sg(filenameLow, g0);
        SG::read_graphviz_sg(filenameHigh, g1);
    } else {
        g0 = SG::read_serialized_graph(filenameLow);
        g1 = SG::read_serialized_graph(filenameHigh);
    }

    auto repeated_points_g0 = SG::check_unique_points_in_graph(g0);
    if (repeated_points_g0.second) {
        std::cout << "Warning: duplicated points exist in low info graph (g0). "
                     "Repeated Points: "
                  << repeated_points_g0.first.size() << std::endl;
        for (const auto &p : repeated_points_g0.first) {
            SG::print_pos(std::cout, p);
            std::cout << std::endl;
        }
    }
    auto repeated_points_g1 = SG::check_unique_points_in_graph(g1);
    if (repeated_points_g1.second) {
        std::cout
                << "Warning: duplicated points exist in high info graph (g1). "
                   "Repeated Points: "
                << repeated_points_g1.first.size() << std::endl;
        for (const auto &p : repeated_points_g1.first) {
            SG::print_pos(std::cout, p);
            std::cout << std::endl;
        }
    }

    auto nvertices_g0 = boost::num_vertices(g0);
    auto nvertices_g1 = boost::num_vertices(g1);
    auto nedges_g0 = boost::num_edges(g0);
    auto nedges_g1 = boost::num_edges(g1);
    std::cout << "** G0:  vertices: " << nvertices_g0
              << ". edges: " << nedges_g0 << std::endl;
    std::cout << "** G1:  vertices: " << nvertices_g1
              << ". edges: " << nedges_g1 << std::endl;
    if (nvertices_g0 == 0 || nvertices_g1 == 0) {
        throw std::runtime_error("Graphs with zero vertices");
    }

    // Make the comparison between low and high and take the result
    // Use extend_low_info_graph
    std::vector<std::reference_wrapper<const SG::GraphType>> graphs;
    graphs.reserve(2);
    graphs.push_back(std::cref(g0));
    graphs.push_back(std::cref(g1));
    auto merger_map_pair = SG::get_vtk_points_from_graphs(graphs);
    auto &mergePoints = merger_map_pair.first;
    auto &idMap = merger_map_pair.second;
    auto octree = SG::build_octree_locator(merger_map_pair.first->GetPoints());
    auto extended_g = extend_low_info_graph_via_dfs(graphs, idMap, octree,
                                                    radius, verbose);

    auto repeated_points_extended_g =
            SG::check_unique_points_in_graph(extended_g);
    if (repeated_points_extended_g.second) {
        std::cout << "Warning: duplicated points exist in extended_low_info "
                     "graph (extended_g). "
                     "Repeated Points: "
                  << repeated_points_extended_g.first.size() << std::endl;
        for (const auto &p : repeated_points_extended_g.first) {
            SG::print_pos(std::cout, p);
            std::cout << std::endl;
        }
    }

    // auto extended_g = SG::compare_low_and_high_info_graphs(g0, g1);
    auto nvertices_extended = boost::num_vertices(extended_g);
    auto nedges_extended = boost::num_edges(extended_g);
    std::cout << "** GExtended:  vertices: " << nvertices_extended
              << ". edges: " << nedges_extended << std::endl;

    if (exportExtendedLowInfoGraph) {
        string exportExtendedLowInfoGraph_filename =
                vm["exportExtendedLowInfoGraph"].as<string>();
        export_graph_merge_low_high_info(
                extended_g, exportExtendedLowInfoGraph_filename,
                output_file_path_extended, useSerialized, verbose, "Extended");
    }

#ifdef VISUALIZE
    if (visualize) {
        SG::visualize_spatial_graph(extended_g);
    }
#endif

    // Return early if user not interested in full analysis.
    if (!computePeninsulas) {
        return EXIT_SUCCESS;
    }

    std::vector<std::reference_wrapper<const SG::GraphType>> graphs_merged;
    graphs_merged.reserve(2);
    graphs_merged.push_back(std::cref(extended_g));
    graphs_merged.push_back(std::cref(g1));
    auto merger_map_pair_merged = SG::get_vtk_points_from_graphs(graphs_merged);
    auto &mergePoints_merged = merger_map_pair_merged.first;
    auto &idMap_merged = merger_map_pair_merged.second;

    // auto extended_g_points_map_pair =
    // SG::get_vtk_points_from_graph(extended_g); SG::append_new_graph_points(
    //     extended_g_points_map_pair.first, extended_g_points_map_pair.second,
    //     mergePoints, idMap);
    size_t extended_graph_index = 0;
    size_t high_info_graph_index = 1;
    double radius_touch = radius;
    auto add_graph_peninsulas_result = SG::add_graph_peninsulas(
            graphs_merged, extended_graph_index, high_info_graph_index,
            mergePoints_merged, idMap_merged, radius_touch, verbose);
    auto &merged_g = add_graph_peninsulas_result.graph;
    auto &octree_merged = add_graph_peninsulas_result.octree;

    auto nvertices_merged = boost::num_vertices(merged_g);
    auto nedges_merged = boost::num_edges(merged_g);
    std::cout << "** GMerged:  vertices: " << nvertices_merged
              << ". edges: " << nedges_merged << std::endl;

    if (exportMergedGraph) {
        string exportMergedGraph_filename =
                vm["exportMergedGraph"].as<string>();
        export_graph_merge_low_high_info(merged_g, exportMergedGraph_filename,
                                         output_file_path_merged, useSerialized,
                                         verbose, "Merged");
    }

#ifdef VISUALIZE
    if (visualize) {
        SG::visualize_spatial_graph(merged_g);
    }
#endif

} // end main
