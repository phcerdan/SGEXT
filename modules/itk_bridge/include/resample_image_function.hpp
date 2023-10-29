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
#include <itkBSplineInterpolateImageFunction.h>
#include <itkIdentityTransform.h>
#include <itkImageDuplicator.h>
#include <itkImageSpatialObject.h>
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

enum class Interpolator { wise, linear, bspline, nearest_neighbor };

template <typename TImage>
typename TImage::Pointer
make_isotropic(const typename TImage::Pointer &input,
               const bool standardize_axis = true,
               const Interpolator &interpolator_method = Interpolator::wise) {
    const auto &original_origin = input->GetOrigin();
    const auto &original_spacing = input->GetSpacing();
    const auto &original_direction_matrix = input->GetDirection();
    const bool is_already_isotropic = std::all_of(
            original_spacing.Begin(), original_spacing.End(),
            [&original_spacing](const auto &a) {
                return itk::Math::FloatAlmostEqual(a, original_spacing[0]);
            });
    // Ravel direction matrix to vector
    auto identity_matrix = typename TImage::DirectionType();
    identity_matrix.SetIdentity();
    const bool is_already_standardized =
            original_direction_matrix == identity_matrix;
    const bool need_standardize_axis =
            standardize_axis && !is_already_standardized;
    if (is_already_isotropic and !need_standardize_axis) {
        // Return a copy of input, for consistency
        using DuplicatorType = itk::ImageDuplicator<TImage>;
        auto duplicator = DuplicatorType::New();
        duplicator->SetInputImage(input);
        duplicator->Update();
        return duplicator->GetOutput();
    }

    const auto original_size = input->GetLargestPossibleRegion().GetSize();
    auto min_spacing =
            *std::min_element(original_spacing.Begin(), original_spacing.End());
    auto new_spacing = decltype(original_spacing)(min_spacing);
    auto new_size = decltype(original_size)();
    for (size_t dim = 0; dim < TImage::ImageDimension; ++dim) {
        new_size[dim] =
                original_size[dim] * original_spacing[dim] / min_spacing;
    }

    auto new_origin = typename TImage::PointType();
    new_origin = original_origin;

    auto new_direction_matrix = typename TImage::DirectionType();
    new_direction_matrix = original_direction_matrix;

    if (need_standardize_axis) {
        new_direction_matrix.SetIdentity();

        auto corners_voxel_space = std::array<typename TImage::IndexType, 8>{};
        // Fill with zeros all of them
        for (auto &vcorner : corners_voxel_space) {
            vcorner.Fill(0);
        }
        corners_voxel_space[1][0] = original_size[0];

        corners_voxel_space[2][1] = original_size[1];

        corners_voxel_space[3][0] = original_size[0];
        corners_voxel_space[3][1] = original_size[1];

        corners_voxel_space[4][2] = original_size[2];

        corners_voxel_space[5][0] = original_size[0];
        corners_voxel_space[5][2] = original_size[2];

        corners_voxel_space[6][1] = original_size[1];
        corners_voxel_space[6][2] = original_size[2];

        corners_voxel_space[7][0] = original_size[0];
        corners_voxel_space[7][1] = original_size[1];
        corners_voxel_space[7][2] = original_size[2];

        auto corners_physical_space =
                std::array<typename TImage::PointType, 8>{};
        for (size_t i = 0; i < corners_voxel_space.size(); ++i) {
            const auto &vcorner = corners_voxel_space[i];
            auto &wcorner = corners_physical_space[i];
            input->TransformIndexToPhysicalPoint(vcorner, wcorner);
        }

        // Compute min and max of each axis
        std::array<typename TImage::PointType::ValueType, 3> mins{
                std::numeric_limits<
                        typename TImage::PointType::ValueType>::max()};
        std::array<typename TImage::PointType::ValueType, 3> maxs{
                std::numeric_limits<
                        typename TImage::PointType::ValueType>::lowest()};
        for (const auto &wcorner : corners_physical_space) {
            for (size_t dim = 0; dim < TImage::ImageDimension; ++dim) {
                mins[dim] = std::min(mins[dim], wcorner[dim]);
                maxs[dim] = std::max(maxs[dim], wcorner[dim]);
            }
        }
        // Compute new size with mins and maxs;
        for (size_t dim = 0; dim < TImage::ImageDimension; ++dim) {
            // new spacing is already computed as isotropic
            new_size[dim] =
                    std::ceil((maxs[dim] - mins[dim]) / new_spacing[dim]);
            new_origin[dim] = mins[dim];
        }
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
    resampler->SetOutputOrigin(new_origin);
    resampler->SetOutputDirection(new_direction_matrix);
    resampler->SetOutputSpacing(new_spacing);
    resampler->SetSize(new_size);

    // Handle interpolator, if wise (default, automatic), choose
    // nearest_neighbor for binary images and linear for anything else.
    using InterpolatorTypeLinear =
            itk::LinearInterpolateImageFunction<TImage, double>;
    using InterpolatorTypeNearest =
            itk::NearestNeighborInterpolateImageFunction<TImage, double>;
    using InterpolatorTypeBSpline =
            itk::BSplineInterpolateImageFunction<TImage, double>;

    if (interpolator_method == Interpolator::wise) {
        if constexpr (std::is_same<TImage, BinaryImageType>()) {
            resampler->SetInterpolator(InterpolatorTypeNearest::New());
        } else {
            resampler->SetInterpolator(InterpolatorTypeBSpline::New());
        }
    } else {
        if (interpolator_method == Interpolator::nearest_neighbor) {
            resampler->SetInterpolator(InterpolatorTypeNearest::New());
        } else if (interpolator_method == Interpolator::linear) {
            resampler->SetInterpolator(InterpolatorTypeLinear::New());
        } else if (interpolator_method == Interpolator::bspline) {
            resampler->SetInterpolator(InterpolatorTypeBSpline::New());
        }
    }

    resampler->Update();

    return resampler->GetOutput();
}
} // namespace SG

#endif
