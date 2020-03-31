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
