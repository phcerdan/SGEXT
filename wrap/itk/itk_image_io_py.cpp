/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"
#include "sgitk_common_py.hpp"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

namespace py = pybind11;

void init_itk_image_io(py::module &m) {
// clang-format off
    const std::string read_as_common_docs = R"delimiter(
Note that this is using ITK to read the image, but the output
type is internal to sgext.
To convert an existing sgext_image to an ITK type, use:
    itk_image = itk.GetImageViewFromArray(sgext_image.to_pyarray())

Parameters
==========
input_file: string
    input file with image
)delimiter";
// clang-format on

    m.def("read_as_float",
            [](const std::string & input_file) -> SG::IF3P {
                using ReaderType = itk::ImageFileReader<SG::IF3>;
                auto reader = ReaderType::New();
                reader->SetFileName(input_file);
                reader->Update();
                return reader->GetOutput();
                },
            ("Read image from file into the float (IF3) image "
            "type used internally in sgext.\n" + read_as_common_docs).c_str(),
            py::arg("input_file")
         );

    m.def("read_as_binary",
            [](const std::string & input_file) -> SG::IUC3P {
                using ReaderType = itk::ImageFileReader<SG::IUC3>;
                auto reader = ReaderType::New();
                reader->SetFileName(input_file);
                reader->Update();
                return reader->GetOutput();
                },
            ("Read image from file into the binary (IUC3) image "
            "type used internally in sgext.\n" + read_as_common_docs).c_str(),
            py::arg("input_file")
         );
// clang-format off
    const std::string write_common_docs = R"delimiter(
Write image into file.

Parameters
==========
input_image: BinaryImageType || FloatImageType
    sgext image type (binary or float)

output_file: string
    filename to save the image
)delimiter";
// clang-format on
    m.def("write",
            [](SG::IUC3P input_image, const std::string & out_file) -> void {
                using WriterType = itk::ImageFileWriter<SG::IUC3>;
                auto writer = WriterType::New();
                writer->SetInput(input_image);
                writer->SetFileName(out_file);
                writer->Update();
            },
            write_common_docs.c_str(),
            py::arg("input_image"),
            py::arg("output_file")
         );
    m.def("write",
            [](SG::IF3P input_image, const std::string & out_file) -> void {
                using WriterType = itk::ImageFileWriter<SG::IF3>;
                auto writer = WriterType::New();
                writer->SetInput(input_image);
                writer->SetFileName(out_file);
                writer->Update();
            },
            write_common_docs.c_str(),
            py::arg("input_image"),
            py::arg("output_file")
         );
}
