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
