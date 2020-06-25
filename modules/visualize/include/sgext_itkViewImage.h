/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
