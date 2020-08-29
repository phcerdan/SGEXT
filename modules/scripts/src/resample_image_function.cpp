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

#include "resample_image_function.hpp"
#include <itkIdentityTransform.h>
#include <itkNearestNeighborInterpolateImageFunction.h>
#include <itkResampleImageFilter.h>

namespace SG {
BinaryImageType::Pointer
resample_image_function(const BinaryImageType::Pointer &input,
                         const double &shrink_factor,
                         bool verbose) {
    const auto spacing = input->GetSpacing();
    const auto size = input->GetLargestPossibleRegion().GetSize();
    auto output_spacing = spacing;
    auto output_size = size;
    for (size_t i = 0; i < BinaryImageType::ImageDimension; ++i) {
        output_spacing[i] *= shrink_factor;
        output_size[i] = static_cast<size_t>(size[i] / shrink_factor);
    }
    if (verbose) {
        std::cout << "Resampler: " << std::endl;
        std::cout << " Original Spacing: " << spacing << std::endl;
        std::cout << " Original Size: " << size << std::endl;
        std::cout << " Resampled Spacing: " << output_spacing << std::endl;
        std::cout << " Resampled Size: " << output_size << std::endl;
    }

    using TransformType =
            itk::IdentityTransform<double, BinaryImageType::ImageDimension>;
    auto transform = TransformType::New();
    transform->SetIdentity();
    using InterpolatorType =
            itk::NearestNeighborInterpolateImageFunction<BinaryImageType,
                                                         double>;
    auto interpolator = InterpolatorType::New();

    using ResampleFilter =
            itk::ResampleImageFilter<BinaryImageType, BinaryImageType>;
    auto resampler = ResampleFilter::New();
    resampler->SetInput(input);
    resampler->SetTransform(transform);
    resampler->SetInterpolator(interpolator);
    resampler->SetDefaultPixelValue(0);
    resampler->SetOutputOrigin(input->GetOrigin());
    resampler->SetOutputSpacing(output_spacing);
    resampler->SetOutputDirection(input->GetDirection());
    resampler->SetSize(output_size);
    resampler->Update();

    return resampler->GetOutput();
}
} // namespace SG
