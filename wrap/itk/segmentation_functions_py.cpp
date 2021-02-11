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

#include "pybind11_common.h"

#include "segmentation_functions.hpp"

namespace py = pybind11;
using namespace SG;

void init_segmentation_functions(py::module &m) {

    /********************** min_max_values ***************************/

    const std::string min_max_values_docs =
            R"(Compute min and max values from input image.

Parameters:
-----------
input: BinaryImageType || FloatImageType
    input image

Returns:
-------
[min_value, max_value]
)";
    m.def(
            "min_max_values",
            [](const BinaryImageType::Pointer &input) {
                return min_max_values<BinaryImageType>(input);
            },
            min_max_values_docs.c_str(), py::arg("input"));
    m.def(
            "min_max_values",
            [](const FloatImageType::Pointer &input) {
                return min_max_values<FloatImageType>(input);
            },
            min_max_values_docs.c_str(), py::arg("input"));

    /*************** threshold_image **********************************/
    m.def(
            "threshold_image",
            [](const FloatImageType::Pointer &input,
               const FloatImageType::PixelType &lower_threshold,
               const FloatImageType::PixelType &upper_threshold,
               const FloatImageType::PixelType &outside_value) {
                return threshold_image<FloatImageType>(
                        input, lower_threshold, upper_threshold, outside_value);
            },
            R"(Returns an image of the same type than input image with the pixels
outside of the range [lower_threshold, upper_threshold] set to outside_value.

Parameters:
-----------
input: FloatImageType
    input image

lower_threshold: Float
    pixels with value lower than this wil be set to ::lowest() possible value.
    Defaults to the lowest value for the pixel type.

upper_threshold: Float
    pixels with value greater than this  will be set to ::lowest() possible value.
    Defaults to the maximum of the pixel type.

outside_value: Float
    sets the value of the pixels outside the thresholds range to this value.
    Defaults to the lowest possible value of the pixel type.
    It might be useful to set it to the already min value of the image
    See min_max_values

Returns:
--------
FloatImageType (same type than input)
)",
            py::arg("input"),
            py::arg("lower_threshold") =
                    itk::NumericTraits<FloatImageType::PixelType>::lowest(),
            py::arg("upper_threshold") =
                    itk::NumericTraits<FloatImageType::PixelType>::max(),
            py::arg("outside_value") =
                    itk::NumericTraits<FloatImageType::PixelType>::lowest());

    /*************** binarize_with_threshold **************************/

    m.def(
            "binarize_with_threshold",
            [](const FloatImageType::Pointer &input,
               const FloatImageType::PixelType &lower_threshold,
               const FloatImageType::PixelType &upper_threshold) {
                return binarize_with_threshold<FloatImageType>(
                        input, lower_threshold, upper_threshold);
            },
            R"(Binarize input image using a lower and upper threshold.

Parameters:
-----------
input: FloatImageType
    input image

lower_threshold: Float
    pixels with value lower than this won't be ON.
    Defaults to the lowest value for the pixel type.

upper_threshold: Float
    pixels with value greater than this won't be ON.
    Defaults to the maximum of the pixel type.

Returns:
--------
BinaryImageType
)",
            py::arg("input"),
            py::arg("lower_threshold") =
                    itk::NumericTraits<FloatImageType::PixelType>::lowest(),
            py::arg("upper_threshold") =
                    itk::NumericTraits<FloatImageType::PixelType>::max());

    /*************** binarize_with_threshold_percentage ***************/

    m.def(
            "binarize_with_percentage",
            [](const FloatImageType::Pointer &input, const double percentage) {
                return binarize_with_percentage<FloatImageType>(input,
                                                                percentage);
            },
            R"(Binarize input image using a percentage to set the lower_threshold.

We understand as a safe binarization, one which does not include false positives.
The lower the input percentage the less pixels will be included (safer the binarization).
A percentage 0.2 indicates that 20% of the pixels (with highest value) will
be considered as ON pixels.

With percentage = 0.0, only the max_value of the image will be ON.
With percentage = 1.0, the returned binary image would have all its pixels ON
(completely white image).

lower_threshold = min_value + (max_value - min_value) * (1 - percentage)
upper_threshold = max_value

Parameters:
-----------
input: FloatImageType
    input image

percentage: Float
    percentage percentage of pixels (with highest value) that will be ON in the output image
    lower_threshold = min_value + (max_value - min_value) * (1 - percentage)

Returns:
--------
BinaryImageType
)",
            py::arg("input"), py::arg("percentage") = 0.05);

    /*************** binarize_with_region_growing ***************/

    m.def(
            "binarize_with_region_growing",
            [](const FloatImageType::Pointer &input,
               const BinaryImageType::Pointer &binary_init,
               const FloatImageType::PixelType &lower_threshold,
               const FloatImageType::PixelType &upper_threshold,
               const std::string &connectivity_str) {
                return binarize_with_region_growing(
                        input.GetPointer(), binary_init.GetPointer(),
                        lower_threshold, upper_threshold, connectivity_str);
            },
            R"(Binarize input image using region growing from an initial binary image with seeds.

Parameters:
-----------
input: FloatImageType
    input image

binary_init: BinaryImageType
    BinaryImage with seeds (i.e a safe binary image with no false positives).

lower_threshold: Float
    pixels with value lower than this won't be ever turned ON in the growing.

upper_threshold: Float
    pixels with value upper than this won't be ever turned ON in the growing.
    defaults to max_value found in the image.

connectivity: str
    Neighbor connectivity. Options: "26" (full connectivity) or "6".
    Defaults to "26".

Returns:
--------
BinaryImageType with the improved binarization using region growing.
)",
            py::arg("input"), py::arg("binary_init"),
            py::arg("lower_threshold"),
            py::arg("upper_threshold") =
                    itk::NumericTraits<FloatImageType::PixelType>::max(),
            py::arg("connectivity") = "26");

    /*************** binarize_with_level_set ***************/

    py::class_<binarize_with_level_set_parameters>(
            m, "binarize_with_level_set_parameters")
            .def(py::init())
            .def_readwrite("gradient_sigma",
                           &binarize_with_level_set_parameters::gradient_sigma)
            .def_readwrite("sigmoid_alpha",
                           &binarize_with_level_set_parameters::sigmoid_alpha)
            .def_readwrite("level_set_propagation_scaling",
                           &binarize_with_level_set_parameters::
                                   level_set_propagation_scaling)
            .def_readwrite("level_set_curvature_scaling",
                           &binarize_with_level_set_parameters::
                                   level_set_curvature_scaling)
            .def_readwrite("level_set_advection_scaling",
                           &binarize_with_level_set_parameters::
                                   level_set_advection_scaling)
            .def_readwrite("level_set_maximum_RMS_error",
                           &binarize_with_level_set_parameters::
                                   level_set_maximum_RMS_error)
            .def_readwrite(
                    "level_set_iterations",
                    &binarize_with_level_set_parameters::level_set_iterations)
            .def_readwrite(
                    "binary_upper_threshold",
                    &binarize_with_level_set_parameters::binary_upper_threshold)
            .def_readwrite(
                    "binary_inside_value",
                    &binarize_with_level_set_parameters::binary_inside_value)
            .def("__str__", [](const binarize_with_level_set_parameters &self) {
                std::stringstream os;
                print_binarize_with_level_set_parameters(self, os);
                return os.str();
            });

    py::class_<binarize_with_level_set_output>(m,
                                               "binarize_with_level_set_output")
            .def(py::init())
            .def_readwrite("parameters",
                           &binarize_with_level_set_output::parameters)
            .def_readwrite("output_binary_image",
                           &binarize_with_level_set_output::output_binary_image)
            .def_readwrite("gradient_image",
                           &binarize_with_level_set_output::gradient_image)
            .def_readwrite("sigmoid_image",
                           &binarize_with_level_set_output::sigmoid_image)
            .def_readwrite("level_set_image",
                           &binarize_with_level_set_output::level_set_image)
            .def("__str__", [](const binarize_with_level_set_output &self) {
                std::stringstream os;
                auto parameters_py = py::cast(self.parameters);
                os << "parameters: " << std::endl;
                os << parameters_py.attr("__repr__")();
                os << " ---" << std::endl;
                os << "output_binary_image: " << self.output_binary_image
                   << std::endl;
                os << "gradient_image: " << self.gradient_image << std::endl;
                os << "sigmoid_image: " << self.sigmoid_image << std::endl;
                os << "level_set_image: " << self.level_set_image << std::endl;
                return os.str();
            });

    m.def(
            "binarize_with_level_set",
            [](const FloatImageType::Pointer &input,
               const BinaryImageType::Pointer &binary_init,
               const binarize_with_level_set_parameters &input_parameters,
               const bool save_intermediate_results) {
                return binarize_with_level_set(
                        input.GetPointer(), binary_init.GetPointer(),
                        input_parameters, save_intermediate_results);
            },
            R"(Binarize input image using level sets.

Use a mini-pipeline and GeodesicActiveContourLevelSetImageFilter
to improve the input initial binarization using level sets.
The initial binarization should not contain false positives.
Use binarize_with_threshold_percentage with a low percentage for this.

Parameters:
-----------
input: FloatImageType
    input image

binary_init: BinaryImageType
    BinaryImage with seeds (i.e a safe binary image with no false positives).

parameters: binarize_with_level_set_parameters
    input parameters. Default to a reasonable set.

save_intermediate_results: Bool
    store intermediate images from the pipeline in the output.
    useful to tweak input parameters.

Returns:
--------
BinaryImageType with the improved binarization using level sets.

)",
            py::arg("input"), py::arg("binary_init"),
            py::arg("parameters") = binarize_with_level_set_parameters(),
            py::arg("save_intermediate_results") = false);

    /*************** connected_components ****************/
    py::class_<ConnectedComponentsOutput>(
            m, "connected_components_output",
            R"(Struct holding the result of connected_components.)")
            .def(py::init())
            .def_readwrite("label_image",
                           &ConnectedComponentsOutput::label_image)
            .def_readwrite("number_of_labels",
                           &ConnectedComponentsOutput::number_of_labels)
            .def_readwrite("size_of_labels",
                           &ConnectedComponentsOutput::size_of_labels)
            .def("__str__", [](const ConnectedComponentsOutput &self) {
                std::stringstream os;
                os << "number_of_labels: " << self.number_of_labels
                   << std::endl;
                os << "size_of_labels:\n  ";
                for (const auto &s : self.size_of_labels) {
                    os << s << ", ";
                }
                os << std::endl;
                return os.str();
            });

    m.def(
            "connected_components",
            [](const BinaryImageType::Pointer &input) {
                return connected_components(input.GetPointer());
            },
            R"(Compute the number of connected componentes of input image and label them
from 0 to N (where N is the total number_of_labels).

Label/Value 0 is the background
Label/Value 1 corresponds to the largest object (in number of pixels), etc.

The output is returned as a connected_components_output structure.

If you want to extract a particular label, use extract_a_label.
Example:
    connected_components_output = connected_components(input_binary_image);
    auto largest_component_binary_image =
        extract_a_label(connected_components_output.label_image, label=1);

Parameters:
-----------
input: BinaryImageType
    input image

Returns:
--------
connected_components_output: object holding the label_image and information
about the number of labels and the size of them.
)",
            py::arg("input"));

    m.def(
            "extract_a_label",
            [](const BinaryImageType::Pointer &input, const size_t &label) {
                return extract_a_label(input.GetPointer(), label);
            },
            R"(Extract a particular label from a binary (labeled) image.

Usually used in conjunction with the output of connected_components.

Parameters:
-----------
input: BinaryImageType
    input binary image with labels (from 1 to 255)
label: int
    particular label (from 1 to 255) to extract.
)",
            py::arg("input"), py::arg("label"));
}
