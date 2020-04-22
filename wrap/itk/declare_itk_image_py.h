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

#ifndef DECLARE_ITK_IMAGE_PY_H
#define DECLARE_ITK_IMAGE_PY_H

#include "pybind11_common.h"

#include "itkImage.h"
#include "itkSmartPointer.h"
#include "itkImportImageFilter.h"

/** For now make copies of data to numpy arrays.
 * Some examples (pybind11 source code docs are non-existant)
 * From: https://github.com/pybind/pybind11/issues/323
 * Also: https://github.com/pybind/pybind11/issues/1042
 * dtypes: https://docs.scipy.org/doc/numpy/user/basics.types.html
 * py::dtype::of<TIMG::ObjectType::PixelType>(),
 */
template<typename TImagePointer>
void declare_itk_image_ptr(pybind11::module &m, const std::string &typestr) {
    namespace py = pybind11;
    // const std::string pyclass_name = std::string("itk_") + typestr;
    py::class_<TImagePointer>(m, typestr.c_str())
        .def(py::init([](){
                    return TImagePointer::ObjectType::New();
                    }
                    ))
        .def("dimension", [](const TImagePointer & img) {
                return img->ImageDimension;
                })
        .def("size", [](const TImagePointer & img) {
                return py::array(
                        img->ImageDimension, // shape
                        img->GetLargestPossibleRegion().GetSize().data());
                })
        .def("index", [](const TImagePointer & img) {
                return py::array(
                        img->ImageDimension, // shape
                        img->GetLargestPossibleRegion().GetIndex().data());
                })
        .def("spacing", [](const TImagePointer & img) {
                return py::array(
                        img->ImageDimension, // shape
                        img->GetSpacing().GetDataPointer());
                })
        .def("origin", [](const TImagePointer & img) {
                return py::array(
                        img->ImageDimension, // shape
                        img->GetOrigin().GetDataPointer());
                })
        .def("direction", [](const TImagePointer & img) {
                const std::vector<size_t> shape{
                img->ImageDimension, img->ImageDimension};
                return py::array(
                        shape,
                        img->GetDirection().GetVnlMatrix().data_block());
                })
        // Follow numpy: CONTIG can be 'C' or 'F'
        // numpy default is F <- this is a pain coming from C data.
        // python ecosystem assume a F layout, so we return it as the default.
        .def("to_pyarray", [](const TImagePointer & img,
                    const std::string & contiguous) {
                const auto size = img->GetLargestPossibleRegion().GetSize();
                const auto shape = (contiguous == "F") ?
                std::vector<size_t>{size[2], size[1], size[1]} :
                std::vector<size_t>{size[0], size[1], size[2]};
                return py::array(
                        py::dtype::of<typename TImagePointer::ObjectType::PixelType>(),
                        shape,
                        img->GetBufferPointer()
                        );
                },
                    py::arg("contig") = "F")
        // TODO: Create a view (non-copy) of the data
        // Problems will arise with the contig differences between numpy(fortran) and c.
        .def("as_pyarray", [](const TImagePointer & img,
                    const std::string & contiguous) {
                throw std::runtime_error("not implemented, use to_pyarray");
                },
                py::arg("contig") = "F")

        .def("from_pyarray", [](TImagePointer & img,
                    py::array_t<typename TImagePointer::ObjectType::PixelType> np_array) {
                using PixelType = typename TImagePointer::ObjectType::PixelType;
                using Image = typename TImagePointer::ObjectType;
                constexpr auto ImageDimension = Image::ImageDimension;
                using ImporterType = itk::ImportImageFilter<PixelType, ImageDimension>;
                auto importer = ImporterType::New();
                importer->SetRegion(img->GetLargestPossibleRegion());
                importer->SetOrigin(img->GetOrigin());
                importer->SetSpacing(img->GetSpacing());
                importer->SetDirection(img->GetDirection());
                // img owns the buffer, not the import filter
                const bool importImageFilterWillOwnTheBuffer = false;
                auto info = np_array.request();
                auto * data = static_cast<typename TImagePointer::ObjectType::PixelType*>(info.ptr);
                const auto numberOfPixels = np_array.size();
                importer->SetImportPointer(
                        data, numberOfPixels, importImageFilterWillOwnTheBuffer);
                importer->Update();
                img = importer->GetOutput();
                })

        .def("__repr__", [](const TImagePointer &img) {
                std::stringstream os;
                os << "Dimension: " << img->ImageDimension << std::endl;
                os << "LargestPossibleRegion: " << std::endl;
                os << "  Index: " << img->GetLargestPossibleRegion().GetIndex() << std::endl;
                os << "  Size (i,j,k) (c_array): " << img->GetLargestPossibleRegion().GetSize() << std::endl;
                os << "Origin: " << img->GetOrigin() << std::endl;
                os << "Spacing: " << img->GetSpacing() << std::endl;
                os << "Direction: " << std::endl;
                os << img->GetDirection();
                os << "Buffer: " << std::endl;
                img->GetPixelContainer()->Print(os);
                return os.str();
                });
}

#endif
