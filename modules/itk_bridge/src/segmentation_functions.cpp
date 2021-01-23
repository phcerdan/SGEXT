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

#include "segmentation_functions.hpp"

#include <itkBinaryThresholdImageFilter.h>
#include <itkConnectedComponentImageFilter.h>
#include <itkGeodesicActiveContourLevelSetImageFilter.h>
#include <itkGradientMagnitudeRecursiveGaussianImageFilter.h>
#include <itkOtsuMultipleThresholdsImageFilter.h>
#include <itkRelabelComponentImageFilter.h>
#include <itkSigmoidImageFilter.h>
#include <itkSignedMaurerDistanceMapImageFilter.h>

namespace SG {

BinaryImageType::Pointer
binarize_with_level_set(
        const FloatImageType * input_float_image,
        const BinaryImageType * binary_image_safe) {

    using GradientFilterType =
        itk::GradientMagnitudeRecursiveGaussianImageFilter< FloatImageType, FloatImageType >;
    GradientFilterType::Pointer gradient_filter = GradientFilterType::New();
    gradient_filter->SetInput(input_float_image);
    const double gradient_sigma = 0.5 * input_float_image->GetSpacing()[0];
    gradient_filter->SetSigma(gradient_sigma);
    gradient_filter->Update();

    using GradientThresholdFilterType = itk::OtsuMultipleThresholdsImageFilter<FloatImageType, BinaryImageType>;
    GradientThresholdFilterType::Pointer gradient_threshold_filter = GradientThresholdFilterType::New();
    gradient_threshold_filter->SetInput(gradient_filter->GetOutput());
    gradient_threshold_filter->SetNumberOfThresholds(2);
    gradient_threshold_filter->Update();

    using SigmoidFilterType = itk::SigmoidImageFilter< FloatImageType, FloatImageType >;
    SigmoidFilterType::Pointer sigmoid_filter = SigmoidFilterType::New();
    sigmoid_filter->SetOutputMinimum(0.0);
    sigmoid_filter->SetOutputMaximum(1.0);
    sigmoid_filter->SetAlpha(-0.5);
    sigmoid_filter->SetBeta(gradient_threshold_filter->GetThresholds()[0]);
    sigmoid_filter->SetInput(gradient_filter->GetOutput());

    // Use input safe binarization as initial guess

    using DistFiltType = itk::SignedMaurerDistanceMapImageFilter<BinaryImageType, FloatImageType>;
    DistFiltType::Pointer distFilt = DistFiltType::New();
    distFilt->SetInput(binary_image_safe);
    distFilt->Update();

    // Level-set evolution
    using LevelSetFilterType =
        itk::GeodesicActiveContourLevelSetImageFilter< FloatImageType, FloatImageType >;
    LevelSetFilterType::Pointer level_set_filter = LevelSetFilterType::New();
    level_set_filter->SetPropagationScaling(1.0);
    level_set_filter->SetCurvatureScaling(1.0);
    level_set_filter->SetAdvectionScaling(1.0);
    level_set_filter->SetMaximumRMSError(0.02);
    level_set_filter->SetNumberOfIterations(20);
    level_set_filter->SetInput(distFilt->GetOutput());
    level_set_filter->SetFeatureImage(sigmoid_filter->GetOutput());

    using LevelSetThresholdFilterType = itk::BinaryThresholdImageFilter<FloatImageType, BinaryImageType>;
    LevelSetThresholdFilterType::Pointer level_set_threshold_filter = LevelSetThresholdFilterType::New();
    level_set_threshold_filter->SetLowerThreshold(-1*itk::NumericTraits<FloatImageType::PixelType>::max());
    level_set_threshold_filter->SetUpperThreshold(0.0);
    level_set_threshold_filter->SetOutsideValue(0);
    level_set_threshold_filter->SetInsideValue(255);
    level_set_threshold_filter->SetInput(level_set_filter->GetOutput());
    level_set_threshold_filter->Update();

    return level_set_threshold_filter->GetOutput();
}

ConnectedComponentsOutput
connected_components(const BinaryImageType * input_binary_image) {
    using ConnectedFilterType = itk::ConnectedComponentImageFilter< BinaryImageType, BinaryImageType >;
    auto connected_components_filter = ConnectedFilterType::New();
    connected_components_filter->SetInput(input_binary_image);
    connected_components_filter->Update();

    // RelabelComponentImageFilter order the components in size.
    // Label 0 is background
    // Label 1 will have the largest object, etc.
    using RelabelComponentFilterType = itk::RelabelComponentImageFilter< BinaryImageType, BinaryImageType >;
    auto relabel_filter = RelabelComponentFilterType::New();
    relabel_filter->SetInput( connected_components_filter->GetOutput() );
    relabel_filter->Update();

    auto output = ConnectedComponentsOutput();
    output.label_image = relabel_filter->GetOutput();
    output.number_of_labels = connected_components_filter->GetObjectCount();
    output.size_of_labels = relabel_filter->GetSizeOfObjectsInPixels();
    return output;
}

BinaryImageType::Pointer
extract_a_label(const BinaryImageType * input_label_image,
                const size_t &label) {
    if (label > 255) {
        throw std::runtime_error("label cannot be greater than 255");
    }
    using LabelThresholdFilterType =
        itk::BinaryThresholdImageFilter< BinaryImageType, BinaryImageType>;
    auto label_threshold = LabelThresholdFilterType::New();
    label_threshold->SetInput(input_label_image);
    label_threshold->SetLowerThreshold( label );
    label_threshold->SetUpperThreshold( label );
    label_threshold->SetOutsideValue( 0 );
    // Set the ON pixel at 255 for consistency with sgext.
    label_threshold->SetInsideValue( 255 );
    label_threshold->Update();
    return label_threshold->GetOutput();
}

// min_max_values instantiation for BinaryImageType and FloatImageType
template std::pair<BinaryImageType::PixelType, BinaryImageType::PixelType>
min_max_values<BinaryImageType>(const BinaryImageType*);

template std::pair<FloatImageType::PixelType, FloatImageType::PixelType>
min_max_values<FloatImageType>(const FloatImageType*);

// threshold_image instantiation for FloatImageType
template FloatImageType::Pointer
threshold_image<FloatImageType>(
        const FloatImageType * any_input_image,
        const FloatImageType::PixelType &lower_threshold,
        const FloatImageType::PixelType &upper_threshold,
        const FloatImageType::PixelType &outside_value);

// binarize_with_threshold instantiation for FloatImageType
template BinaryImageType::Pointer
binarize_with_threshold<FloatImageType>(
        const FloatImageType * any_input_image,
        const FloatImageType::PixelType &lower_threshold,
        const FloatImageType::PixelType &upper_threshold);

// binarize_with_percentage instantiation for FloatImageType
template BinaryImageType::Pointer
binarize_with_percentage<FloatImageType>(
        const FloatImageType * any_input_image,
        const double percentage);

// binarize_with_region_growing instantiation for FloatImageType
template BinaryImageType::Pointer
binarize_with_region_growing<FloatImageType>(
        const FloatImageType * any_input_image,
        const BinaryImageType * binary_image_with_seeds,
        const FloatImageType::PixelType &lower_threshold,
        const FloatImageType::PixelType &upper_threshold,
        const std::string &connectivity_str);
} // end namespace SG
