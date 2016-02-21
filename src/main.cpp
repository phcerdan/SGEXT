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

// #include "phcCCfunctions.h"
using namespace DGtal;
using namespace std;
using namespace DGtal::Z2i;
int main(int argc, char *argv[])
{
  cout << "Reading ITK Image" << endl;
  typedef Domain Domain;
  typedef ImageContainerByITKImage<Domain, unsigned char> Image;
  string filename = "./bX2D.png";
  Image imageRead = ITKReader<Image>::importITK(filename);
  // Invert using ITK.
  const unsigned int Dim = 2;
  typedef unsigned char PixelType;
  typedef itk::Image<PixelType, Dim> itkImageType;
  typedef itk::InvertIntensityImageFilter<itkImageType, itkImageType> InverterType;
  auto inverter = InverterType::New();
  inverter->SetInput(imageRead.getITKImagePointer());
  inverter->Update();
  Image::ITKImagePointer handle_out = inverter->GetOutput();
  Image image(handle_out);

  DigitalSet set2d (image.domain());
  SetFromImage<Z2i::DigitalSet>::append<Image>(set2d, image, 1, 255);

  KSpace K;
  K.init(image.domain().lowerBound(), image.domain().upperBound(), true);

  SurfelAdjacency<2> sAdj(true);

  Board2D board;
  board.setUnit(Board2D::UCentimeter);
  board << image.domain();
  /* << set2d; */
  /* board.saveSVG("./img.svg"); */

  // Cubical Complex
  typedef KSpace::Point                             Point;
  typedef KSpace::Cell                              Cell;
  /* typedef CubicalComplex< KSpace >             CC; */
  typedef CubicalComplex<KSpace> CC;
  typedef CC::CellMapConstIterator                  CellMapConstIterator;
  CC X( K );
  X.construct< DigitalSet >( set2d );

  CC Y( K );
  {
    typedef CC::CellMapConstIterator CellMapConstIterator;
    typedef CC::CellMapIterator CellMapIterator;
    for(unsigned int d = 0 ; d <=2 ; ++d)
      for ( CellMapIterator it = X.begin( d ), itE = X.end( d );
          it != itE; ++it )
      {
        board << it->first;
      //   CellMapIterator it_up;
      //   CC::CellType type = X.computeCellType(it->first, it_up, 2);
      //   bool maximal = (type == CC::CellType::Free) ;
      //   if ( maximal )
      //   {
      //     cout << "maximal" << endl;
      //     it->second.data = CC::FIXED;
      //     Y.insertCell(it->first, it->second);
      //   }
      // }
      }
  }
  board.saveSVG("./cc.svg");
  CC::DefaultCellMapIteratorPriority P;
  uint64_t removed
    = functions::collapse( X, X.begin(), X.end(), P, true, true, true );

  board.clear();
  typedef CC::CellMapConstIterator CellMapConstIterator;
  for(unsigned int d = 0 ; d <=2 ; ++d)
    for ( CellMapConstIterator it = X.begin( d ), itE = X.end( d );
        it != itE; ++it )
      board << it->first;

  board.saveSVG("./collapse.svg");
}


