/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
