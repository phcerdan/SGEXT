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
#include "sglocate_common.h"

#include "get_vtk_points_from_graph.hpp"

namespace py = pybind11;
using namespace SG;

void init_get_vtk_points_from_graph(py::module &m) {
    py::class_<vtkPoints, vtkSmartPointer<vtkPoints>>(m, "vtkPoints")
            .def("__str__",
                 [](vtkPoints &p) {
                     std::stringstream os;
                     p.Print(os);
                     return os.str();
                 })
            .def("get_number_of_points", &vtkPoints::GetNumberOfPoints);

    /* ************************************************************ */

    py::class_<vtkPointLocator, vtkSmartPointer<vtkPointLocator>>(
            m, "vtkPointLocator")

            .def("__str__",
                 [](vtkPointLocator &p) {
                     std::stringstream os;
                     p.Print(os);
                     return os.str();
                 })

            .def(
                    "find_closest_point",
                    [](vtkPointLocator &locator, const PointType &sgext_point) {
                        return locator.FindClosestPoint(sgext_point.data());
                    },
                    R"(
Given a position, returns the id of the point closest to it.
                )",
                    py::arg("position"))

            .def(
                    "find_closest_point_within_radius",
                    [](vtkPointLocator &locator, const double &radius,
                       const PointType &sgext_point) {
                        double dist2 = 0.0;
                        const auto vtk_id =
                                locator.FindClosestPointWithinRadius(
                                        radius, sgext_point.data(), dist2);
                        return std::make_pair(vtk_id, dist2);
                    },
                    R"(
Given a radius and a position, returns the id of the point
closest to the point in that radius, and the distance squared to the point.
                )",
                    py::arg("radius"), py::arg("position"));

    /* ************************************************************ */

    py::class_<vtkMergePoints, vtkSmartPointer<vtkMergePoints>,
               vtkPointLocator>(m, "vtkMergePoints")
            .def("__str__", [](vtkMergePoints &p) {
                std::stringstream os;
                p.Print(os);
                return os.str();
            });

    /* ************************************************************ */

    m.def("get_vtk_points_from_graph", &get_vtk_points_from_graph);

    m.def("get_vtk_points_from_graphs", &get_vtk_points_from_graphs,
          py::arg("graphs"), py::arg("bounding_box") = nullptr);
}
