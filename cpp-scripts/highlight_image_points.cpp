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
#include <chrono>
#include <unordered_map>
#include <sstream>
#include <DGtal/base/Common.h>
#include <DGtal/helpers/StdDefs.h>
#include <DGtal/io/readers/GenericReader.h>
#include <DGtal/io/readers/ITKReader.h>
#include <DGtal/images/ImageContainerByITKImage.h>
#include <DGtal/images/imagesSetsUtils/SetFromImage.h>
#include <DGtal/images/imagesSetsUtils/ImageFromSet.h>
// boost::program_options
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

// Boost Filesystem
#include <boost/filesystem.hpp>

// Viewer
#include <DGtal/io/Color.h>
#include <DGtal/io/colormaps/GradientColorMap.h>
#include <DGtal/io/DrawWithDisplay3DModifier.h>
#include <DGtal/io/viewers/Viewer3D.h>

using namespace DGtal;
using namespace std;
using namespace DGtal::Z3i;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

int main(int argc, char* const argv[]) {
  /*-------------- Parse command line -----------------------------*/
  po::options_description opt_desc("Allowed options are: ");
  opt_desc.add_options()("help,h", "display this message.");
  opt_desc.add_options()("input,i", po::value<string>()->required(),
                         "Input thin image.");
  opt_desc.add_options()("exportSDP,o", po::value<string>(),
                         "Write .sdp file the digital set points.");
  opt_desc.add_options()(
      "highlightRedPoints,p", po::value<string>(),
      "Give a string of points to highlight them in RED (require visualize "
      "ON). Example: \"1 3 4, 43 45 53\" ");
  opt_desc.add_options()(
      "highlightBluePoints,b", po::value<string>(),
      "Give a string of points to highlight them n BLUE (require visualize "
      "ON). Example: \"1 3 4, 43 45 53\" ");
  opt_desc.add_options()("visualize,t", po::bool_switch()->default_value(false),
                         "Visualize object with DGtal.");
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

  string filename = vm["input"].as<string>();
  bool verbose = vm["verbose"].as<bool>();
  bool visualize = vm["visualize"].as<bool>();
  bool highlightRedPoints = vm.count("highlightRedPoints");
  string highlightRedPoints_input =
      highlightRedPoints ? vm["highlightRedPoints"].as<string>() : "";
  bool highlightBluePoints = vm.count("highlightBluePoints");
  string highlightBluePoints_input =
      highlightBluePoints ? vm["highlightBluePoints"].as<string>() : "";
  bool exportSDP = vm.count("exportSDP");
  string exportSDP_filename = exportSDP ? vm["exportSDP"].as<string>() : "";
  // Get filename without extension (and without folders).
  const fs::path input_stem = fs::path(filename).stem();
  const fs::path output_file_path = fs::path(input_stem.string() + "_points");

  using Domain = Z3i::Domain;
  using Image = ImageContainerByITKImage<Domain, unsigned char>;
  const unsigned int Dim = 3;
  using PixelType = unsigned char;
  using ItkImageType = itk::Image<PixelType, Dim>;
  // Read Image using ITK
  using ReaderType = itk::ImageFileReader<ItkImageType>;
  auto reader = ReaderType::New();
  reader->SetFileName(filename);
  reader->Update();
  auto itk_image = reader->GetOutput();
  Image image(itk_image);

  using DigitalSet = DGtal::DigitalSetByAssociativeContainer<
      Domain, std::unordered_set<typename Domain::Point> >;

  DigitalSet image_set(image.domain());
  SetFromImage<Z3i::DigitalSet>::append<Image>(image_set, image, 0, 255);

  if(visualize) {
    KSpace ks;
    // Domain of kspace must be padded.
    ks.init(image.domain().lowerBound(), image.domain().upperBound(), true);
    int argc(1);
    char** argv(nullptr);
    QApplication app(argc, argv);
    Viewer3D<> viewer(ks);
    viewer.show();

    DigitalSet highlight_red_set(image.domain());
    if(highlightRedPoints) {
      std::cout << "highlighting RED points" << std::endl;
      std::istringstream ss(highlightRedPoints_input);
      std::cout << ss.str() << std::endl;
      std::string spoint;
      Domain::Point p;
      Image::ITKImage::PointType itk_point;
      Image::ITKImage::IndexType itk_index;
      // 12 9 131, 23 44 5
      while(std::getline(ss, spoint, ',')) {
        std::istringstream sspoint(spoint);
        std::cout << sspoint.str() << std::endl;
        // 12 9 131
        sspoint >> itk_point[0] >> itk_point[1] >> itk_point[2];
        itk_image->TransformPhysicalPointToIndex(itk_point, itk_index);
        p[0] = itk_index[0];
        p[1] = itk_index[1];
        p[2] = itk_index[2];
        std::cout << p << std::endl;
        highlight_red_set.insert(p);
      }

      viewer.setFillColor(Color(255, 0, 0, 255));
      viewer << highlight_red_set;
    }

    DigitalSet highlight_blue_set(image.domain());
    if(highlightBluePoints) {
      std::cout << "highlighting BLUE points" << std::endl;
      std::istringstream ss(highlightBluePoints_input);
      std::cout << ss.str() << std::endl;
      std::string spoint;
      Domain::Point p;
      Image::ITKImage::PointType itk_point;
      Image::ITKImage::IndexType itk_index;
      // 12 9 131, 23 44 5
      while(std::getline(ss, spoint, ',')) {
        std::istringstream sspoint(spoint);
        std::cout << sspoint.str() << std::endl;
        // 12 9 131
        sspoint >> itk_point[0] >> itk_point[1] >> itk_point[2];
        itk_image->TransformPhysicalPointToIndex(itk_point, itk_index);
        p[0] = itk_index[0];
        p[1] = itk_index[1];
        p[2] = itk_index[2];
        std::cout << p << std::endl;
        highlight_blue_set.insert(p);
      }

      viewer.setFillColor(Color(0, 0, 255, 255));
      viewer << highlight_blue_set;
    }

    viewer.setFillColor(Color(255, 255, 255, 250));
    viewer << image_set;

    viewer << Viewer3D<>::updateDisplay;

    app.exec();
  }

  if(exportSDP) {
    const fs::path output_folder_path{exportSDP_filename};
    fs::path output_full_path =
        output_folder_path / fs::path(output_file_path.string() + ".sdp");
    std::ofstream out;
    out.open(output_full_path.string().c_str());
    out << "# file : " << input_stem.string() << std::endl;
    for(auto& p : image_set) {
      out << p[0] << " " << p[1] << " " << p[2] << std::endl;
    }
    if(verbose) {
      std::cout << "list of points (SDP) output in: "
                << output_full_path.string() << std::endl;
    }
  }
}
