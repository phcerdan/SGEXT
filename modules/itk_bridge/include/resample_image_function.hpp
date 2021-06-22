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

#ifndef SG_RESAMPLE_IMAGE_FUNCTION_HPP
#define SG_RESAMPLE_IMAGE_FUNCTION_HPP

#include "image_types.hpp"
#include <itkIdentityTransform.h>
#include <itkImageDuplicator.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkNearestNeighborInterpolateImageFunction.h>
#include <itkResampleImageFilter.h>

namespace SG {
/**
 * Resample binary image.
 *
 * If shrink_factor is greater than 1, it downsamples the image.
 * If shrink_factor is between (0, 1), it upsamples the image.
 *
 * Output image has the same origin, and the right spacing and size based on
 * shrink factor.
 *
 * @param input binary image
 * @param shrink_factor value greater than 0 to downsample or upsample the
 * image.
 * @param verbose extra info to std::cout
 *
 * @return resampled image
 */
BinaryImageType::Pointer
resample_image_function(const BinaryImageType::Pointer &input,
                         const double &shrink_factor,
                         bool verbose);

enum class Interpolator {
    wise,
    linear,
    nearest_neighbor
};

template<typename TImage>
typename TImage::Pointer make_isotropic(
        const typename TImage::Pointer &input,
        const Interpolator & interpolator_method = Interpolator::wise
        ) {
    const auto & original_spacing = input->GetSpacing();
    const bool is_already_isotropic =
        std::all_of(original_spacing.Begin(), original_spacing.End(),
            [&original_spacing](const auto &a) {
            return itk::Math::FloatAlmostEqual(a, original_spacing[0]);
            });
    if (is_already_isotropic) {
        // Return a copy of input, for consistency
        using DuplicatorType = itk::ImageDuplicator<TImage>;
        auto duplicator = DuplicatorType::New();
        duplicator->SetInputImage(input);
        duplicator->Update();
        return duplicator->GetOutput();
    }

    const auto original_size = input->GetLargestPossibleRegion().GetSize();
    auto min_spacing = *std::min_element(original_spacing.Begin(), original_spacing.End());
    auto new_spacing = decltype(original_spacing)(min_spacing);
    auto new_size = decltype(original_size)();
    for(size_t dim = 0; dim < TImage::ImageDimension; ++dim) {
        new_size[dim] = original_size[dim] * original_spacing[dim] / min_spacing;
    }

    using TransformType =
            itk::IdentityTransform<double, TImage::ImageDimension>;
    auto transform = TransformType::New();
    transform->SetIdentity();

    using ResampleFilter = itk::ResampleImageFilter<TImage, TImage>;
    auto resampler = ResampleFilter::New();
    resampler->SetInput(input);
    resampler->SetTransform(transform);
    resampler->SetDefaultPixelValue(0);
    resampler->SetOutputOrigin(input->GetOrigin());
    resampler->SetOutputDirection(input->GetDirection());
    resampler->SetOutputSpacing(new_spacing);
    resampler->SetSize(new_size);

    // Handle interpolator, if wise (default, automatic), choose nearest_neighbor
    // for binary images and linear for anything else.
    using InterpolatorTypeLinear =
            itk::LinearInterpolateImageFunction<TImage, double>;
    using InterpolatorTypeNearest =
            itk::NearestNeighborInterpolateImageFunction<TImage, double>;

    if(interpolator_method == Interpolator::wise) {
        if constexpr(std::is_same<TImage, BinaryImageType>()) {
            resampler->SetInterpolator(InterpolatorTypeNearest::New());
        } else {
            resampler->SetInterpolator(InterpolatorTypeLinear::New());
        }
    } else {
        if(interpolator_method == Interpolator::nearest_neighbor) {
            resampler->SetInterpolator(InterpolatorTypeNearest::New());
        } else {
            resampler->SetInterpolator(InterpolatorTypeLinear::New());
        }
    }

    resampler->Update();

    return resampler->GetOutput();
}
} // namespace SG

#endif
