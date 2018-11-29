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

#include <chrono>
#include <DGtal/base/Common.h>
#include <DGtal/helpers/StdDefs.h>
#include <DGtal/images/ImageContainerByITKImage.h>
#include "DGtal/images/imagesSetsUtils/SetFromImage.h"
#include "DGtal/images/imagesSetsUtils/ImageFromSet.h"
#include <DGtal/io/readers/ITKReader.h>
#include "DGtal/io/writers/ITKWriter.h"

#include "DGtal/geometry/volumes/distance/ExactPredicateLpSeparableMetric.h"
#include "DGtal/geometry/volumes/distance/DistanceTransformation.h"
#include "DGtal/images/SimpleThresholdForegroundPredicate.h"

// Invert
#include "itkInvertIntensityImageFilter.h"

// ITKWriter
#include "itkImageFileWriter.h"
#include "itkChangeInformationImageFilter.h"

// boost::program_options
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
// Boost Filesystem
#include <boost/filesystem.hpp>

using namespace DGtal;
using namespace std;
using namespace DGtal::Z3i;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

int main(int argc, char* const argv[]){

  /*-------------- Parse command line -----------------------------*/
  po::options_description general_opt ( "Allowed options are: " );
  general_opt.add_options()
    ( "help,h", "display this message." )
    ( "input,i", po::value<string>()->required(), "Input thin image." )
    ( "outputFolder,o", po::value<string>()->required(), "Output folder for the distance map." )
    // ( "visualize,t", po::bool_switch()->default_value(false), "Visualize object with DGtal. Requires VISUALIZE option enabled at build.")
    ( "foreground,f",  po::value<string>()->default_value("white"), "foreground color in binary image [black|white]" )
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

  std::string filename = vm["input"].as<string>();
  std::string outputFolder = vm["outputFolder"].as<string>();;
  bool verbose = vm["verbose"].as<bool>();
  string foreground = vm["foreground"].as<string>();
  if (vm.count("foreground") && (!(foreground == "white" || foreground == "black")))
    throw po::validation_error(po::validation_error::invalid_option_value, "foreground");
  bool invert_image = (foreground == "black") ? true : false ;

  // Get filename without extension (and without folders).
  const fs::path input_stem = fs::path(filename).stem();
  const fs::path output_file_path = fs::path(
      input_stem.string() + "_DMAP");

  // Typedefs
  using Domain = Z3i::Domain ;
  using Image = ImageContainerByITKImage<Domain, unsigned char> ;
  const unsigned int Dim = 3;
  using PixelType = unsigned char ;
  using ItkImageType = itk::Image<PixelType, Dim> ;

  // Read Image using ITK
  using ReaderType = itk::ImageFileReader<ItkImageType> ;
  auto reader = ReaderType::New();
  reader->SetFileName(filename);
  reader->Update();

  // Invert Filter using ITK.
  using InverterType =
    itk::InvertIntensityImageFilter<ItkImageType, ItkImageType> ;
  auto inverter = InverterType::New();
  if(invert_image) {
    inverter->SetInput(reader->GetOutput());
    inverter->Update();
  }

  Image::ITKImagePointer handle_out = (invert_image) ?
    Image::ITKImagePointer(inverter->GetOutput()) :
    Image::ITKImagePointer(reader->GetOutput());
  // Convert to DGtal Container
  Image image(handle_out);

  auto start = std::chrono::system_clock::now();
  trace.beginBlock("Create Distance Map");
  // using Predicate = Z3i::DigitalSet;
  using Predicate = DGtal::functors::SimpleThresholdForegroundPredicate<Image>;
  Predicate aPredicate(image,0);
  using L3Metric = ExactPredicateLpSeparableMetric<Z3i::Space, 3>;
  using DT       = DistanceTransformation<Z3i::Space, Predicate, L3Metric>;
  L3Metric l3;
  DT dt(image.domain(),aPredicate, l3);
  trace.endBlock();
  auto end = std::chrono::system_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::seconds> (end - start) ;
  if (verbose) std::cout <<"Time elapsed: " << elapsed.count() << std::endl;

  // Write the dt image, keeping the metadata of the original image.
  {
    using FloatPixelType = float;
    using FloatImage = ImageContainerByITKImage<Domain, FloatPixelType> ;
    using ItkFloatImageType = itk::Image<FloatPixelType, Dim> ;
    FloatImage dt_itk_image(dt.domain());
    std::copy(dt.constRange().begin(), dt.constRange().end(), dt_itk_image.range().outputIterator());

    using ChangeInformationFilter = itk::ChangeInformationImageFilter<ItkFloatImageType>;
    auto changeInfo = ChangeInformationFilter::New();
    changeInfo->ChangeAll();
    changeInfo->SetOutputOrigin(reader->GetOutput()->GetOrigin());
    changeInfo->SetOutputSpacing(reader->GetOutput()->GetSpacing());
    changeInfo->SetOutputDirection(reader->GetOutput()->GetDirection());
    changeInfo->SetInput(dt_itk_image.getITKImagePointer());
    changeInfo->Update();

    // Write the image
    using ITKImageWriter = itk::ImageFileWriter<ItkFloatImageType>;
    auto writer = ITKImageWriter::New();
    const fs::path output_folder_path{outputFolder};
    fs::path output_full_path = output_folder_path / fs::path(output_file_path.string() + ".nrrd");
    try
    {
      writer->SetFileName(output_full_path.string().c_str());
      writer->SetInput(changeInfo->GetOutput());
      writer->Update();
    }
    catch (itk::ExceptionObject &e)
    {
      std::cerr << "Failure writing file: " << output_full_path.string() << std::endl;
      trace.error() << e;
      throw IOException();
    }
  }
}
