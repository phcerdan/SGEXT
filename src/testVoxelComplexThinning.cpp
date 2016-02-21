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

// Invert
#include "itkInvertIntensityImageFilter.h"
#include <DGtal/io/viewers/Viewer3D.h>

using namespace DGtal;
using namespace std;
using namespace DGtal::Z3i;
SCENARIO( "Read an image using ITK", "[thin]" ){
  cout << "Reading ITK Image" << endl;
  typedef Domain Domain;
  typedef ImageContainerByITKImage<Domain, unsigned char> Image;
  string filename = "./bX3D.tif";
  Image imageRead = ITKReader<Image>::importITK(filename);
  // Invert using ITK.
  const unsigned int Dim = 3;
  typedef unsigned char PixelType;
  typedef itk::Image<PixelType, Dim> itkImageType;
  typedef itk::InvertIntensityImageFilter<itkImageType, itkImageType> InverterType;
  auto inverter = InverterType::New();
  inverter->SetInput(imageRead.getITKImagePointer());
  inverter->Update();
  Image::ITKImagePointer handle_out = inverter->GetOutput();
  Image image(handle_out);

  DigitalSet setImage (image.domain());
  SetFromImage<Z3i::DigitalSet>::append<Image>(setImage, image, 1, 255);

  KSpace ks;
  ks.init(image.domain().lowerBound() - Point{1,1,1}, image.domain().upperBound() + Point{1,1,1}, true);

  THEN( "visualize the cells" ){

    int argc(1);
    char** argv(nullptr);
    QApplication app(argc, argv);
    Viewer3D<> viewer(ks);
    viewer.show();

    viewer.setFillColor(Color(255, 255, 255, 255));
    viewer << setImage;

    // All kspace voxels
    // viewer.setFillColor(Color(40, 200, 55, 10));
    // for ( auto it = vc.begin(3); it!= vc.end(3); ++it )
    //   viewer << it->first;

    viewer << Viewer3D<>::updateDisplay;
    app.exec();
  }

  GIVEN( "A voxel complex" ){
    THEN( "description" ){
      CHECK(1==1);
    }
  }
}
// WHEN( "collapse" ){
//   VC::DefaultCellMapIteratorPriority P;
//   // auto removed = functions::collapse(X, X.begin(), X.end(), P, true, true, true);
//   // auto removed = functions::collapse(X, X.begin(), X.end(), P, false, false, true);
// }
