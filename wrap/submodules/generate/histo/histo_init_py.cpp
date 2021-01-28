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
#include "declare_histo_py.hpp"

namespace py = pybind11;
void init_histo(py::module & mparent) {
    auto m = mparent.def_submodule("histo");
    m.doc() = "Wrappings for Histo<double,size_t> "; // optional module docstring

    using SG_Histogram = histo::Histo<double, size_t>;
    histo::declare_histo<SG_Histogram>(m, "histo");

    using NormalizedByAreaHistogram = histo::Histo<double, double>;
    histo::declare_histo<NormalizedByAreaHistogram>(m, "histo_normalized");

    // Free functions
    m.def("generate_breaks_from_range_and_bins",
            py::overload_cast<const double&, const double&, const size_t&>(
            &histo::GenerateBreaksFromRangeAndBins<double>),
            py::arg("low"), py::arg("upper"), py::arg("bins"));
    m.def("generate_breaks_from_range_and_bins",
            py::overload_cast<const std::pair<double, double> &, const size_t&>(
            &histo::GenerateBreaksFromRangeAndBins<double>),
            py::arg("range"), py::arg("bins"));
    m.def("generate_breaks_from_range_and_width",
            py::overload_cast<const double&, const double&, const double&>(
            &histo::GenerateBreaksFromRangeAndWidth<double>),
            py::arg("low"), py::arg("upper"), py::arg("width"));
    m.def("generate_breaks_from_range_and_width",
            py::overload_cast<const std::pair<double, double> &, const double&>(
            &histo::GenerateBreaksFromRangeAndWidth<double>),
            py::arg("range"), py::arg("width"));

    const std::string create_histogram_normalized_docs = R"(
Normalize the histogram by area. Useful for probability density functions.
The output histogram has counts with double precision, instead of size_t.
Implemented using:
https://stackoverflow.com/questions/5320677/how-to-normalize-a-histogram-in-matlab

@param input_histo input histogram to normalize.

@return histo_normalized with float counts.
)";
    m.def("create_histogram_normalized_by_area",
          py::overload_cast<const SG_Histogram &>(
                  &histo::NormalizeByArea<double, size_t>),
          create_histogram_normalized_docs.c_str());
    m.def("create_histogram_normalized_by_area",
          py::overload_cast<const NormalizedByAreaHistogram &>(
                  &histo::NormalizeByArea<double, double>),
          create_histogram_normalized_docs.c_str());
}
