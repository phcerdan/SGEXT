/* ********************************************************************
 * Copyright (C) 2021 Pablo Hernandez-Cerdan.
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

#include "visualize_with_label_image.hpp"

#include <iostream>
#include <chrono>
#include <unordered_map>
// boost::program_options
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include <itkImageFileReader.h>

using namespace std;
namespace po = boost::program_options;

int main(int argc, char *const argv[]) {
    /*-------------- Parse command line -----------------------------*/
    po::options_description opt_desc("Allowed options are: ");
    opt_desc.add_options()("help,h", "display this message.");
    opt_desc.add_options()(
            "inputImage,i", po::value<string>()->required(),
            "Input image of any type.");
    opt_desc.add_options()(
            "inputLabelImage,l", po::value<string>()->required(),
            "Input label/binary Image in the same region than inputImage.");
    opt_desc.add_options()("opacity,o", po::value<double>()->default_value(0.5),
                           "Opacity value.");
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

    string filenameImage = vm["inputImage"].as<string>();
    string filenameLabelImage = vm["inputLabelImage"].as<string>();
    double opacity = vm["opacity"].as<double>();
    bool verbose = vm["verbose"].as<bool>();
    if (verbose) {
        std::cout << "Filename Input Image: " << filenameImage << std::endl;
        std::cout << "Filename Input Label Image: " << filenameLabelImage << std::endl;
        std::cout << "Opacity: " << opacity << std::endl;
    }

    // Read images
    const unsigned int Dim = 3;

    using InputPixelType = float;
    using InputImageType = itk::Image<InputPixelType, Dim>;
    using InputReaderType = itk::ImageFileReader<InputImageType>;
    auto readerInput = InputReaderType::New();
    readerInput->SetFileName(filenameImage);
    readerInput->Update();
    auto * inputImage = readerInput->GetOutput();

    using LabelPixelType = unsigned char;
    using LabelImageType = itk::Image<LabelPixelType, Dim>;
    using LabelReaderType = itk::ImageFileReader<LabelImageType>;
    auto readerLabel = LabelReaderType::New();
    readerLabel->SetFileName(filenameLabelImage);
    readerLabel->Update();
    auto * labelImage = readerLabel->GetOutput();

    SG::view_image_with_label(inputImage, labelImage, opacity);
} // end main
