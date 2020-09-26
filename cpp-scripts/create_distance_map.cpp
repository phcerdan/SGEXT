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

#include "create_distance_map_function.hpp"

// boost::program_options
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

namespace po = boost::program_options;

int main(int argc, char *const argv[]) {
    /*-------------- Parse command line -----------------------------*/
    po::options_description opt_desc("Allowed options are: ");
    opt_desc.add_options()("help,h", "display this message.");
    opt_desc.add_options()("input,i", po::value<std::string>()->required(),
                           "Input thin image.");
    opt_desc.add_options()("outputFolder,o",
                           po::value<std::string>()->required(),
                           "Output folder for the distance map.");
    // opt_desc.add_options()( "visualize,t",
    // po::bool_switch()->default_value(false), "Visualize object with DGtal.
    // Requires VISUALIZE option enabled at build.");
    opt_desc.add_options()("foreground,f",
                           po::value<std::string>()->default_value("white"),
                           "foreground color in binary image [black|white]");
    opt_desc.add_options()("use_itk_approximate,a",
                           po::bool_switch()->default_value(false),
                           "approximate dmap using itk (Chamfer distance).");
    opt_desc.add_options()("verbose,v", po::bool_switch()->default_value(false),
                           "verbose output.");

    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, opt_desc), vm);
        if (static_cast<bool>(vm.count("help")) || argc <= 1) {
            std::cout << "Basic usage:\n" << opt_desc << "\n";
            return EXIT_SUCCESS;
        }
        po::notify(vm);
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::string filename = vm["input"].as<std::string>();
    std::string outputFolder = vm["outputFolder"].as<std::string>();
    ;
    bool verbose = vm["verbose"].as<bool>();
    bool use_itk_approximate = vm["use_itk_approximate"].as<bool>();
    std::string foreground = vm["foreground"].as<std::string>();
    if (static_cast<bool>(vm.count("foreground")) &&
        (!(foreground == "white" || foreground == "black"))) {
        throw po::validation_error(po::validation_error::invalid_option_value,
                                   "foreground");
    }

    SG::create_distance_map_function_io(filename, outputFolder, foreground,
                                        use_itk_approximate, verbose);
}
