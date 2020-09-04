/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "morphological_watershed.hpp"
#include <itkInvertIntensityImageFilter.h>
#include <itkMaskImageFilter.h>
#include <itkMorphologicalWatershedFromMarkersImageFilter.h>

namespace SG {

BinaryImageType::Pointer
morphological_watershed(const BinaryImageType::Pointer &original_binary_image,
                        const BinaryImageType::Pointer &label_marker_image) {

    // Use the the inverted binary original binary image instead of a gradient
    // for setting the watershed contour.
    using InverterType =
            itk::InvertIntensityImageFilter<BinaryImageType, BinaryImageType>;
    auto inverter = InverterType::New();
    inverter->SetInput(original_binary_image);
    inverter->Update();

    using WatershedFilterType =
            itk::MorphologicalWatershedFromMarkersImageFilter<BinaryImageType,
                                                              BinaryImageType>;
    auto watershed_filter = WatershedFilterType::New();
    watershed_filter->SetInput1(inverter->GetOutput());
    watershed_filter->SetMarkerImage(label_marker_image);
    // If On, voxels values between labels would be zero, creating disconnected
    // components.
    watershed_filter->MarkWatershedLineOff();
    // Fully connected always on for label images.
    watershed_filter->FullyConnectedOn();

    // Mask the watershed image with the original_binary_image
    // because watershed always bleeds out.
    using MaskFilterType =
            itk::MaskImageFilter<BinaryImageType, BinaryImageType>;
    auto mask_filter = MaskFilterType::New();
    mask_filter->SetInput(watershed_filter->GetOutput());
    mask_filter->SetMaskImage(original_binary_image);
    mask_filter->Update();

    return mask_filter->GetOutput();
}

} // end namespace SG
