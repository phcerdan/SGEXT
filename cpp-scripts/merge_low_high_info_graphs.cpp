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
#include "DGtal/graph/ObjectBoostGraphInterface.h"
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

#ifdef VISUALIZE
#include "visualize_spatial_graph.hpp"
// #include "itkViewImage.h"
#endif

using namespace std;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

int main(int argc, char* const argv[]){

  /*-------------- Parse command line -----------------------------*/
  po::options_description general_opt ( "Allowed options are: " );
  general_opt.add_options()
    ( "help,h", "display this message." )
    ( "highInfoGraph,i", po::value<string>()->required(), "Input high info graph." )
    ( "lowInfoGraph,l", po::value<string>()->required(), "Input low info graph ." )
    ( "exportMergedGraph,o", po::value<string>()->required(), "Write .dot file with the merged spatial graph." )
#ifdef VISUALIZE
    ( "visualize,t", po::bool_switch()->default_value(false), "Visualize object with DGtal. Requires VISUALIZE option enabled at build.")
#endif
    ( "verbose,v",  po::bool_switch()->default_value(false), "verbose output." );

  po::variables_map vm;
  try {
    po::store(po::parse_command_line(argc, argv, general_opt), vm);
    if (vm.count ( "help" ) || argc<=1 )
    {
      std::cout << "Basic usage:\n" << general_opt << "\n";
      return EXIT_SUCCESS;
    }
    po::notify ( vm );
  } catch ( const std::exception& e ) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  string filenameHigh = vm["highInfoGraph"].as<string>();
  string filenameLow = vm["lowInfoGraph"].as<string>();
  bool verbose = vm["verbose"].as<bool>();
  if(verbose){
    std::cout <<"Filename High Info Graph: " << filenameHigh << std::endl;
    std::cout <<"Filename Low Info Graph: " << filenameLow << std::endl;
  }
  bool exportMergedGraph = vm.count("exportMergedGraph");
#ifdef VISUALIZE
  bool visualize = vm["visualize"].as<bool>();
#endif
  // Get filenameHigh without extension (and without folders).
  const fs::path input_stem = fs::path(filenameHigh).stem();
  const fs::path output_file_path = fs::path(
      input_stem.string() + "_low_high_merged");

  SG::GraphType g0; // lowGraph
  SG::GraphType g1; // highGraph
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

  auto nvertices_g0 = boost::num_vertices(g0);
  auto nvertices_g1 = boost::num_vertices(g1);
  auto nedges_g0 = boost::num_edges(g0);
  auto nedges_g1 = boost::num_edges(g1);
  std::cout << "** G0:  vertices: " <<  nvertices_g0 << ". edges: " << nedges_g0 << std::endl;
  std::cout << "** G1:  vertices: " <<  nvertices_g1 << ". edges: " << nedges_g1 << std::endl;
  if(nvertices_g0 == 0 || nvertices_g1 == 0){
    throw std::runtime_error("Graphs with zero vertices");
  }

  // Make the comparison between low and high and take the result
  auto merged_g = SG::compare_high_and_low_info_graphs(g0, g1);

  if(exportMergedGraph) {
    string exportMergedGraph_filename_merged = vm["exportMergedGraph"].as<string>();
    boost::dynamic_properties dp_merged;
    dp_merged.property("node_id", boost::get(boost::vertex_index, merged_g));
    dp_merged.property("spatial_node", boost::get(boost::vertex_bundle, merged_g));
    dp_merged.property("spatial_edge", boost::get(boost::edge_bundle, merged_g));
    {
      const fs::path output_folder_path{exportMergedGraph_filename_merged};
      if(!fs::exists(output_folder_path)) {
        throw std::runtime_error("output folder doesn't exist : " + output_folder_path.string());
      }
      fs::path output_full_path = output_folder_path / fs::path(
          output_file_path.string() +
          ".dot");
      std::ofstream out;
      out.open(output_full_path.string().c_str());
      boost::write_graphviz_dp(out, merged_g, dp_merged);
      if(verbose)
        std::cout << "Output merged graph (graphviz) to: " << output_full_path.string() << std::endl;
    }
  }
#ifdef VISUALIZE
    if(visualize)
    {
      SG::visualize_spatial_graph(merged_g);
    }
#endif

} // end main
