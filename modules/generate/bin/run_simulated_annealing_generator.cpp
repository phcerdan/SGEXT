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
// boost::program_options
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
// boost::filesystem
#include <boost/filesystem.hpp>

#include "simulated_annealing_generator.hpp"
#include "spatial_graph_io.hpp"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

int main(int argc, char *const argv[]) {
    /*-------------- Parse command line -----------------------------*/
    po::options_description opt_desc("Allowed options are: ");
    opt_desc.add_options()("help,h", "display this message.");
    opt_desc.add_options()("input,i",
                           po::value<std::string>()->default_value(""),
                           "Input configuration file with parameters.");
    opt_desc.add_options()("output,o",
                           po::value<std::string>()->default_value(""),
                           "Output filename.");
    opt_desc.add_options()("verbose,v", po::bool_switch()->default_value(false),
                           "verbose output");
    opt_desc.add_options()("default_config_file,c", po::value<std::string>(),
                           "Generate a default config file.");
    opt_desc.add_options()(
            "visualize,t", po::bool_switch()->default_value(false),
            "Visualize thin result. Requires VISUALIZE option at build");

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

    // Parse options
    std::string input_filename = vm["input"].as<std::string>();
    bool verbose = vm["verbose"].as<bool>();
    std::string output_filename = vm["output"].as<std::string>();
    std::string default_config_file;
    if (!vm["default_config_file"].empty()) {
        default_config_file = vm["default_config_file"].as<std::string>();
        auto gen = SG::simulated_annealing_generator(100);
        gen.init_histograms(100, 100);
        gen.save_parameters_to_file(default_config_file);
        std::cout << "Config file generated in: " + default_config_file
                  << std::endl;
        return EXIT_SUCCESS;
    }
    if (input_filename.empty() || output_filename.empty()) {

        std::cout << "Provide input_filename and output_filename options"
                  << std::endl;
        std::cout << "Basic usage:\n" << opt_desc << "\n";
        return EXIT_FAILURE;
    }

    // Output file
    const auto output_filename_parent_path =
            fs::path(output_filename).parent_path();
    const auto output_filename_without_extension =
            fs::path(output_filename).stem();
    const auto output_filename_extension =
            fs::path(output_filename).extension();
    std::string final_output_filename =
            (output_filename_parent_path / output_filename_without_extension)
                    .string();
    // Get time for the output filename
    std::time_t t = std::time(nullptr);
    char timestr[120];
    if (std::strftime(timestr, sizeof(timestr), "%Y-%m-%d-%H:%M",
                      std::localtime(&t))) {
        std::cout << timestr << '\n';
    }
    final_output_filename += timestr;
    final_output_filename += output_filename_extension.string();

    // Generator
    auto gen = SG::simulated_annealing_generator(input_filename);
    if(verbose) { gen.verbose = verbose; }
    // gen.set_parameters_from_file(input_filename);
    // gen.init_graph_degree(gen.physical_scaling_params.num_vertices);
    // gen.init_graph_vertex_positions();
    // gen.init_parameters();
    // gen.init_histograms(gen.ete_distance_params.num_bins,
    //                     gen.cosine_params.num_bins);
    gen.print(std::cout);
    gen.engine();
    std::ofstream ofile(final_output_filename, std::ios_base::app);
    gen.print(ofile);
    gen.print_histo_and_target_distribution_ete_distances(ofile);
    gen.print_histo_and_target_distribution_cosines(ofile);
    SG::write_graphviz_sg(ofile, gen.graph_);

    std::string parameters_output_filename =
            (output_filename_parent_path / output_filename_without_extension)
                    .string() +
            timestr + "_parameters.json";
    gen.save_parameters_to_file(parameters_output_filename);
}
