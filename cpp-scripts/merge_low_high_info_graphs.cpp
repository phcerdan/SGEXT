/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <iostream>
#include <chrono>
#include <unordered_map>
// boost::program_options
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
// graph
#include <boost/graph/graph_concepts.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/graphviz.hpp>

// Boost Filesystem
#include <boost/filesystem.hpp>

// Reduce graph via dfs:
#include "spatial_graph.hpp"
#include "spatial_graph_utilities.hpp"
#include "compare_graphs.hpp"
#include "serialize_spatial_graph.hpp"

#ifdef VISUALIZE
#include "visualize_spatial_graph.hpp"
// #include "itkViewImage.h"
#endif

using namespace std;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

int main(int argc, char* const argv[]) {
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
  opt_desc.add_options()("exportMergedGraph,o", po::value<string>()->required(),
                         "Write .dot file with the merged spatial graph or "
                         ".txt file if --useSerialized is on.");
#ifdef VISUALIZE
  opt_desc.add_options()(
      "visualize,t", po::bool_switch()->default_value(false),
      "Visualize. Requires VISUALIZE option enabled at build.");
#endif
  opt_desc.add_options()("verbose,v", po::bool_switch()->default_value(false),
                         "verbose output.");

  po::variables_map vm;
  try {
    po::store(po::parse_command_line(argc, argv, opt_desc), vm);
    if(vm.count("help") || argc <= 1) {
      std::cout << "Basic usage:\n" << opt_desc << "\n";
      return EXIT_SUCCESS;
    }
    po::notify(vm);
  } catch(const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  string filenameHigh = vm["highInfoGraph"].as<string>();
  string filenameLow = vm["lowInfoGraph"].as<string>();
  bool verbose = vm["verbose"].as<bool>();
  if(verbose) {
    std::cout << "Filename High Info Graph: " << filenameHigh << std::endl;
    std::cout << "Filename Low Info Graph: " << filenameLow << std::endl;
  }
  bool exportMergedGraph = vm.count("exportMergedGraph");
  bool useSerialized = vm["useSerialized"].as<bool>();

#ifdef VISUALIZE
  bool visualize = vm["visualize"].as<bool>();
#endif
  // Get filenameHigh without extension (and without folders).
  const fs::path input_stem = fs::path(filenameHigh).stem();
  const fs::path output_file_path = fs::path(input_stem.string() + "_MERGED");

  SG::GraphType g0;     // lowGraph
  SG::GraphType g1;     // highGraph
  if(!useSerialized) {  // read graphviz
    boost::dynamic_properties dp0;
    {
      dp0.property("node_id", boost::get(&SG::SpatialNode::label, g0));
      dp0.property("spatial_node", boost::get(boost::vertex_bundle, g0));
      dp0.property("spatial_edge", boost::get(boost::edge_bundle, g0));
      std::ifstream ifileLow(filenameLow);
      boost::read_graphviz(ifileLow, g0, dp0, "node_id");
    }
    boost::dynamic_properties dp1;
    {
      dp1.property("node_id", boost::get(&SG::SpatialNode::label, g1));
      dp1.property("spatial_node", boost::get(boost::vertex_bundle, g1));
      dp1.property("spatial_edge", boost::get(boost::edge_bundle, g1));
      std::ifstream ifileHigh(filenameHigh);
      boost::read_graphviz(ifileHigh, g1, dp1, "node_id");
    }
  } else {
    g0 = SG::read_serialized_graph(filenameLow);
    g1 = SG::read_serialized_graph(filenameHigh);
  }

  auto repeated_points_g0 = SG::check_unique_points_in_graph(g0);
  if(repeated_points_g0.second) {
    std::cout << "Warning: duplicated points exist in low info graph (g0)"
                 "Repeated Points: "
              << repeated_points_g0.first.size() << std::endl;
    for(const auto& p : repeated_points_g0.first) {
      SG::print_pos(std::cout, p);
      std::cout << std::endl;
    }
  }
  auto repeated_points_g1 = SG::check_unique_points_in_graph(g1);
  if(repeated_points_g1.second) {
    std::cout << "Warning: duplicated points exist in high info graph (g1)"
                 "Repeated Points: "
              << repeated_points_g1.first.size() << std::endl;
    for(const auto& p : repeated_points_g1.first) {
      SG::print_pos(std::cout, p);
      std::cout << std::endl;
    }
  }

  auto nvertices_g0 = boost::num_vertices(g0);
  auto nvertices_g1 = boost::num_vertices(g1);
  auto nedges_g0 = boost::num_edges(g0);
  auto nedges_g1 = boost::num_edges(g1);
  std::cout << "** G0:  vertices: " << nvertices_g0 << ". edges: " << nedges_g0
            << std::endl;
  std::cout << "** G1:  vertices: " << nvertices_g1 << ". edges: " << nedges_g1
            << std::endl;
  if(nvertices_g0 == 0 || nvertices_g1 == 0) {
    throw std::runtime_error("Graphs with zero vertices");
  }

  // Make the comparison between low and high and take the result
  auto merged_g = SG::compare_low_and_high_info_graphs(g0, g1);
  auto nvertices_merged = boost::num_vertices(merged_g);
  auto nedges_merged = boost::num_edges(merged_g);
  std::cout << "** GMerged:  vertices: " << nvertices_merged
            << ". edges: " << nedges_merged << std::endl;

  if(exportMergedGraph) {
    string exportMergedGraph_filename_merged =
        vm["exportMergedGraph"].as<string>();
    const fs::path output_folder_path{exportMergedGraph_filename_merged};
    if(!fs::exists(output_folder_path)) {
      throw std::runtime_error("output folder doesn't exist : " +
                               output_folder_path.string());
    }

    if(!useSerialized) {
      boost::dynamic_properties dp_merged;
      dp_merged.property("node_id", boost::get(boost::vertex_index, merged_g));
      dp_merged.property("spatial_node",
                         boost::get(boost::vertex_bundle, merged_g));
      dp_merged.property("spatial_edge",
                         boost::get(boost::edge_bundle, merged_g));
      fs::path output_full_path =
          output_folder_path / fs::path(output_file_path.string() + ".dot");
      std::ofstream out;
      out.open(output_full_path.string().c_str());
      boost::write_graphviz_dp(out, merged_g, dp_merged);
      if(verbose)
        std::cout << "Output merged graph (graphviz) to: "
                  << output_full_path.string() << std::endl;
    } else {
      fs::path output_full_path =
          output_folder_path / fs::path(output_file_path.string() + ".txt");
      SG::write_serialized_graph(merged_g, output_full_path.string());
      if(verbose)
        std::cout << "Output merged graph (serialize) to: "
                  << output_full_path.string() << std::endl;
    }
  }
#ifdef VISUALIZE
  if(visualize) {
    SG::visualize_spatial_graph(merged_g);
  }
#endif

}  // end main
