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

#include "sgcore_common_py.hpp"
#include "spatial_graph.hpp"
#include "spatial_graph_utilities.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

namespace py = pybind11;
using namespace SG;

void init_spatial_graph(py::module &m) {
    auto mgraph = m.def_submodule("graph");
    py::class_<GraphType::vertex_descriptor>(mgraph, "vertex_descriptor")
            .def(py::init());
    // TODO probably missing something wrappable in vertex_descriptor
    py::class_<GraphType::edge_descriptor>(mgraph, "edge_descriptor")
            .def(py::init())
            // No access to pointer m_property from python for now
            // The pointer in the constructor is confusing,
            // use a lambda and ignore SpatialEdge for descriptors...
            // probably not used anyway
            // .def(py::init<size_t, size_t, SpatialEdge*>())
            .def_readwrite("source", &GraphType::edge_descriptor::m_source)
            .def_readwrite("target", &GraphType::edge_descriptor::m_target)
            .def("__repr__", [](const GraphType::edge_descriptor &ed) {
                return "<edge_descriptor: source: " +
                       std::to_string(ed.m_source) +
                       "; target: " + std::to_string(ed.m_target) + " >";
            });

    py::class_<graph_descriptor>(mgraph, "graph_descriptor")
            .def_readwrite("exist", &graph_descriptor::exist)
            .def_readwrite("is_edge", &graph_descriptor::is_edge)
            .def_readwrite("is_vertex", &graph_descriptor::is_vertex)
            .def_readwrite("vertex_d", &graph_descriptor::vertex_d)
            .def_readwrite("edge_d", &graph_descriptor::edge_d)
            .def_readwrite("edge_points_index",
                           &graph_descriptor::edge_points_index)
            .def("__repr__", [](const graph_descriptor &gd) {
                std::stringstream os;
                print_graph_descriptor(gd, "graph_descriptor", os);
                return os.str();
            });
    py::class_<GraphType>(m, "spatial_graph")
            .def(py::init())
            .def(py::init<size_t>())
            .def("__repr__",
                 [](const GraphType &graph) {
                     std::stringstream os;
                     print_degrees(graph, os);
                     print_spatial_edges(graph, os);
                     return "spatial_graph:\n" + os.str();
                 })
            .def("vertex",
                 [](const GraphType &graph, const size_t &n) -> SpatialNode {
                     return graph[n];
                 })
            .def("edge",
                 [](const GraphType &graph,
                    const GraphType::edge_descriptor &ed) -> SpatialEdge {
                     return graph[ed];
                 })
            .def("set_vertex",
                 [](GraphType &graph, const size_t &n,
                    const SpatialNode &sn) -> void { graph[n] = sn; })
            .def("set_edge",
                 [](GraphType &graph, const GraphType::edge_descriptor &ed,
                    const SpatialEdge &se) -> void { graph[ed] = se; })
            .def("num_vertices", [](const GraphType &graph) -> size_t {
                return boost::num_vertices(graph);
            });

    // return type: Not wrapped
    // .def_readwrite("vertices", &GraphType::m_vertices)
    // .def_readwrite("edges", &GraphType::m_edges);

    mgraph.def("num_edge_points", &num_edge_points<GraphType>);
    mgraph.def("add_edge",
               [](size_t u, size_t v, const SpatialEdge &se, GraphType &graph) {
                   return boost::add_edge(u, v, se, graph);
               });
    mgraph.def("get_all_points", &get_all_points);
}
