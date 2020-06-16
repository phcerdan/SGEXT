/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
        if (vm.count("help") || argc <= 1) {
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
    if (vm.count("foreground") &&
        (!(foreground == "white" || foreground == "black")))
        throw po::validation_error(po::validation_error::invalid_option_value,
                                   "foreground");

    SG::create_distance_map_function_io(filename, outputFolder, foreground,
                                        use_itk_approximate, verbose);
}
