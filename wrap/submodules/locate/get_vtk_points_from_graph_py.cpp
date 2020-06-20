/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"
#include "sglocate_common.h" // define holder for vtkSmartPointer

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
