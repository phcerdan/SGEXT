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

#include "thin_function.hpp"

// Boost Filesystem
#include <boost/filesystem.hpp>

#include <DGtal/base/Common.h>
#include <DGtal/helpers/StdDefs.h>
#include <DGtal/io/readers/GenericReader.h>
#include <DGtal/io/readers/ITKReader.h>
#include <DGtal/io/writers/ITKWriter.h>
#include <DGtal/images/ImageContainerByITKImage.h>
#include <DGtal/images/imagesSetsUtils/SetFromImage.h>
#include <DGtal/images/imagesSetsUtils/ImageFromSet.h>
#include "DGtal/topology/KhalimskyCellHashFunctions.h"

#include <DGtal/topology/VoxelComplex.h>
#include <DGtal/topology/VoxelComplexFunctions.h>

#include <DGtal/topology/NeighborhoodConfigurations.h>
#include <DGtal/topology/tables/NeighborhoodTables.h>

// Iterate for sequence discrete points
#include <itkIndexRange.h>
// Invert
#include <itkInvertIntensityImageFilter.h>
#include <itkNumericTraits.h>
// ITKWriter
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkChangeInformationImageFilter.h>

#ifdef VISUALIZE
// Viewer
#include <DGtal/io/Color.h>
#include <DGtal/io/colormaps/GradientColorMap.h>
#include <DGtal/io/DrawWithDisplay3DModifier.h>
#include <DGtal/io/viewers/Viewer3D.h>
#endif

namespace SG {

BinaryImageType::Pointer thin_function(
    const BinaryImageType::Pointer & input_image,
    const std::string & skel_type_str,
    const std::string & skel_select_type_str,
    const int & persistence,
    const FloatImageType::Pointer & distance_map_image,
    const bool profile,
    const bool verbose,
    const bool visualize
    ) {
  if(verbose) {
    using DGtal::trace;
    trace.beginBlock("thin_function parameters:");
    trace.info() << "skel_type_str: " << skel_type_str << std::endl;
    trace.info() << "skel_select_type_str: " << skel_select_type_str << std::endl;
    if(distance_map_image) {
      trace.info() << " -- provided distance_map_image." << std::endl;
    }
    trace.info() << "persistence: " << persistence << std::endl;
    trace.info() << "profile: " << profile << std::endl;
    trace.info() << "verbose: " << verbose << std::endl;
    trace.info() << "visualize: " << visualize << std::endl;
    trace.info() << "----------" << std::endl;
    trace.endBlock();
  }

  // Validate input skel method and skel_select
  auto skel_type = skel_string_to_enum(skel_type_str);
  auto skel_select_type = skel_select_string_to_enum(skel_select_type_str);

  // Convert to DGtal Container
  using Domain = DGtal::Z3i::Domain;
  using Image = DGtal::ImageContainerByITKImage<Domain, unsigned char>;
  using ItkImageType = BinaryImageType;
  Image image(input_image);

  // Create a VoxelComplex from the set
  using DigitalTopology = DGtal::Z3i::DT26_6;
  using KSpace = DGtal::Z3i::KSpace;
  using Cell = DGtal::Z3i::Cell;
  using DigitalSet =  // DGtal::Z3i::DigitalSet;
      DGtal::DigitalSetByAssociativeContainer<
          Domain, std::unordered_set<typename Domain::Point> >;
  using ComplexMap = std::unordered_map<Cell, DGtal::CubicalCellData>;
  using Complex = DGtal::VoxelComplex<KSpace, ComplexMap>;
  using Point = DGtal::Z3i::Point;

  KSpace ks;
  // TODO
  // TODO
  // FIX THIS
  // Domain of kspace must be padded. TODO: pad might not be needed anymore
  KSpace::Point d1(KSpace::Point::diagonal(1));
  ks.init(image.domain().lowerBound() - d1, image.domain().upperBound() + d1,
          true);
  DigitalTopology::ForegroundAdjacency adjF;
  DigitalTopology::BackgroundAdjacency adjB;
  DigitalTopology topo(adjF, adjB, DGtal::DigitalTopologyProperties::JORDAN_DT);

  if(verbose) DGtal::trace.beginBlock("construct with table");
  Complex vc(ks);
  // Optimization, Construct in place to save memory. vc stores object.
  {
    DigitalSet image_set(image.domain());
    // Get the whole image, segmentation is outside the scope of this function
    // Assume BinaryImagePixelType is of type uchar
    static_assert(std::is_same<BinaryImagePixelType, unsigned char>::value,
        "BinaryImagePixelType is not uchar, change threshold values.");
    DGtal::SetFromImage<DGtal::Z3i::DigitalSet>::append<Image>(
        image_set, image, 0, 255);

    vc.construct(image_set);
  }
  vc.setSimplicityTable(DGtal::functions::loadTable(DGtal::simplicity::tableSimple26_6));
  if(verbose) DGtal::trace.endBlock();

  if(verbose) DGtal::trace.beginBlock("load isthmus table");
  boost::dynamic_bitset<> isthmus_table;
  auto &sk = skel_type;
  if(sk == SkelType::isthmus)
    isthmus_table = *DGtal::functions::loadTable(DGtal::isthmusicity::tableIsthmus);
  else if(sk == SkelType::isthmus1)
    isthmus_table = *DGtal::functions::loadTable(DGtal::isthmusicity::tableOneIsthmus);
  if(verbose) DGtal::trace.endBlock();


  // SKEL FUNCTION:
  // Load a look-up-table for the neighborgood of a point
  auto pointMap =
      *DGtal::functions::mapZeroPointNeighborhoodToConfigurationMask<Point>();
  std::function<bool(const Complex&, const Cell&)> Skel;
  if(sk == SkelType::ultimate)
    Skel = DGtal::functions::skelUltimate<Complex>;
  else if(sk == SkelType::end)
    Skel = DGtal::functions::skelEnd<Complex>;
  // else if (sk == "1is") Skel = oneIsthmus<Complex>;
  // else if (sk == "is") Skel = skelIsthmus<Complex>;
  else if(sk == SkelType::isthmus1)
    Skel = [&isthmus_table, &pointMap](const Complex& fc,
                                       const Complex::Cell& c) {
      return DGtal::functions::skelWithTable(isthmus_table, pointMap, fc, c);
    };
  else if(sk == SkelType::isthmus)
    Skel = [&isthmus_table, &pointMap](const Complex& fc,
                                       const Complex::Cell& c) {
      return DGtal::functions::skelWithTable(isthmus_table, pointMap, fc, c);
    };
  else
    throw std::runtime_error("Invalid skel string");

  // SELECT FUNCTION
  std::function<std::pair<typename Complex::Cell, typename Complex::Data>(
      const Complex::Clique&)>
      Select;

  // profile
  auto start = std::chrono::system_clock::now();
  // Crate a dummy distance map for the posibility of loading
  // the optional distance map image.
  using DistanceMapPixelType = float;
  using DistanceMapImage =
      DGtal::ImageContainerByITKImage<Domain, DistanceMapPixelType>;
  DGtal::Z3i::Domain dummyDomain(Point(0, 0, 0), Point(1, 1, 1));
  DistanceMapImage distanceMapImage(dummyDomain);

  auto &sel = skel_select_type;
  if(sel == SkelSelectType::random)
    Select = DGtal::functions::selectRandom<Complex>;
  else if(sel == SkelSelectType::first)
    Select = DGtal::functions::selectFirst<Complex>;
  else if(sel == SkelSelectType::dmax) {
    if(verbose) DGtal::trace.beginBlock("Import Distance Map");
    distanceMapImage = DistanceMapImage(distance_map_image);
    if(verbose) DGtal::trace.endBlock();
    Select = [&distanceMapImage](const Complex::Clique& clique) {
      return DGtal::functions::selectMaxValue<DistanceMapImage, Complex>(distanceMapImage,
                                                       clique);
    };
  } else
    throw std::runtime_error("Invalid skel select type");

  // Perform the thin/skeletonization
  Complex vc_new(ks);
  if(persistence == 0)
    vc_new = DGtal::functions::asymetricThinningScheme<Complex>(vc, Select, Skel, verbose);
  else
    vc_new = DGtal::functions::persistenceAsymetricThinningScheme<Complex>(vc, Select, Skel,
                                                         persistence, verbose);

  // profile
  auto end = std::chrono::system_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(end - start);
  if(profile) std::cout << "Time elapsed: " << elapsed.count() << std::endl;


  // Convert back to ITK Image
  DigitalSet thin_set(image.domain());
  vc_new.dumpVoxels(thin_set);

  Image thin_image(image.domain());
  unsigned int foreground_value = 255;
  DGtal::ImageFromSet<Image>::append<DigitalSet>(thin_image, thin_set, foreground_value);
  // Copy metadata information (spacing, origin, direction) from original
  // image to the output.
  using ChangeInformationFilter =
    itk::ChangeInformationImageFilter<ItkImageType>;
  auto changeInfo = ChangeInformationFilter::New();
  changeInfo->ChangeAll();
  changeInfo->UseReferenceImageOn();
  changeInfo->SetReferenceImage(input_image);
  changeInfo->SetInput(thin_image.getITKImagePointer());
  changeInfo->Update();

#ifdef VISUALIZE
  if(visualize) {
    DigitalSet all_set(image.domain());
    vc.dumpVoxels(all_set);
    int argc(1);
    char** argv(nullptr);
    QApplication app(argc, argv);
    DGtal::Viewer3D<> viewer(ks);
    viewer.show();

    viewer.setFillColor(DGtal::Color(255, 255, 255, 255));
    viewer << thin_set;

    // All kspace voxels
    viewer.setFillColor(DGtal::Color(40, 200, 55, 10));
    viewer << all_set;

    viewer << DGtal::Viewer3D<>::updateDisplay;

    app.exec();
  }
#endif

  return changeInfo->GetOutput();

}
// TODO replace filename for ImageType, and generate other interfaces with
// filename filename was used in script mode, but with python wrapping in mind,
// we can provide: a) numpy interface (compatible with ITK python wrap) +
// origin, spacing, cosines b) itk image in c++ c) actual filename
BinaryImageType::Pointer thin_function_io(const std::string &filename,
        const std::string & skel_type_str,
        const std::string & skel_select_type_str,
        const std::string & output_foldername,
        const int & persistence,
        const std::string & inputDistanceMapImageFilename,
        const std::string & foreground,
        const std::string & out_sequence_discrete_points_foldername,
        const bool profile,
        const bool verbose,
        const bool visualize,
        const int thresholdMin,
        const int thresholdMax
        ) {
  if(verbose) {
    using DGtal::trace;
    trace.beginBlock("thin_function_io exclusive parameters:");
    trace.info() << "output_foldername: " << output_foldername << std::endl;
    trace.info() << "inputDistanceMapImageFilename: " <<
      inputDistanceMapImageFilename << std::endl;
    trace.info() << "foreground: " << foreground << std::endl;
    trace.info() << "out_sequence_discrete_points_foldername: " <<
      out_sequence_discrete_points_foldername << std::endl;
    trace.info() << "thresholdMin: " << thresholdMin << std::endl;
    trace.info() << "thresholdMax: " << thresholdMax << std::endl;
    trace.info() << "----------" << std::endl;
    trace.endBlock();
  }
  // Validate input skel method and skel_select
  auto skel_type = skel_string_to_enum(skel_type_str);
  auto skel_select_type = skel_select_string_to_enum(skel_select_type_str);

  // Get filename without extension (and without folders).
  namespace fs = boost::filesystem;
  const fs::path input_stem = fs::path(filename).stem();
  std::string output_file_string = input_stem.string() + "_SKEL";
  output_file_string += "_" + to_string(skel_select_type) +
      "_" + to_string(skel_type) + "_p" +
      std::to_string(persistence);

  const fs::path output_file_path = fs::path(output_file_string);
  using Domain = DGtal::Z3i::Domain;
  using Image = DGtal::ImageContainerByITKImage<Domain, unsigned char>;
  auto Dim = BinaryImageDimension;
  using PixelType = BinaryImagePixelType;
  using ItkImageType = BinaryImageType;

  // Read image
  using ReaderType = itk::ImageFileReader<ItkImageType>;
  auto reader = ReaderType::New();
  reader->SetFileName(filename);
  reader->Update();
  auto input_image = reader->GetOutput();


  // Apply itk filters.
  // Dev: This might be refactored to be done outside this function
  const bool invert_image = (foreground == "black") ? true : false;
  using InverterType =
      itk::InvertIntensityImageFilter<ItkImageType, ItkImageType>;
  auto inverter = InverterType::New();
  if(invert_image) {
    inverter->SetInput(reader->GetOutput());
    inverter->Update();
  }
  // Apply filters if neccesary
  Image::ITKImagePointer handle_out =
      (invert_image) ? Image::ITKImagePointer(inverter->GetOutput())
                     : Image::ITKImagePointer(input_image);


  using ItkFloatImageType = SG::FloatImageType;
  using DGtalFloatImage = DGtal::ImageContainerByITKImage<Domain,
        SG::FloatImageType::PixelType>;
  typename ItkFloatImageType::Pointer distance_map_itk_image = nullptr;
  if(!inputDistanceMapImageFilename.empty()) {
      using DistanceMapReaderType = itk::ImageFileReader<ItkFloatImageType>;
      auto dmap_reader = DistanceMapReaderType::New();
      reader->SetFileName(inputDistanceMapImageFilename);
      reader->Update();
      distance_map_itk_image = dmap_reader->GetOutput();
  }

  auto thin_image = thin_function(handle_out, skel_type_str, skel_select_type_str, persistence, distance_map_itk_image, profile, verbose, visualize);

  // Export
  // Export sequence of discrete points
  if(!out_sequence_discrete_points_foldername.empty()) {
    const fs::path output_folder_path{out_sequence_discrete_points_foldername};
    if(!fs::exists(output_folder_path)) {
      throw std::runtime_error(
          "output folder for discrete points file doesn't exist : " +
          output_folder_path.string());
    }
    fs::path output_full_path =
        output_folder_path / fs::path(output_file_path.string() + ".sdp");
    std::ofstream out;
    out.open(output_full_path.string().c_str());
    const auto range =
      itk::Experimental::IndexRange<BinaryImageDimension, false>(
          thin_image->GetLargestPossibleRegion());
    for(const auto& index : range) {
      if (thin_image->GetPixel(index) > 0) {
        out << index[0] << " " << index[1] << " " << index[2] << std::endl;
      }
    }
  }

  // Export thin image
  if(output_foldername.empty()) {
    throw std::runtime_error("provide output_foldername in thin_function");
  }
  const fs::path output_folder_path{output_foldername};
  if(!fs::exists(output_folder_path)) {
    throw std::runtime_error(
        "output folder for output thin image doesn't exist : " +
        output_folder_path.string());
  }
  fs::path output_full_path =
    output_folder_path / fs::path(output_file_path.string() + ".nrrd");

  // Write the image
  using ITKImageWriter = itk::ImageFileWriter<ItkImageType>;
  auto writer = ITKImageWriter::New();
  try {
    writer->SetFileName(output_full_path.string().c_str());
    writer->SetInput(thin_image);
    writer->Update();
  } catch(itk::ExceptionObject& e) {
    std::cerr << "Failure writing file: " << output_full_path.string()
      << std::endl;
    DGtal::trace.error() << e;
    throw DGtal::IOException();
  }

  return thin_image;

};
} // end namespace SG
