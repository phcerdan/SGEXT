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

#include "sgext_fixture_images.hpp"
#include "view_image_function.hpp"
#include "scripts_types.hpp"

#include "gmock/gmock.h"

#include <itkImageFileReader.h>

TEST(read_a_fixture_image, read_a_fixture_image) {
    using ReaderType = itk::ImageFileReader<SG::FloatImageType>;
    auto reader = ReaderType::New();
    reader->SetFileName(SG::sgext_fixture_images_path + "/bX3D_white.nrrd");
    reader->Update();
}
