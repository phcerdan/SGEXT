/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef MASK_IMAGE_FUNCTION_HPP
#define MASK_IMAGE_FUNCTION_HPP

#include <itkMaskImageFilter.h>

namespace SG {

/**
 * Wrap itk::MaskImageFilterType for using it in python sgext
 * Used in script @sa mask_distance_map_with_thin_image
 *
 * @tparam TInputImage
 * @tparam TMaskImage
 * @param input
 * @param mask
 *
 * @return masked image
 */
template<typename TInputImage, typename TMaskImage>
typename TInputImage::Pointer
mask_image_function(
        typename TInputImage::Pointer input,
        typename TMaskImage::Pointer mask)
{
  using MaskFilterType = itk::MaskImageFilter<TInputImage, TMaskImage>;
  auto maskFilter = MaskFilterType::New();
  maskFilter->SetInput(input);
  maskFilter->SetMaskImage(mask);
  maskFilter->Update();
  return maskFilter->GetOutput();
}

} // end ns SG
#endif
