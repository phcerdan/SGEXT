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

#include <iostream>

// NeighborhoodTables.h contains strings with full-paths of the system where
// DGtal was originally built or installed. So it cannot be used when DGtal
// library is redistributed/packaged. However, it can be used to get the filenames
// of the tables, but the full-path itself would not exist.
// However we use it here to provide a default value for tables_folder.
#include <DGtal/topology/tables/NeighborhoodTables.h>


// boost::program_options
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

// Boost Filesystem
#include <boost/filesystem.hpp>

#include "thin_function.hpp"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

int main(int argc, char* const argv[]) {
  const fs::path maybe_wrong_tableSimple26_6{DGtal::simplicity::tableSimple26_6};
  const std::string tables_folder_default =
    maybe_wrong_tableSimple26_6.parent_path().string();
  /*-------------- Parse command line -----------------------------*/
  po::options_description opt_desc("Allowed options are: ");
  opt_desc.add_options()("help,h", "display this message.");
  opt_desc.add_options()("input,i", po::value<std::string>()->required(),
                         "Input 3D image file.");
  opt_desc.add_options()(
      "skel,s", po::value<std::string>()->required(),
      "type of skeletonization. Valid: 1isthmus, isthmus, end, ulti");
  opt_desc.add_options()(
      "select,c", po::value<std::string>()->required(),
      "select method for skeletonization. Valid: dmax, random, first");
  opt_desc.add_options()("foreground,f",
                         po::value<std::string>()->default_value("white"),
                         "foreground color in binary image. [black|white]");
  opt_desc.add_options()(
      "persistence,p", po::value<int>()->default_value(0),
      "persistence value, implies use of persistence algorithm if p>=1");
  opt_desc.add_options()("profile", po::bool_switch()->default_value(false),
                         "profile algorithm");
  opt_desc.add_options()("verbose,v", po::bool_switch()->default_value(false),
                         "verbose output");
  opt_desc.add_options()(
      "visualize,t", po::bool_switch()->default_value(false),
      "Visualize thin result. Requires VISUALIZE option at build");
  opt_desc.add_options()(
      "output_filename_simple,z", po::bool_switch()->default_value(false),
      "Filename does not contain the parameters used for this filter.");
  opt_desc.add_options()("exportSDP,e", po::value<std::string>()->default_value(""),
                         "Folder to export the resulting set of points in a "
                         "simple (sequence of discrete point (sdp)).");
  opt_desc.add_options()(
      "exportImage,o", po::value<std::string>()->required(),
      "Folder to export the resulting set of points as an ITK Image.");
  opt_desc.add_options()("inputDistanceMapImageFilename,d", po::value<std::string>(),
                         "Input 3D Distance Map Image from script "
                         "create_distance_map. Used with option --select=dmax");
  opt_desc.add_options()(
      "tables_folder,l", po::value<std::string>()->default_value(tables_folder_default),
      "Folder where the DGtal look-up-tables are located. "
      "For example simplicity_table26_6.zlib");

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

  // Parse options
  std::string filename = vm["input"].as<std::string>();
  bool verbose = vm["verbose"].as<bool>();
  bool output_filename_simple = vm["output_filename_simple"].as<bool>();
  bool profile = vm["profile"].as<bool>();
  int persistence = vm["persistence"].as<int>();
  if(vm.count("persistence") && persistence < 0)
    throw po::validation_error(po::validation_error::invalid_option_value,
                               "persistence");
  std::string foreground = vm["foreground"].as<std::string>();
  if(vm.count("foreground") &&
     (!(foreground == "white" || foreground == "black")))
    throw po::validation_error(po::validation_error::invalid_option_value,
                               "foreground");

  std::string sk_string = vm["skel"].as<std::string>();
  if(vm.count("skel") && (!(sk_string == "ulti" || sk_string == "end" ||
                            sk_string == "isthmus" || sk_string == "1isthmus" ||
                            sk_string == "isthmus1"
                            )))
    throw po::validation_error(po::validation_error::invalid_option_value,
                               "skel");
  std::string select_string = vm["select"].as<std::string>();
  if(vm.count("select") &&
     (!(select_string == "random" || select_string == "dmax" ||
        select_string == "first")))
    throw po::validation_error(po::validation_error::invalid_option_value,
                               "select");

  bool visualize = vm["visualize"].as<bool>();

  const auto exportImageFolder = vm["exportImage"].as<std::string>();
  const fs::path output_folder_path{exportImageFolder};
  if(!fs::exists(output_folder_path)) {
    std::cerr << "output folder doesn't exist : "
      << output_folder_path.string() << std::endl;
    throw po::validation_error(po::validation_error::invalid_option_value,
        "output_folder_path");
  }

  const auto exportSDP = vm["exportSDP"].as<std::string>();
  if(!exportSDP.empty()){
    const fs::path output_folder_path{exportSDP};
    if(!fs::exists(output_folder_path)) {
      std::cerr << "output folder doesn't exist : "
                << output_folder_path.string() << std::endl;
      throw po::validation_error(po::validation_error::invalid_option_value,
                                 "output_folder_path");
    }
  }

  if(select_string == "dmax" && !vm.count("inputDistanceMapImageFilename")) {
    std::cerr << "Please select an inputDistanceMapImageFilename.\n";
    std::cerr << "A distance map can be generated using the script:\n";
    std::cerr << "  create_distance_map -i inputImage -o outputFolder \n";
    throw po::validation_error(po::validation_error::invalid_option_value,
                               "inputDistanceMapImageFilename");
  }

  std::string inputDistanceMapImageFilename = "";
  if(vm.count("inputDistanceMapImageFilename")) {
    const fs::path inputDistanceMapImageFilename_path{
        vm["inputDistanceMapImageFilename"].as<std::string>()};
    if(!fs::exists(inputDistanceMapImageFilename_path)) {
      std::cerr << "input distance map does not exist : "
                << inputDistanceMapImageFilename_path.string() << std::endl;
      throw po::validation_error(po::validation_error::invalid_option_value,
                                 "inputDistanceMapImageFilename");
    }
    inputDistanceMapImageFilename = inputDistanceMapImageFilename_path.string();
  }

  const auto tables_folder = vm["tables_folder"].as<std::string>();
  const fs::path tables_folder_path{tables_folder};
  if(!fs::exists(tables_folder_path)) {
    std::cerr << "tables_folder doesn't exist : "
      << tables_folder_path.string() << std::endl;
    throw po::validation_error(po::validation_error::invalid_option_value,
        "tables_folder_path");
  }

  SG::thin_function_io(
      filename,
      sk_string,
      select_string,
      exportImageFolder,
      tables_folder,
      persistence,
      inputDistanceMapImageFilename,
      foreground,
      exportSDP,
      profile,
      verbose,
      visualize
      );

  /*-------------- End of parse -----------------------------*/
}
