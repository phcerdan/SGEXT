/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VIEW_IMAGE_FUNCTION_HPP
#define VIEW_IMAGE_FUNCTION_HPP

#include <string>
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkViewImage.h"

namespace SG {
    namespace defaults {
        const std::string view_image_win_title = "SGEXT itkViewImage";
        const size_t view_image_win_width = 600;
        const size_t view_image_win_height = 600;
    }

template<typename ImageType>
void view_image(const std::string & inputFile,
        const std::string & winTitle = defaults::view_image_win_title,
        const size_t & winWidth = defaults::view_image_win_width,
        const size_t & winHeight = defaults::view_image_win_width)
{
  using ReaderType = itk::ImageFileReader<ImageType>;
  auto reader = ReaderType::New();
  reader->SetFileName(inputFile);
  reader->Update();

  itk::ViewImage<ImageType>::View(reader->GetOutput(), winTitle, winWidth, winHeight);
}

template<typename ImageType>
void view_image(const ImageType* img_ptr,
        const std::string & winTitle = defaults::view_image_win_title,
        const size_t & winWidth = defaults::view_image_win_width,
        const size_t & winHeight = defaults::view_image_win_height)
{
  itk::ViewImage<ImageType>::View(img_ptr, winTitle, winWidth, winHeight);
}

template<typename ImageType>
void view_image(const typename ImageType::Pointer & img_smart_ptr,
        const std::string & winTitle = defaults::view_image_win_title,
        const size_t & winWidth = defaults::view_image_win_width,
        const size_t & winHeight = defaults::view_image_win_height)
{
    return view_image(img_smart_ptr.GetPointer(),
            winTitle, winWidth, winHeight);
}

} // end ns
#endif
