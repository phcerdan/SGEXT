/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
