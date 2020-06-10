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

#ifndef DECLARE_HISTO_PY_HPP
#define DECLARE_HISTO_PY_HPP

#include "histo.hpp"
#include "pybind11_common.h"

#ifdef SG_USING_VTK
#include "visualize_histo.hpp"
#endif

namespace histo {

template <typename THisto>
void declare_histo(pybind11::module &m, const std::string &typestr) {
    namespace py = pybind11;

    using Histogram = THisto;
    using DataVector = std::vector<double>;

    auto histo = py::class_<Histogram>(m, typestr.c_str());
    // Constructors
    histo.def(py::init());
    // Constructors with input data.
    // Use default binning method, Scott when no breaks are provided.
    histo.def(py::init([](const DataVector &data) {
        return std::make_unique<Histogram>(data, histo::breaks_method::Scott);
    }));
    histo.def(py::init([](const DataVector &data,
                          const typename Histogram::RangeType input_range) {
        return std::make_unique<Histogram>(data, input_range,
                                           histo::breaks_method::Scott);
    }));
    histo.def(py::init([](const DataVector &data,
                          const typename Histogram::BreaksType input_breaks) {
        return std::make_unique<Histogram>(data, input_breaks);
    }));
    histo.def("print_breaks", [](const Histogram &histo) {
        std::stringstream os;
        histo.PrintBreaks(os);
        return os.str();
    });
    histo.def("print_counts", [](const Histogram &histo) {
        std::stringstream os;
        histo.PrintCounts(os);
        return os.str();
    });
    histo.def("print_centers", [](const Histogram &histo) {
        std::stringstream os;
        histo.PrintCenters(os);
        return os.str();
    });
    histo.def("print_centers_and_counts", [](const Histogram &histo) {
        std::stringstream os;
        histo.PrintCentersAndCounts(os);
        return os.str();
    });
    histo.def("print_breaks_and_counts", [](const Histogram &histo) {
        std::stringstream os;
        histo.PrintBreaksAndCounts(os);
        return os.str();
    });
    histo.def("__str__", [](const Histogram &histo) {
        std::stringstream os;
        histo.PrintBreaksAndCounts(os);
        os << "bins: " << histo.bins << std::endl;
        return os.str();
    });

    histo.def("fill_counts",
              [](Histogram &histo, const DataVector &data) -> void {
                  histo.FillCounts(data);
              });
    histo.def("set_count", &Histogram::SetCount);
    histo.def("reset_counts", &Histogram::ResetCounts);
    histo.def("index_from_value",
              [](const Histogram &histo, const double &value) {
                  return histo.IndexFromValue(value);
              });
    histo.def("compute_bin_centers", &Histogram::ComputeBinCenters);
    // Data public members
    histo.def_readwrite("range", &Histogram::range);
    histo.def_readwrite("breaks", &Histogram::breaks);
    histo.def_readwrite("bins", &Histogram::bins);
    histo.def_readwrite("counts", &Histogram::counts);
    histo.def_readwrite("name", &Histogram::name);
    histo.def("mean", [](const Histogram &histo) {
            return Mean(histo);
            });
#ifdef SG_USING_VTK
    histo.def("visualize", [](const Histogram &histo) -> void {
        visualize_histo<Histogram>(histo);
    });
#endif
}

} // end namespace histo
#endif
