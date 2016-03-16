#include <iostream>
#include <chrono>
#include <DGtal/base/Common.h>
#include <DGtal/helpers/StdDefs.h>
#include <DGtal/io/readers/ITKReader.h>
#include <DGtal/images/ImageContainerByITKImage.h>
#include <DGtal/images/imagesSetsUtils/SetFromImage.h>

#include <DGtal/topology/SurfelAdjacency.h>
#include <DGtal/io/boards/Board2D.h>
#include <DGtal/topology/CubicalComplex.h>
#include <DGtal/topology/CubicalComplexFunctions.h>
#include <unordered_map>

#include <DGtal/topology/VoxelComplex.h>
#include <DGtal/topology/VoxelComplexFunctions.h>
// Invert
#include "itkInvertIntensityImageFilter.h"
#include <DGtal/io/viewers/Viewer3D.h>
#include <itkConstantPadImageFilter.h>
#include <itkNumericTraits.h>
// boost::program_options
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
using namespace DGtal;
using namespace std;
using namespace DGtal::Z3i;
namespace po = boost::program_options;

int main(int argc, char* const argv[]){

  /*-------------- Parse command line -----------------------------*/
  po::options_description general_opt ( "Allowed options are: " );
  general_opt.add_options()
    ( "help,h", "display this message." )
    ( "input,i", po::value<string>()->required(), "Input vol file." )
    ( "skel,s",  po::value<string>()->required(), "type of skeletonization" )
    ( "foreground,f",  po::value<string>()->default_value("black"), "foreground color in binary image" )
    ( "thresholdMin,m",  po::value<int>()->default_value(0), "threshold min (excluded) to define binary shape" )
    ( "thresholdMax,M",  po::value<int>()->default_value(255), "threshold max (included) to define binary shape" )
    ( "persistence,p",  po::value<int>()->default_value(0), "persistence value, implies use of persistence algorithm if p>=1" )
    ( "profile",  po::bool_switch()->default_value(false), "profile algorithm" )
    ( "verbose,v",  po::bool_switch()->default_value(false), "verbose output" );
  bool parseOK=true;
  po::variables_map vm;

  try {
    po::store(po::parse_command_line(argc, argv, general_opt), vm);
  } catch(const exception& ex) {
    parseOK=false;
    trace.info()<< "Error checking program options: "<< ex.what()<< endl;
  }
  po::notify ( vm );
  if (!parseOK || vm.count ( "help" ) || argc<=1 )
  {
    trace.info() <<
    "Compute the thinning of a volume using an AsymetricThinningScheme"<< endl
    << endl << "Basic usage: "<< endl
    << "asymThin -i <volFileName> -s <ulti,end,1is,is>"
    " [ -f <white,black> -m <minlevel> -M <maxlevel> -v ] "
    " [-p <value>" << endl
    << "options for skel_string = ulti, end, 1is, is" << endl
    << general_opt << "\n";
    return 0;
  }
  //Parse options
  string filename = vm["input"].as<string>();
  bool verbose = vm["verbose"].as<bool>();
  bool profile = vm["profile"].as<bool>();
  int thresholdMin = vm["thresholdMin"].as<int>();
  int thresholdMax = vm["thresholdMax"].as<int>();
  int persistence = vm["persistence"].as<int>();
  if (vm.count("persistence") && persistence < 0 )
    throw po::validation_error(po::validation_error::invalid_option_value, "persitence");
  string foreground = vm["foreground"].as<string>();
  if (vm.count("foreground") && (!(foreground == "white" || foreground == "black")))
    throw po::validation_error(po::validation_error::invalid_option_value, "foreground");
  bool invert_image = (foreground == "black") ? true : false ;

  string sk_string = vm["skel"].as<string>();
  if (vm.count("skel") &&
     (!( sk_string == "ulti" || sk_string == "end" ||
         sk_string == "1is" || sk_string == "is")))
     throw po::validation_error(po::validation_error::invalid_option_value, "skel");
  /*-------------- End of parse -----------------------------*/

  using Domain = Domain ;
  using Image = ImageContainerByITKImage<Domain, unsigned char> ;

  Image imageReader = ITKReader<Image>::importITK(filename);
  const unsigned int Dim = 3;
  using PixelType = unsigned char ;
  using ItkImageType = itk::Image<PixelType, Dim> ;
  // Pad image:
  using PadType = itk::ConstantPadImageFilter<ItkImageType, ItkImageType> ;
  auto padder = PadType::New();
  ItkImageType::PixelType constantValue;
  constantValue =(invert_image == true) ?
    itk::NumericTraits<PixelType>::max() : itk::NumericTraits<PixelType>::min();
  ItkImageType::SizeType padRegion;
  for( unsigned int i = 0 ; i<Dim ; ++i)
    padRegion[i] = 1;
  padder->SetInput(imageReader.getITKImagePointer());
  padder->SetPadLowerBound(padRegion);
  padder->SetPadUpperBound(padRegion);
  padder->SetConstant(constantValue);
  padder->Update();
  /*----------------*/

  // Invert Filter using ITK.
  using InverterType =
    itk::InvertIntensityImageFilter<ItkImageType, ItkImageType> ;
  auto inverter = InverterType::New();
  inverter->SetInput(padder->GetOutput());
  inverter->Update();
  /*----------------*/
  // Apply filters if neccesary
  Image::ITKImagePointer handle_out = (invert_image) ?
    Image::ITKImagePointer(inverter->GetOutput()) :
    Image::ITKImagePointer(padder->GetOutput());
  Image image(handle_out);

  DigitalSet image_set (image.domain());
  SetFromImage<Z3i::DigitalSet>::append<Image>(
      image_set, image,
      thresholdMin, thresholdMax);


  // Create a VoxelComplex from the set

  using DigitalTopology = DT26_6;
  using DigitalSet =
    DGtal::DigitalSetByAssociativeContainer<Domain ,
      std::unordered_set< typename Domain::Point> >;
  using Object =
    DGtal::Object<DigitalTopology, DigitalSet>;
  using Complex =
    DGtal::VoxelComplex<KSpace, Object>;

  KSpace ks;
  ks.init(image.domain().lowerBound() ,
      image.domain().upperBound() , true);

  DigitalTopology::ForegroundAdjacency adjF;
  DigitalTopology::BackgroundAdjacency adjB;
  DigitalTopology topo(adjF, adjB, DGtal::DigitalTopologyProperties::JORDAN_DT);
  Object obj(topo,image_set);

  Complex vc(ks);
  vc.construct(obj);

  using namespace DGtal::functions ;

  std::function< bool(const Complex&, const Cell&) > Skel ;
  auto & sk = sk_string;
  if (sk == "ulti") Skel = skelUltimate<Complex>;
  else if (sk == "end") Skel = skelEnd<Complex>;
  else if (sk == "1is") Skel = oneIshtmus<Complex>;
  else if (sk == "is") Skel = skelIshtmus<Complex>;
  else throw std::runtime_error("Invalid skel string");
  auto start = std::chrono::system_clock::now();
  Complex vc_new(ks);
  if (persistence == 0)
    vc_new = asymetricThinningScheme< Complex >(
        vc, selectRandom<Complex>,  Skel, verbose);
  else
    vc_new = persistenceAsymetricThinningScheme< Complex >(
        vc, selectRandom<Complex>,  Skel, persistence, verbose);

  auto end = std::chrono::system_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::seconds> (end - start) ;
  if (profile) std::cout <<"Time elapsed: " << elapsed.count() << std::endl;
  // THEN( "visualize the cells" )
  {
    int argc(1);
    char** argv(nullptr);
    QApplication app(argc, argv);
    Viewer3D<> viewer(ks);
    viewer.show();

    viewer.setFillColor(Color(255, 255, 255, 255));
    for ( auto it = vc_new.begin(3); it!= vc_new.end(3); ++it )
      viewer << it->first;
    // viewer << image_set;

    // All kspace voxels
    viewer.setFillColor(Color(40, 200, 55, 10));
    for ( auto it = vc.begin(3); it!= vc.end(3); ++it )
      viewer << it->first;

    viewer << Viewer3D<>::updateDisplay;
    app.exec();
  }
}
