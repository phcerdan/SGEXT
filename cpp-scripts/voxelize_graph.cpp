/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "spatial_graph_io.hpp"
#include "voxelize_graph.hpp"
#include <iostream>
#include <string>
// ITK
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
// boost::program_options
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
// boost::filesystem
#include <boost/filesystem.hpp>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

int main(int argc, char *const argv[]) {
    /*-------------- Parse command line -----------------------------*/
    po::options_description opt_desc("Allowed options are: ");
    opt_desc.add_options()("help,h", "display this message.");
    opt_desc.add_options()("input,i", po::value<std::string>()->required(),
                           "Input graph.");
    opt_desc.add_options()(
            "reference_image,r", po::value<std::string>()->required(),
            "Reference binary image to get size and metadata of output image.");
    opt_desc.add_options()("vertex_to_label_map_file,n",
                           po::value<std::string>()->required(),
                           "Stored vertex_to_label_map file.");
    opt_desc.add_options()("edge_to_label_map_file,m",
                           po::value<std::string>()->default_value(""),
                           "Stored edge_to_label_map file. If empty, only "
                           "nodes will be voxelized. If equal to 'max' "
                           "create_edge_to_label_map_from_vertex_to_label_map_"
                           "using_max will be used.");
    opt_desc.add_options()(
            "graph_positions_are_in_index_space,p",
            po::value<bool>()->default_value(false),
            "Stored graph positions might be in index or physical space. "
            "Assume by default that they in physical space, turn this ON if "
            "they are in index space");
    opt_desc.add_options()("verbose,v", po::bool_switch()->default_value(false),
                           "verbose output");
    opt_desc.add_options()("outputFolder,o",
                           po::value<std::string>()->required(),
                           "Folder to export the resulting binary image.");
    opt_desc.add_options()("outputFilename,e", po::value<std::string>(),
                           "FileName of the output (needs outputFolder).");

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

    // Parse options
    std::string filename = vm["input"].as<std::string>();
    std::string reference_image_filename =
            vm["reference_image"].as<std::string>();
    std::string vertex_to_label_map_filename =
            vm["vertex_to_label_map_file"].as<std::string>();
    std::string edge_to_label_map_filename =
            vm["edge_to_label_map_file"].as<std::string>();
    const bool graph_positions_are_in_physical_space =
            !!!vm["graph_positions_are_in_index_space"].as<bool>();
    bool verbose = vm["verbose"].as<bool>();
    if (vm.count("outputFolder")) {
        const fs::path output_folder_path{vm["outputFolder"].as<std::string>()};
        if (!fs::exists(output_folder_path)) {
            std::cerr << "output folder doesn't exist : "
                      << output_folder_path.string() << std::endl;
            throw po::validation_error(
                    po::validation_error::invalid_option_value,
                    "output_folder_path");
        }
    }

    const fs::path input_stem = fs::path(filename).stem();
    fs::path output_full_path;
    fs::path output_folder_path;
    fs::path output_file_path;
    /*-------------- End of parse -----------------------------*/
    // Get filename without extension (and without folders).
    if (vm.count("outputFolder")) {
        output_folder_path = vm["outputFolder"].as<std::string>();
    }

    if (vm.count("outputFilename")) {
        output_file_path = fs::path(vm["outputFilename"].as<std::string>());
        output_full_path =
                output_folder_path / fs::path(output_file_path.string());
    } else {
        std::string output_file_string = input_stem.string() + "_voxelized";
        output_file_path = fs::path(output_file_string);
        output_full_path = output_folder_path /
                           fs::path(output_file_path.string() + ".nrrd");
    }
    if (verbose) {
        std::cout << "output_folder_path: " << output_folder_path.string()
                  << std::endl;
        std::cout << "output_file_path: " << output_file_path.string()
                  << std::endl;
        std::cout << "output_full_path: " << output_full_path.string()
                  << std::endl;
    }

    // Read Graph
    const fs::path input_extension = fs::path(filename).extension();
    const bool is_graphviz_graph = (input_extension == ".dot") ? true : false;
    const auto graph = (is_graphviz_graph) ? SG::read_graphviz_sg(filename)
                                           : SG::read_serialized_sg(filename);
    // Read reference_image
    using PixelType = unsigned char;
    const unsigned int Dim = 3;
    using ImageType = itk::Image<PixelType, Dim>;
    using ReaderType = itk::ImageFileReader<ImageType>;
    auto reader = ReaderType::New();
    reader->SetFileName(reference_image_filename);
    reader->Update();

    // Read vertex_to_label_map
    const auto vertex_to_label_map =
            SG::read_vertex_to_label_map(vertex_to_label_map_filename);
    // Read edge_to_label_map
    const auto edge_to_label_map =
            (edge_to_label_map_filename == "max")
                    ? SG::create_edge_to_label_map_from_vertex_to_label_map_using_max(
                              graph, vertex_to_label_map)
                    : SG::read_edge_to_label_map(edge_to_label_map_filename);

    const auto voxelized_image = SG::voxelize_graph(
            graph, reader->GetOutput(), vertex_to_label_map, edge_to_label_map,
            graph_positions_are_in_physical_space);

    using WriterType = itk::ImageFileWriter<ImageType>;
    auto writer = WriterType::New();
    try {
        writer->SetFileName(output_full_path.string().c_str());
        writer->SetInput(voxelized_image);
        writer->UseCompressionOn();
        writer->Update();
    } catch (itk::ExceptionObject &e) {
        std::cerr << "Failure writing file: " << output_full_path.string()
                  << std::endl;
        std::cerr << e.what() << std::endl;
        throw e;
    }
    if (verbose) {
        std::cout << "Voxelize Graph finished." << std::endl;
        std::cout << "Output: " << output_full_path.string() << std::endl;
    }
}
