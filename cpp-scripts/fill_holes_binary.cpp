/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <iostream>
#include <string>
// ITK
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkInvertIntensityImageFilter.h>
#include <itkVotingBinaryIterativeHoleFillingImageFilter.h> // Module ITKLabelVoting
// boost::program_options
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
// boost::filesystem
#include <boost/filesystem.hpp>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

int main(int argc, char* const argv[]){

    /*-------------- Parse command line -----------------------------*/
    po::options_description general_opt ( "Allowed options are: " );
    general_opt.add_options()
        ( "help,h", "display this message." )
        ( "input,i", po::value<std::string>()->required(), "Input 3D binary image file." )
        ( "foreground,f",  po::value<std::string>()->default_value("white"), "foreground color in binary image. [black|white]" )
        ( "thresholdMin,m",  po::value<int>()->default_value(0), "threshold min (excluded) to define binary shape" )
        ( "thresholdMax,M",  po::value<int>()->default_value(255), "threshold max (included) to define binary shape" )
        ( "majority",  po::value<int>()->default_value(3), "Majority needed in the neighborhood to switch pixel to ON" )
        ( "radius",  po::value<int>()->default_value(1), "Radius to define the neighborhood" )
        ( "iterations",  po::value<size_t>()->default_value(1000), "Number of max iterations" )
    ( "output_filename_simple,z",  po::bool_switch()->default_value(false), "Filename does not contain the parameters used for this filter." )
        ( "verbose,v",  po::bool_switch()->default_value(false), "verbose output" )
        ( "outputFolder,o", po::value<std::string>()->required(), "Folder to export the resulting binary image.")
        ( "outputFilename,e", po::value<std::string>(), "FileName of the output (needs outputFolder).")
        ( "visualize,t", po::bool_switch()->default_value(false), "Visualize thin result. Requires VISUALIZE option at build");

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

    //Parse options
    std::string filename = vm["input"].as<std::string>();
    int majority = vm["majority"].as<int>();
    int radius = vm["radius"].as<int>();
    size_t iterations = vm["iterations"].as<size_t>();
    bool verbose = vm["verbose"].as<bool>();
    bool output_filename_simple = vm["output_filename_simple"].as<bool>();
    int thresholdMin = vm["thresholdMin"].as<int>();
    int thresholdMax = vm["thresholdMax"].as<int>();
    std::string foreground = vm["foreground"].as<std::string>();
    if (vm.count("foreground") && (!(foreground == "white" || foreground == "black")))
        throw po::validation_error(po::validation_error::invalid_option_value, "foreground");
    bool invert_image = (foreground == "black") ? true : false ;
#ifdef VISUALIZE
    bool visualize = vm["visualize"].as<bool>();
#endif
    if (vm.count("outputFolder"))
    {
        const fs::path output_folder_path{vm["outputFolder"].as<std::string>()};
        if(!fs::exists(output_folder_path)) {
            std::cerr << "output folder doesn't exist : " << output_folder_path.string() << std::endl;
            throw po::validation_error(po::validation_error::invalid_option_value, "output_folder_path");
        }
    }

    const fs::path input_stem = fs::path(filename).stem();
    fs::path output_full_path;
    fs::path output_folder_path;
    fs::path output_file_path;
    /*-------------- End of parse -----------------------------*/
    // Get filename without extension (and without folders).
    if (vm.count("outputFolder"))
    {
        output_folder_path = vm["outputFolder"].as<std::string>();
    }

    if(vm.count("outputFilename")) {
        output_file_path = fs::path(vm["outputFilename"].as<std::string>());
        output_full_path = output_folder_path / fs::path(output_file_path.string());
    } else {
        std::string output_file_string =
                input_stem.string() + "_FILLED";
        if(!output_filename_simple) {
            output_file_string +=
                "_M" + std::to_string(majority) +
                "_R" + std::to_string(radius) +
                "_N" + std::to_string(iterations);
        }
        output_file_path = fs::path(output_file_string);
        output_full_path = output_folder_path / fs::path(output_file_path.string() + ".nrrd");
    }
    if(verbose) {
        std::cout << "output_folder_path: " << output_folder_path.string() << std::endl;
        std::cout << "output_file_path: " << output_file_path.string() << std::endl;
        std::cout << "output_full_path: " << output_full_path.string() << std::endl;
    }

    // Read Image using ITK
    using PixelType = unsigned char;
    const unsigned int Dim = 3;
    using ImageType = itk::Image<PixelType, Dim> ;
    using ReaderType = itk::ImageFileReader<ImageType> ;
    auto reader = ReaderType::New();
    reader->SetFileName(filename);
    reader->Update();

    // Invert Filter using ITK.
    using InverterType = itk::InvertIntensityImageFilter<ImageType, ImageType> ;
    auto inverter = InverterType::New();
    if(invert_image) {
        inverter->SetInput(reader->GetOutput());
        inverter->Update();
    }
    ImageType::Pointer image = (invert_image) ?
        inverter->GetOutput() : reader->GetOutput();

    using FillingFilterType = itk::VotingBinaryIterativeHoleFillingImageFilter<ImageType>;
    auto filler = FillingFilterType::New();
    filler->SetInput(image);
    filler->SetMajorityThreshold(majority);
    filler->SetMaximumNumberOfIterations(iterations);
    FillingFilterType::InputSizeType radius_array;
    radius_array.Fill(radius);
    filler->SetRadius(radius_array);

    using WriterType = itk::ImageFileWriter<ImageType>;
    auto writer = WriterType::New();
    try {
        writer->SetFileName(output_full_path.string().c_str());
        writer->SetInput(filler->GetOutput());
        writer->Update();
    } catch (itk::ExceptionObject &e) {
        std::cerr << "Failure writing file: " << output_full_path.string() << std::endl;
        std::cerr << e.what() << std::endl;
        throw e;
    }
    if(verbose) {
        std::cout << "Filling holes finished" << std::endl;
        std::cout << "Output: " << output_full_path.string() << std::endl;
    }
}
