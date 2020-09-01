/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <iostream>
#include <unordered_map>
// boost::program_options
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
// Boost Filesystem
#include <boost/filesystem.hpp>

#include "spatial_histograms.hpp"
#include "graph_data.hpp"
#include <fstream>

using namespace std;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

enum class DataType {
  degrees,
  ete_distances,
  angles,
  cosines,
  contour_lengths
};

int main(int argc, char* const argv[]) {
  /*-------------- Parse command line -----------------------------*/
  po::options_description opt_desc("Allowed options are: ");
  opt_desc.add_options()("help,h", "display this message.");
  opt_desc.add_options()("input,i", po::value<string>()->required(),
                         "Input thin image.");
  opt_desc.add_options()("exportHistograms,e", po::value<string>(),
                         "Export histogram.");
  opt_desc.add_options()("binsHistoDegrees,d",
                         po::value<size_t>()->default_value(0),
                         "Bins for the histogram of degrees. Default [0] get "
                         "the breaks between 0 and max_degree");
  opt_desc.add_options()("binsHistoAngles,a",
                         po::value<size_t>()->default_value(100),
                         "Bins for the histogram of angles . Use 0 for "
                         "automatic computation of breaks (not recommended)");
  opt_desc.add_options()("binsHistoCosines,n",
                         po::value<size_t>()->default_value(100),
                         "Bins for the histogram of cosines .Use 0 for "
                         "automatic computation of breaks (not recommended)");
  opt_desc.add_options()(
      "widthHistoEteDistances,l", po::value<double>()->default_value(0.3),
      "Width between breaks for histogram of ete distances. Use 0.0 to "
      "automatically compute breaks (not recommended).");
  opt_desc.add_options()(
      "widthHistoContourLengths,p", po::value<double>()->default_value(0.3),
      "Width between breaks for histogram of contour lengths. Use 0.0 to "
      "automatically compute breaks (not recommended).");
  // opt_desc.add_options()( "visualize,t",
  // po::bool_switch()->default_value(false), "Visualize object with DGtal.
  // Requires VISUALIZE option enabled at build."); opt_desc.add_options()(
  // "verbose,v",  po::bool_switch()->default_value(false), "verbose output."
  // );;

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

  string filename = vm["input"].as<string>();
  // bool verbose = vm["verbose"].as<bool>();
  size_t binsHistoDegrees = vm["binsHistoDegrees"].as<size_t>();
  size_t binsHistoAngles = vm["binsHistoAngles"].as<size_t>();
  size_t binsHistoCosines = vm["binsHistoCosines"].as<size_t>();
  double widthHistoEteDistances = vm["widthHistoEteDistances"].as<double>();
  double widthHistoContourLengths = vm["widthHistoContourLengths"].as<double>();

  // #ifdef VISUALIZE
  //   bool visualize = vm["visualize"].as<bool>();
  // #endif

  // Get filename without extension (and without folders).
  const fs::path input_stem = fs::path(filename).stem();
  const fs::path output_file_path = fs::path(input_stem.string());

  // Parse graph_data
  std::vector<std::pair<std::string, std::vector<double> > > graph_datas =
    SG::read_graph_data(filename);

  // Parse
  string exportHistograms_filename = vm["exportHistograms"].as<string>();
  const fs::path histo_output_folder_path{exportHistograms_filename};
  if(!fs::exists(histo_output_folder_path)) {
    throw std::runtime_error("histo_output folder doesn't exist : " +
                             histo_output_folder_path.string());
  }
  fs::path histo_output_full_path =
      histo_output_folder_path /
      fs::path(output_file_path.string() + "_bD" +
               std::to_string(binsHistoDegrees) + "_bA" +
               std::to_string(binsHistoAngles) + "_bC" +
               std::to_string(binsHistoCosines) + "_wL" +
               std::to_string(widthHistoEteDistances) + "_wP" +
               std::to_string(widthHistoContourLengths) + ".histo");
  std::ofstream histo_out;
  histo_out.open(histo_output_full_path.string().c_str());
  // Create a map, from strings to DataType
  std::unordered_map<std::string, DataType> dict_types;
  dict_types["degrees"] = DataType::degrees;
  dict_types["ete_distances"] = DataType::ete_distances;
  dict_types["angles"] = DataType::angles;
  dict_types["cosines"] = DataType::cosines;
  dict_types["contour_lengths"] = DataType::contour_lengths;
  // Parse the names
  for(const auto& graph_data : graph_datas) {
    auto found = dict_types.find(graph_data.first);
    if(found == dict_types.end())
      throw std::runtime_error("Unexpected data type: " + graph_data.first);
    if(found->second == DataType::degrees)
      SG::print_histogram(
          SG::histogram_degrees(graph_data.second, binsHistoDegrees),
          histo_out);
    else if(found->second == DataType::ete_distances)
      SG::print_histogram(SG::histogram_ete_distances(graph_data.second,
                                                      widthHistoEteDistances),
                          histo_out);
    else if(found->second == DataType::contour_lengths)
      SG::print_histogram(SG::histogram_contour_lengths(
                              graph_data.second, widthHistoContourLengths),
                          histo_out);
    else if(found->second == DataType::angles)
      SG::print_histogram(
          SG::histogram_angles(graph_data.second, binsHistoAngles), histo_out);
    else if(found->second == DataType::cosines)
      SG::print_histogram(
          SG::histogram_cosines(graph_data.second, binsHistoCosines),
          histo_out);
  }
  std::cout << "Output histograms to: " << histo_output_full_path.string()
            << std::endl;
}
