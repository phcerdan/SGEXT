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
#include "sgitk_common_py.hpp"

#include "itkImageFileReader.h"

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
}
