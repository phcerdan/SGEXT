#include "catch_header.h"
#include <iostream>
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

using namespace DGtal;
using namespace std;
using namespace DGtal::Z3i;
SCENARIO( "Read an image using ITK", "[thin]" ){
  cout << "Reading ITK Image" << endl;
  using Domain = Domain ;
  using Image = ImageContainerByITKImage<Domain, unsigned char> ;
  string filename = "./bX3D.tif";
  Image imageRead = ITKReader<Image>::importITK(filename);
  // Invert using ITK.
  const unsigned int Dim = 3;
  using PixelType = unsigned char ;
  using itkImageType = itk::Image<PixelType, Dim> ;
  using InverterType = itk::InvertIntensityImageFilter<itkImageType, itkImageType> ;
  auto inverter = InverterType::New();
  inverter->SetInput(imageRead.getITKImagePointer());
  inverter->Update();
  Image::ITKImagePointer handle_out = inverter->GetOutput();
  Image image(handle_out);

  DigitalSet image_set (image.domain());
  SetFromImage<Z3i::DigitalSet>::append<Image>(image_set, image, 1, 255);


  // Create a VoxelComplex from the set

  using DigitalTopology = DT26_6;
  using DigitalSet =
    DGtal::DigitalSetByAssociativeContainer<Domain , std::unordered_set< typename Domain::Point> >;
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

  // std::function< bool(const Complex&, const Cell&) > Skel =
  //   [](const Complex & vlambda, const Cell & cell){
  //     return !vlambda.isSimple(cell);
  //   };

  std::function< bool(const Complex&, const Cell&) > Skel =
    [](const Complex & vlambda, const Cell & cell){
      // return !vlambda.isSimple(cell);
      auto &ks = vlambda.space();
      auto pnsize = vlambda.voxels().properNeighborhoodSize(ks.uCoords(cell));
      return (pnsize == 1);
    };
  using namespace DGtal::functions ;
  auto vc_new = asymetricThinningScheme< Complex >(vc, selectFirst<Complex>, Skel);
  THEN( "visualize the cells" ){

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
// WHEN( "collapse" ){
//   VC::DefaultCellMapIteratorPriority P;
//   // auto removed = functions::collapse(X, X.begin(), X.end(), P, true, true, true);
//   // auto removed = functions::collapse(X, X.begin(), X.end(), P, false, false, true);
// }
