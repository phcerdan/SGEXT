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
#include <DGtal/base/Common.h>
#include <DGtal/helpers/StdDefs.h>
#include <DGtal/io/readers/GenericReader.h>
#include <DGtal/io/readers/ITKReader.h>
#include "DGtal/io/writers/ITKWriter.h"
#include <DGtal/images/ImageContainerByITKImage.h>
#include "DGtal/images/imagesSetsUtils/SetFromImage.h"
#include "DGtal/images/imagesSetsUtils/ImageFromSet.h"
// #include "DGtal/images/SimpleThresholdForegroundPredicate.h"
#include "DGtal/images/ImageSelector.h"

#include <DGtal/topology/SurfelAdjacency.h>
#include <DGtal/io/boards/Board2D.h>
#include <DGtal/topology/CubicalComplex.h>
#include <DGtal/topology/CubicalComplexFunctions.h>

#include <DGtal/topology/VoxelComplex.h>
#include <DGtal/topology/VoxelComplexFunctions.h>
#include "DGtal/topology/NeighborhoodConfigurations.h"
#include "DGtal/topology/tables/NeighborhoodTables.h"
// ITKWriter
#include <itkImageFileWriter.h>

// Invert
#include "itkInvertIntensityImageFilter.h"
#include <itkNumericTraits.h>
// boost::program_options
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
// Distance Map
#include "DGtal/kernel/BasicPointPredicates.h"
#include "DGtal/images/SimpleThresholdForegroundPredicate.h"
#include "DGtal/geometry/volumes/distance/ExactPredicateLpSeparableMetric.h"
#include "DGtal/geometry/volumes/distance/VoronoiMap.h"
#include "DGtal/geometry/volumes/distance/DistanceTransformation.h"

// Boost Graph:
#include "DGtal/topology/Object.h"
#include "DGtal/graph/ObjectBoostGraphInterface.h"
#include <boost/graph/graph_concepts.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/filtered_graph.hpp>

// Boost Filesystem
#include <boost/filesystem.hpp>
#ifdef VISUALIZE
// Viewer
#include "DGtal/io/Color.h"
#include "DGtal/io/colormaps/GradientColorMap.h"
#include "DGtal/io/DrawWithDisplay3DModifier.h"
#include <DGtal/io/viewers/Viewer3D.h>
#endif


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
    ( "input,i", po::value<string>()->required(), "Input 3D image file." )
    ( "skel,s",  po::value<string>()->required(), "type of skeletonization. Valid: 1isthmus, isthmus, end, ulti" )
    ( "select,c",  po::value<string>()->required(), "select method for skeletonization. Valid: dmax, random, first" )
    ( "foreground,f",  po::value<string>()->default_value("black"), "foreground color in binary image" )
    ( "thresholdMin,m",  po::value<int>()->default_value(0), "threshold min (excluded) to define binary shape" )
    ( "thresholdMax,M",  po::value<int>()->default_value(255), "threshold max (included) to define binary shape" )
    ( "persistence,p",  po::value<int>()->default_value(0), "persistence value, implies use of persistence algorithm if p>=1" )
    ( "profile",  po::bool_switch()->default_value(false), "profile algorithm" )
    ( "verbose,v",  po::bool_switch()->default_value(false), "verbose output" )
    ( "visualize,t", po::bool_switch()->default_value(false), "Visualize thin result. Requires VISUALIZE option at build")
    ( "exportSDP,e", po::value<std::string>(), "Folder to export the resulting set of points in a simple (sequence of discrete point (sdp)).")
    ( "exportImage,o", po::value<std::string>(), "Folder to export the resulting set of points as an ITK Image.")
    ( "inputDistanceMapImageFilename,d", po::value<string>(), "Input 3D Distance Map Image from script create_distance_map. Used with option --select=dmax" );

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
  string filename = vm["input"].as<string>();
  bool verbose = vm["verbose"].as<bool>();
  bool profile = vm["profile"].as<bool>();
  int thresholdMin = vm["thresholdMin"].as<int>();
  int thresholdMax = vm["thresholdMax"].as<int>();
  int persistence = vm["persistence"].as<int>();
  if (vm.count("persistence") && persistence < 0 )
    throw po::validation_error(po::validation_error::invalid_option_value, "persistence");
  string foreground = vm["foreground"].as<string>();
  if (vm.count("foreground") && (!(foreground == "white" || foreground == "black")))
    throw po::validation_error(po::validation_error::invalid_option_value, "foreground");
  bool invert_image = (foreground == "black") ? true : false ;

  string sk_string = vm["skel"].as<string>();
  if (vm.count("skel") &&
     (!( sk_string == "ulti" || sk_string == "end" ||
         sk_string == "isthmus" || sk_string == "1isthmus"))
     )
     throw po::validation_error(po::validation_error::invalid_option_value, "skel");
  string select_string = vm["select"].as<string>();
  if (vm.count("select") &&
     (!( select_string == "random" || select_string == "dmax" ||
         select_string == "first" ))
     )
     throw po::validation_error(po::validation_error::invalid_option_value, "select");

#ifdef VISUALIZE
  bool visualize = vm["visualize"].as<bool>();
#endif

  if (vm.count("exportImage"))
  {
    const fs::path output_folder_path{vm["exportImage"].as<std::string>()};
    if(!fs::exists(output_folder_path)) {
      std::cerr << "output folder doesn't exist : " << output_folder_path.string() << std::endl;
      throw po::validation_error(po::validation_error::invalid_option_value, "output_folder_path");
    }
  }

  if (vm.count("exportSDP"))
  {
    const fs::path output_folder_path{vm["exportSDP"].as<std::string>()};
    if(!fs::exists(output_folder_path)) {
      std::cerr << "output folder doesn't exist : " << output_folder_path.string() << std::endl;
      throw po::validation_error(po::validation_error::invalid_option_value, "output_folder_path");
    }
  }

  if(select_string == "dmax" && !vm.count("inputDistanceMapImageFilename"))
  {
      std::cerr << "Please select an inputDistanceMapImageFilename.\n";
      std::cerr << "A distance map can be generated using the script:\n";
      std::cerr << "  create_distance_map -i inputImage -o outputFolder \n";
      throw po::validation_error(po::validation_error::invalid_option_value, "inputDistanceMapImageFilename");
  }

  string inputDistanceMapImageFilename = "";
  if (vm.count("inputDistanceMapImageFilename"))
  {
    const fs::path inputDistanceMapImageFilename_path{vm["inputDistanceMapImageFilename"].as<std::string>()};
    if(!fs::exists(inputDistanceMapImageFilename_path)) {
      std::cerr << "input distance map does not exist : " << inputDistanceMapImageFilename_path.string() << std::endl;
      throw po::validation_error(po::validation_error::invalid_option_value, "inputDistanceMapImageFilename");
    }
    inputDistanceMapImageFilename = vm["inputDistanceMapImageFilename"].as<string>();
  }

  /*-------------- End of parse -----------------------------*/
  // Get filename without extension (and without folders).
  const fs::path input_stem = fs::path(filename).stem();
  const fs::path output_file_path = fs::path(
      input_stem.string() +
      "_SKEL_" + select_string + "_" + sk_string + "_p" + std::to_string(persistence));

  using Domain = Z3i::Domain ;
  using Image = ImageContainerByITKImage<Domain, unsigned char> ;
  const unsigned int Dim = 3;
  using PixelType = unsigned char ;
  using ItkImageType = itk::Image<PixelType, Dim> ;
  /*----------------*/

  // Read Image using ITK
  using ReaderType = itk::ImageFileReader<ItkImageType> ;
  auto reader = ReaderType::New();
  reader->SetFileName(filename);
  reader->Update();

  // Invert Filter using ITK.
  using InverterType =
    itk::InvertIntensityImageFilter<ItkImageType, ItkImageType> ;
  auto inverter = InverterType::New();
  inverter->SetInput(reader->GetOutput());
  inverter->Update();
  /*----------------*/
  // Apply filters if neccesary
  Image::ITKImagePointer handle_out = (invert_image) ?
    Image::ITKImagePointer(inverter->GetOutput()) :
    Image::ITKImagePointer(reader->GetOutput());
  // Convert to DGtal Container
  Image image(handle_out);


  // Create a VoxelComplex from the set
  using DigitalTopology = DT26_6;
  using DigitalSet =
    DGtal::DigitalSetByAssociativeContainer<Domain ,
      std::unordered_set< typename Domain::Point> >;
  using Object =
    DGtal::Object<DigitalTopology, DigitalSet>;
  using Complex =
    DGtal::VoxelComplex<KSpace, Object>;

  auto & sk = sk_string;
  KSpace ks;
  // Domain of kspace must be padded.
  KSpace::Point d1( KSpace::Point::diagonal( 1 ) );
  ks.init(image.domain().lowerBound() - d1 ,
              image.domain().upperBound() + d1 , true);
  DigitalTopology::ForegroundAdjacency adjF;
  DigitalTopology::BackgroundAdjacency adjB;
  DigitalTopology topo(adjF, adjB, DGtal::DigitalTopologyProperties::JORDAN_DT);

  trace.beginBlock("construct with table");
  Complex vc(ks);
  // Optimization, Construct object in place to save memory. vc stores object.
  {
    DigitalSet image_set (image.domain());
    SetFromImage<Z3i::DigitalSet>::append<Image>(
        image_set, image,
        thresholdMin, thresholdMax);

    vc.construct( Object(topo, image_set) );
  }
  vc.setSimplicityTable(functions::loadTable(simplicity::tableSimple26_6 ));
  // vc.construct(obj.pointSet(), functions::loadTable(simplicity::tableSimple26_6 ));
  trace.endBlock();
  trace.beginBlock("load isthmus table");
  boost::dynamic_bitset<> isthmus_table;
  if (sk == "isthmus") isthmus_table = *functions::loadTable(isthmusicity::tableIsthmus);
  else if (sk == "1isthmus") isthmus_table = *functions::loadTable(isthmusicity::tableOneIsthmus);
  auto pointMap = *functions::mapZeroPointNeighborhoodToConfigurationMask<Point>();

  trace.endBlock();
  using namespace DGtal::functions ;
  // SKEL FUNCTION:
  std::function< bool(const Complex&, const Cell&) > Skel ;
  if (sk == "ulti") Skel = skelUltimate<Complex>;
  else if (sk == "end") Skel = skelEnd<Complex>;
  // else if (sk == "1is") Skel = oneIsthmus<Complex>;
  // else if (sk == "is") Skel = skelIsthmus<Complex>;
  else if (sk == "1isthmus")
      Skel = [&isthmus_table, &pointMap](const Complex & fc,
               const Complex::Cell & c){
        return skelWithTable(isthmus_table, pointMap, fc, c);
      };
  else if (sk == "isthmus")
      Skel = [&isthmus_table, &pointMap](const Complex & fc,
               const Complex::Cell & c){
        return skelWithTable(isthmus_table, pointMap, fc, c);
      };
  else throw std::runtime_error("Invalid skel string");
  auto start = std::chrono::system_clock::now();

  // SELECT FUNCTION
  /* TODO fix this(new exe?) DT has no default, no copy constructor so we have
   * to calculate for every image....
   */

  /* trace.beginBlock("Create Distance Map"); */
  /* using Predicate = Z3i::DigitalSet; */
  /* using L3Metric = ExactPredicateLpSeparableMetric<Z3i::Space, 3>; */
  /* using DT       = DistanceTransformation<Z3i::Space, Predicate, L3Metric>; */
  /* L3Metric l3; */
  /* DT dt(vc.object().domain(), vc.objectSet(), l3); */
  /* trace.endBlock(); */

  using DistanceMapPixelType = float ;
  using DistanceMapImage = ImageContainerByITKImage<Domain, DistanceMapPixelType> ;
  Z3i::Domain dummyDomain(Z3i::Point(0,0,0), Z3i::Point(1,1,1));
  DistanceMapImage distanceMapImage(dummyDomain);

  std::function< std::pair<typename Complex::Cell, typename Complex::Data>(const Complex::Clique&) > Select ;
  auto & sel = select_string;
  if (sel == "random") Select = selectRandom<Complex>;
  else if (sel == "first") Select = selectFirst<Complex>;
  else if (sel == "dmax"){
    trace.beginBlock("Import Distance Map");
    distanceMapImage = DGtal::ITKReader<DistanceMapImage>::importITK(inputDistanceMapImageFilename);
    trace.endBlock();
    Select =
      [&distanceMapImage](const Complex::Clique & clique){
        return selectMaxValue<DistanceMapImage, Complex>(distanceMapImage, clique);
      };
  } else throw std::runtime_error("Invalid skel string");

  Complex vc_new(ks);
  if (persistence == 0)
    vc_new = asymetricThinningScheme< Complex >(
        vc, Select,  Skel, verbose);
  else
    vc_new = persistenceAsymetricThinningScheme< Complex >(
        vc, Select,  Skel, persistence, verbose);

  auto end = std::chrono::system_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::seconds> (end - start) ;
  if (profile) std::cout <<"Time elapsed: " << elapsed.count() << std::endl;
  const auto & thin_set = vc_new.objectSet();

  // Export it as a simple list point
  if (vm.count("exportSDP"))
  {
    const fs::path output_folder_path{vm["exportSDP"].as<std::string>()};
    if(!fs::exists(output_folder_path)) {
      throw std::runtime_error("output folder doesn't exist : " + output_folder_path.string());
    }
    fs::path output_full_path = output_folder_path / fs::path(output_file_path.string() + ".sdp");
    std::ofstream out;
    out.open(output_full_path.string().c_str());
    for (auto &p : thin_set)
    {
      out << p[0] << " " << p[1] << " " << p[2] << std::endl;
    }
  }
  if (vm.count("exportImage"))
  {
    const fs::path output_folder_path{vm["exportImage"].as<std::string>()};
    if(!fs::exists(output_folder_path)) {
      throw std::runtime_error("output folder doesn't exist : " + output_folder_path.string());
    }
    fs::path output_full_path = output_folder_path / fs::path(output_file_path.string() + ".nrrd");
    unsigned int foreground_value = 255;
    Image thin_image(image.domain());
    ImageFromSet<Image>::append<DigitalSet>(thin_image, thin_set, foreground_value);
    bool write_status = ITKWriter<Image>::exportITK(
        output_full_path.string().c_str(),
        thin_image);
    if(!write_status) {
        std::cerr << "Failure writing file: " << output_full_path.string() << std::endl;
    }
  }

#ifdef VISUALIZE
  if (visualize)
  {
    const auto & all_set = vc.objectSet();
    int argc(1);
    char** argv(nullptr);
    QApplication app(argc, argv);
    Viewer3D<> viewer(ks);
    viewer.show();

    viewer.setFillColor(Color(255, 255, 255, 255));
    viewer << thin_set;

    // All kspace voxels
    viewer.setFillColor(Color(40, 200, 55, 10));
    viewer << all_set;

    viewer << Viewer3D<>::updateDisplay;

    app.exec();
  }
#endif
}

////////////////////////////////////////////////////////
// copiers between Object and boost::adjacency_list
////////////////////////////////////////////////////////
struct vertex_position_t {
  using kind = boost::vertex_property_tag ;
};

struct vertex_position {
  Z3i::Point myP;
  vertex_position():myP(){}
};
using VertexProperties = boost::property<
  boost::vertex_index_t, std::size_t,
  boost::property<vertex_position_t, vertex_position> > ;
