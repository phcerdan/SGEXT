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

#ifndef SEGMENTATION_FUNCTIONS_HXX
#define SEGMENTATION_FUNCTIONS_HXX

#include "image_types.hpp"

#include <itkBinaryThresholdImageFilter.h>
#include <itkConnectedThresholdImageFilter.h>
#include <itkMinimumMaximumImageFilter.h>
#include <itkThresholdImageFilter.h>

// Template functions which are instantiated for
// BinaryImageType and FloatImageType in the .cpp file.
// You can use them for any other ImageType.
// Because the `extern template` it needs linking to the file
// that instantiate those types (segmentation_functions.cpp).

namespace SG {

/**
 * Compute min and max values from input image
 *
 * @tparam AnyImageType any itk::Image
 * @param any_input_image input image
 *
 * @return pair(min, max)
 */
template <typename AnyImageType>
std::pair<typename AnyImageType::PixelType, typename AnyImageType::PixelType>
min_max_values(const AnyImageType *any_input_image) {
    // Compute the min and max of input image
    using MinMaxFilter = itk::MinimumMaximumImageFilter<AnyImageType>;
    auto min_max_filter = MinMaxFilter::New();
    min_max_filter->SetInput(any_input_image);
    min_max_filter->Update();
    // return {min, max}
    return std::make_pair(min_max_filter->GetMinimum(),
                          min_max_filter->GetMaximum());
}

/**
 * Returns an image of the same type than input image with the pixels
 * outside of the range [lower_threshold, upper_threshold] set to outside_value.
 *
 * Useful to remove high value pixels before any other kind of binarization.
 *
 * @tparam AnyImageType any itk::Image
 * @param any_input_image input image
 * @param lower_threshold pixels with value lower than this will be set to
 * ::lowest() Defaults to the lowest value for the pixel type.
 * @param upper_threshold pixels with value greater than this will be set to
 * ::lowest() Defaults to the maximum of the pixel type.
 * @param outside_value  set the value of the pixels outside the threshold to
 * this value. Defaults to the lowest of the pixel type. It might be useful to
 * set it to the already min value of the image
 *   @see min_max_values
 *
 * @return image of the same type than input image
 */
template <typename AnyImageType>
typename AnyImageType::Pointer threshold_image(
        const AnyImageType *any_input_image,
        const typename AnyImageType::PixelType &lower_threshold =
                itk::NumericTraits<typename AnyImageType::PixelType>::lowest(),
        const typename AnyImageType::PixelType &upper_threshold =
                itk::NumericTraits<typename AnyImageType::PixelType>::max(),
        const typename AnyImageType::PixelType &outside_value = itk::
                NumericTraits<typename AnyImageType::PixelType>::lowest()) {
    using ThresholdFilter = itk::ThresholdImageFilter<AnyImageType>;
    auto threshold_filter = ThresholdFilter::New();
    threshold_filter->SetInput(any_input_image);
    // The default OutsideValue is ::Zero(), but prefer lowest()
    threshold_filter->SetOutsideValue(outside_value);
    threshold_filter->ThresholdOutside(lower_threshold, upper_threshold);
    threshold_filter->Update();
    return threshold_filter->GetOutput();
}

/**
 * Binarize input image using a lower and upper threshold.
 *
 * @tparam AnyImageType any itk::Image
 * @param any_input_image input image
 * @param lower_threshold pixels with value lower than this won't be ON.
 *   Defaults to the lowest value for the pixel type.
 * @param upper_threshold pixels with value greater than this won't be ON.
 *   Defaults to the maximum of the pixel type.
 *
 * @return  binarized image
 */
template <typename AnyImageType>
BinaryImageType::Pointer binarize_with_threshold(
        const AnyImageType *any_input_image,
        const typename AnyImageType::PixelType &lower_threshold =
                itk::NumericTraits<typename AnyImageType::PixelType>::lowest(),
        const typename AnyImageType::PixelType &upper_threshold =
                itk::NumericTraits<typename AnyImageType::PixelType>::max()) {
    using BinaryThresholdFilter =
            itk::BinaryThresholdImageFilter<AnyImageType, BinaryImageType>;
    auto binary_filter = BinaryThresholdFilter::New();
    binary_filter->SetInput(any_input_image);
    binary_filter->SetLowerThreshold(lower_threshold);
    binary_filter->SetUpperThreshold(upper_threshold);
    binary_filter->Update();
    return binary_filter->GetOutput();
}

/* *************** connected components *********************/
/**
 * Struct holding the result of connected_components
 */
struct ConnectedComponentsOutput {
    /**
     * label image where label 0 is background,
     * label 1 is the biggest object in number of pixels, etc.
     */
    BinaryImageType::Pointer label_image;
    /** The number of disconnected objects/labels */
    size_t number_of_labels;
    /**
     * The size of each labeled object by the number of pixels.
     * size_of_labels[0] corresponds to label 1 (the label with the largest
     * object) the number of background pixels (label 0) is not computed.
     */
    std::vector<size_t> size_of_labels;
};

/**
 * Compute the number of connected componentes of input image and label them
 * from 0 to N (where N is the total number_of_labels).
 * Label/Value 0 is the background
 * Label/Value 1 corresponds to the largest object (in number of pixels), etc.
 *
 * The output is returned as a @ConnectedComponentsOutput struct.
 *
 * If you want to extract a particular label, use extract_a_label.
 * Example:
 * ```
 *  auto connected_components_output = connected_components(input_binary_image);
 *  auto largest_component_binary_image =
 *      extract_a_label(connected_components_output.label_image, 1);
 * ```
 *
 * @param input_binary_image input binary image.
 *
 * @return ConnectedComponentsOutput holding the label_image and information
 * about the number of labels and the size of them.
 */
ConnectedComponentsOutput
connected_components(const BinaryImageType *input_binary_image);

BinaryImageType::
        Pointer
        /**
         * Extract a particular label from a binary (labeled) image.
         * Usually used in conjunction with the output of @connected_components.
         *
         * @param input_label_image input binary image but with labels (from 0
         * to 255)
         * @param label particular label (from 0 to 255) to extract.
         */
        extract_a_label(const BinaryImageType *input_label_image,
                        const size_t &label);

/**
 * Binarize input image using a percentage to set the lower_threshold.
 *
 * We understand as a safe binarization, one which does not include false
 * positives. The lower the input percentage the less pixels will be included
 * (safer the binarization). A percentage 0.2 indicates that 20% of the pixels
 * (with highest value) will be considered as ON pixels.
 *
 * With percentage = 0.0, only the max_value of the image will be ON.
 * With percentage = 1.0, the returned binary image would have all its pixels ON
 * (completely white image).
 *
 * lower_threshold = min_value + (max_value - min_value) * (1 - percentage)
 * upper_threshold = max_value
 *
 * If you need to remove high value pixels, use
 *
 * @tparam AnyImageType any itk::Image
 * @param any_input_image input image
 * @param percentage percentage of pixels (with highest value) that will be ON
 * in the output image
 *
 * @return binary image
 */
template <typename AnyImageType>
BinaryImageType::Pointer
binarize_with_percentage(const AnyImageType *any_input_image,
                         const double percentage = 0.05) {

    if (percentage < 0.0 || percentage > 1.0) {
        throw std::runtime_error("percentage has to be in the range: [0.0, "
                                 "1.0]. Actual value: " +
                                 std::to_string(percentage));
    }

    const auto min_max = min_max_values<AnyImageType>(any_input_image);
    const auto &min_value = min_max.first;
    const auto &max_value = min_max.second;

    // Compute the lower_threshold using max, min and percentage.
    const double lower_threshold =
            min_value + (max_value - min_value) * (1.0 - percentage);
    // upper_threshold is the max value by default in the following filter

    using BinaryThresholdFilter =
            itk::BinaryThresholdImageFilter<AnyImageType, BinaryImageType>;
    auto binary_filter = BinaryThresholdFilter::New();
    binary_filter->SetInput(any_input_image);
    binary_filter->SetLowerThreshold(lower_threshold);
    binary_filter->Update();
    return binary_filter->GetOutput();
}

/**
 * Binarize input image using region growing from an initial binary image with
 * seeds.
 *
 * @tparam AnyImageType any itk::Image
 * @param any_input_image input image to binarize
 * @param binary_image_with_seeds safe binarization (no false positives)
 * @param lower_threshold lower threshold
 * @param upper_threshold upper threshold (defaults to max value of image)
 * @param connectivity_str neighbor connectivity for the growing.
 *   Options:"26" (full connectivity) or "6" (face connectivity).  Defaults to
 * "26"
 *
 * @return  binarized image after region growing
 */
template <typename AnyImageType>
BinaryImageType::Pointer binarize_with_region_growing(
        const AnyImageType *any_input_image,
        const BinaryImageType *binary_image_with_seeds,
        const typename AnyImageType::PixelType &lower_threshold,
        const typename AnyImageType::PixelType &upper_threshold =
                itk::NumericTraits<typename AnyImageType::PixelType>::max(),
        const std::string &connectivity_str = "26" // or "6"
) {
    if (connectivity_str != "26" && connectivity_str != "6") {
        throw std::runtime_error(
                "Invalid connectivity_str (" + connectivity_str +
                "). Valid values are: \"26\" (full) and \"6\" (face).");
    }
    using ConnectedFilterType =
            itk::ConnectedThresholdImageFilter<AnyImageType, BinaryImageType>;
    auto connected_threshold = ConnectedFilterType::New();
    connected_threshold->SetInput(any_input_image);
    connected_threshold->SetLower(lower_threshold);
    connected_threshold->SetUpper(upper_threshold);
    connected_threshold->SetReplaceValue(255);
    auto connectivity = (connectivity_str == "26")
                                ? itk::ConnectedThresholdImageFilterEnums::
                                          Connectivity::FullConnectivity
                                : itk::ConnectedThresholdImageFilterEnums::
                                          Connectivity::FaceConnectivity;
    connected_threshold->SetConnectivity(connectivity);
    // Set seeds from binary image
    using OutputIteratorType =
            itk::ImageRegionConstIteratorWithIndex<BinaryImageType>;
    OutputIteratorType it(binary_image_with_seeds,
                          binary_image_with_seeds->GetLargestPossibleRegion());
    for (it.GoToBegin(); !it.IsAtEnd(); ++it) {
        if (it.Get() > itk::NumericTraits<BinaryImageType::PixelType>::Zero) {
            connected_threshold->AddSeed(it.GetIndex());
        }
    }
    connected_threshold->Update();
    return connected_threshold->GetOutput();
}

/**
 * Input parameters for the @see binarize_with_level_set pipeline
 */
struct binarize_with_level_set_parameters {
    /** If gradient_sigma is -999, then  the gradient_sigma used is:
     *  0.5 * input_image.GetSpacing()[0] */
    double gradient_sigma = -999;
    double sigmoid_alpha = -0.5;
    double level_set_propagation_scaling = 1.0;
    double level_set_curvature_scaling = 1.0;
    double level_set_advection_scaling = 1.0;
    double level_set_maximum_RMS_error = 0.02;
    unsigned int level_set_iterations = 20;
    FloatImageType::PixelType binary_upper_threshold = 1.41;
    BinaryImageType::PixelType binary_inside_value = 255;
};
void print_binarize_with_level_set_parameters(
        const binarize_with_level_set_parameters &parameters, std::ostream &os);

/**
 * Output of @see binarize_with_level_set
 */
struct binarize_with_level_set_output {
    /// input parameters @see binarize_with_level_set_parameters
    binarize_with_level_set_parameters parameters;
    /// output binary image after level set
    BinaryImageType::Pointer output_binary_image;
    /// gradient image of the input float image. (might be null
    FloatImageType::Pointer gradient_image;
    /// sigmoid after gradient
    FloatImageType::Pointer sigmoid_image;
    /// level set using sigmoid and signed distance from input binary image.
    FloatImageType::Pointer level_set_image;
};

/**
 * Binarize input image using level sets.
 *
 * Use a mini-pipeline and GeodesicActiveContourLevelSetImageFilter
 * to improve the input initial binarization using level sets.
 * The initial binarization should not contain false positives.
 * Use @see binarize_with_threshold_percentage with a low percentage for this.
 * Pipeline:
 *  gradient (from float_image). Parameters: gradient_sigma
 *  sigmoid (from gradient). Parameters: sigmoid_alpha
 *  signed distance (from binary_image_safe)
 *  level_set from (sigmoid and signed and distance)
 *  output from thresholding the level_set. Parameters: binary_upper_threshold
 *
 * @param input_float_image input image to binarize
 * @param binary_image_safe safe binarization (shoudn't contain false positives)
 * @param input_parameters parameters for all the filters of the pipeline
 * @param save_intermediate_results if true, the output will store
 * images from the intermediate filters.
 *
 * @return struct storing the resulting binarization, input parameters
 * and optionally, images from intermediate filters of the pipeline.
 */
binarize_with_level_set_output binarize_with_level_set(
        const FloatImageType *input_float_image,
        const BinaryImageType *binary_image_safe,
        const binarize_with_level_set_parameters &input_parameters =
                binarize_with_level_set_parameters(),
        const bool save_intermediate_results = false);

// Explicitly instantiated in segmentation_functions.cpp
extern template std::pair<BinaryImageType::PixelType,
                          BinaryImageType::PixelType>
min_max_values<BinaryImageType>(const BinaryImageType *);
extern template std::pair<FloatImageType::PixelType, FloatImageType::PixelType>
min_max_values<FloatImageType>(const FloatImageType *);

extern template FloatImageType::Pointer threshold_image<FloatImageType>(
        const FloatImageType *any_input_image,
        const FloatImageType::PixelType &lower_threshold,
        const FloatImageType::PixelType &upper_threshold,
        const FloatImageType::PixelType &outside_value);

extern template BinaryImageType::Pointer
binarize_with_threshold<FloatImageType>(
        const FloatImageType *any_input_image,
        const FloatImageType::PixelType &lower_threshold,
        const FloatImageType::PixelType &upper_threshold);

extern template BinaryImageType::Pointer
binarize_with_percentage<FloatImageType>(const FloatImageType *any_input_image,
                                         const double percentage);

extern template BinaryImageType::Pointer
binarize_with_region_growing<FloatImageType>(
        const FloatImageType *any_input_image,
        const BinaryImageType *binary_image_with_seeds,
        const FloatImageType::PixelType &lower_threshold,
        const FloatImageType::PixelType &upper_threshold,
        const std::string &connectivity_str);

} // end namespace SG
#endif
