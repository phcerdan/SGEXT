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
#include "gmock/gmock.h"

#include "sgext_fixture_images.hpp"
#include "visualize_with_label_image.hpp"

#include <itkBinaryThresholdImageFilter.h>
#include <itkImageFileReader.h>
#include <itkViewImage.h> // TODO REMOVE

// TODO Workaround vtk_module_autoinit not working for test files.
#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2)

TEST(visualize_with_label_image, works) {

    using FloatImageType = itk::Image<float, 3>;
    using BinaryImageType = itk::Image<unsigned char, 3>;

    using ReaderType = itk::ImageFileReader<BinaryImageType>;
    auto reader_input = ReaderType::New();
    reader_input->SetFileName(SG::sgext_fixture_images_path +
                              "/bX3D_white.nrrd");
    reader_input->Update();
    auto input_image = reader_input->GetOutput();

    using ReaderFloatType = itk::ImageFileReader<FloatImageType>;
    auto reader_dmap = ReaderFloatType::New();
    reader_dmap->SetFileName(SG::sgext_fixture_images_path +
                             "/bX3D_white_DMAP.nrrd");

    using BinaryThresholdImageFilter =
            itk::BinaryThresholdImageFilter<FloatImageType, BinaryImageType>;
    auto thresholder = BinaryThresholdImageFilter::New();
    thresholder->SetInput(reader_dmap->GetOutput());
    thresholder->SetLowerThreshold(2.0);
    thresholder->Update();

    auto label_image = thresholder->GetOutput();

    const double label_opacity = 0.8;

    // input is binary
    SG::view_image_with_label(input_image, label_image,
                              label_opacity);
}

