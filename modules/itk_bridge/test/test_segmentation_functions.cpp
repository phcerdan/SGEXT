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


#include "segmentation_functions.hpp"

#include <itkImageFileReader.h>

#include "sgext_fixture_images.hpp"
#include "gmock/gmock.h"

TEST(binarize_with_percentage, works_for_all_image_types) {
    const std::string image_name_stem("bX3D_white");
    const std::string filename =
            SG::sgext_fixture_images_path + "/" + image_name_stem + ".nrrd";
    using ReaderType = itk::ImageFileReader<SG::BinaryImageType>;
    auto reader = ReaderType::New();
    reader->SetFileName(filename);
    reader->Update();
    auto input_image = reader->GetOutput();
    const double percentage = 0.2;
    auto binarized_image =
        SG::binarize_with_percentage(input_image, percentage);

    // The image is already binary, so the filter doesn't change anything.
    // Pick an ON pixel.
    using IndexType = SG::BinaryImageType::IndexType;
    IndexType index;
    index[0] = 25;
    index[1] = 25;
    index[2] = 4;
    EXPECT_EQ(binarized_image->GetPixel(index), 255);

    // Float
    using FloatReaderType = itk::ImageFileReader<SG::FloatImageType>;
    auto reader_float = FloatReaderType::New();
    reader_float->SetFileName(filename);
    reader_float->Update();
    SG::FloatImageType::Pointer input_image_float = reader_float->GetOutput();
    auto binarized_image_float =
        SG::binarize_with_percentage<SG::FloatImageType>(input_image_float, percentage);

    EXPECT_EQ(binarized_image_float->GetPixel(index), 255);

    // Any other type
    using AnyImageType = itk::Image<short, 3>;
    using AnyReaderType = itk::ImageFileReader<AnyImageType>;
    auto reader_any = AnyReaderType::New();
    reader_any->SetFileName(filename);
    reader_any->Update();
    AnyImageType::Pointer input_image_any = reader_any->GetOutput();
    auto binarized_image_any =
        SG::binarize_with_percentage(input_image_any.GetPointer(), percentage);
    EXPECT_EQ(binarized_image_any->GetPixel(index), 255);
}
