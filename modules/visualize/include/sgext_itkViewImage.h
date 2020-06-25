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

/* This is the original implementation of itk::ViewImage.
 * It was merged upstream after refactoring into a class.
 * Kept here just in case modifications are needed for
 * this project.
 */
#ifndef sgext_itkViewImage_h
#define sgext_itkViewImage_h
#include <cstddef>
#include <string>
namespace SG {
template <typename T>
void ViewImage(const T* img, const std::string& win_title = "itkView",
               size_t win_x = 600, size_t win_y = 600);

template <typename TLeft, typename TRight>
void ViewImages(const TLeft* leftImg, const TRight* rightImg,
                const std::string& win_title = "itkView", size_t win_x = 800,
                size_t win_y = 800);
}  // namespace SG

#include "sgext_itkViewImage.hxx"
#endif
