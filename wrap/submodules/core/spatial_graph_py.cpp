/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include "spatial_graph.hpp"
#include "spatial_graph_utilities.hpp"

namespace py = pybind11;
using namespace SG;

void init_spatial_graph(py::module &m) {
    auto mgraph = m.def_submodule("graph");
    // py::class_<GraphType::vertex_descriptor>(mgraph, "vertex_descriptor")
    //         .def(py::init());
    // TODO probably missing something wrappable in vertex_descriptor
    py::class_<GraphType::edge_descriptor>(mgraph, "edge_descriptor")
            .def(py::init())
            .def(py::init([](GraphType::vertex_descriptor s,
                             GraphType::vertex_descriptor t) {
                return std::unique_ptr<GraphType::edge_descriptor>(
                        new GraphType::edge_descriptor(s, t, 0));
            }))
            // No access to pointer m_property from python for now
            // The pointer in the constructor is confusing,
            // use a lambda and ignore SpatialEdge for descriptors...
            // probably not used anyway
            // .def(py::init<size_t, size_t, SpatialEdge*>())
            .def_readwrite("source", &GraphType::edge_descriptor::m_source)
            .def_readwrite("target", &GraphType::edge_descriptor::m_target)
            .def("__eq__",
                 [](const GraphType::edge_descriptor &self,
                    const GraphType::edge_descriptor &other) {
                     return (self.m_source == other.m_source) &&
                            (self.m_target == other.m_target);
                 })
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
                     return "spatial_graph:\n num_vertices: " +
                            std::to_string(boost::num_vertices(graph)) +
                            "\n num_edges:  " +
                            std::to_string(boost::num_edges(graph));
                 })
            .def("__str__",
                 [](const GraphType &graph) {
                     std::stringstream os;
                     print_degrees(graph, os);
                     print_spatial_edges(graph, os);
                     return "spatial_graph:\n" + os.str();
                 })
            .def("spatial_node",
                 [](const GraphType &graph, const size_t &n) -> SpatialNode {
                     return graph[n];
                 })
            .def("spatial_edge",
                 [](const GraphType &graph,
                    const GraphType::edge_descriptor &ed) -> SpatialEdge {
                     return graph[ed];
                 })
            .def("edge",
                 [](const GraphType &graph,
                    const GraphType::vertex_descriptor s,
                    const GraphType::vertex_descriptor t) ->
                 std::pair<GraphType::edge_descriptor, bool> {
                     return boost::edge(s, t, graph);
                 })
            .def("set_vertex",
                 [](GraphType &graph, const size_t &n,
                    const SpatialNode &sn) -> void { graph[n] = sn; })
            .def("set_edge",
                 [](GraphType &graph, const GraphType::edge_descriptor &ed,
                    const SpatialEdge &se) -> void { graph[ed] = se; })
            .def("num_vertices",
                 [](const GraphType &graph) -> size_t {
                     return boost::num_vertices(graph);
                 })
            .def("num_edges",
                 [](const GraphType &graph) -> size_t {
                     return boost::num_edges(graph);
                 })
            .def("num_edge_points",
                 [](const GraphType &graph) -> size_t {
                     return SG::num_edge_points(graph);
                 })
            .def("source",
                 [](const GraphType &graph,
                    const GraphType::edge_descriptor &u) {
                     return boost::source(u, graph);
                 })
            .def("target",
                 [](const GraphType &graph,
                    const GraphType::edge_descriptor &u) {
                     return boost::target(u, graph);
                 })
            .def("out_edges",
                 [](const GraphType &graph, size_t u) {
                     std::vector<GraphType::edge_descriptor> edge_descriptors;
                     for (auto [ei, ei_end] = out_edges(u, graph); ei != ei_end;
                          ++ei) {
                         edge_descriptors.push_back(*ei);
                     }
                     return edge_descriptors;
                 })
            .def("add_edge",
                 [](GraphType &graph, size_t u, size_t v,
                    const SpatialEdge &se) {
                     return boost::add_edge(u, v, se, graph);
                 })
            .def("vertices",
                 [](GraphType &graph) {
                     std::vector<size_t> vertices_descriptors;
                     for (auto [vi, vi_end] = boost::vertices(graph);
                          vi != vi_end; vi++) {
                         vertices_descriptors.push_back(*vi);
                     }
                     return vertices_descriptors;
                 })
            .def("edges", [](GraphType &graph) {
                std::vector<GraphType::edge_descriptor> edges_descriptors;
                for (auto [ei, ei_end] = boost::edges(graph); ei != ei_end;
                     ei++) {
                    edges_descriptors.push_back(*ei);
                }
                return edges_descriptors;
            });

    // return type: Not wrapped
    // .def_readwrite("edges", &GraphType::m_edges);

    mgraph.def("num_edge_points", &num_edge_points<GraphType>);
    mgraph.def("add_edge",
               [](size_t u, size_t v, const SpatialEdge &se, GraphType &graph) {
                   return boost::add_edge(u, v, se, graph);
               });
    mgraph.def("source",
               [](const GraphType::edge_descriptor &u, const GraphType &graph) {
                   return boost::source(u, graph);
               });
    mgraph.def("target",
               [](const GraphType::edge_descriptor &u, const GraphType &graph) {
                   return boost::target(u, graph);
               });
    mgraph.def("vertices", [](GraphType &graph) {
        std::vector<size_t> vertices_descriptors;
        for (auto [vi, vi_end] = boost::vertices(graph); vi != vi_end; vi++) {
            vertices_descriptors.push_back(*vi);
        }
        return vertices_descriptors;
    });
    mgraph.def("edges", [](GraphType &graph) {
        std::vector<GraphType::edge_descriptor> edges_descriptors;
        for (auto [ei, ei_end] = boost::edges(graph); ei != ei_end; ei++) {
            edges_descriptors.push_back(*ei);
        }
        return edges_descriptors;
    });
    mgraph.def("out_edges", [](size_t u, const GraphType &graph) {
        std::vector<GraphType::edge_descriptor> edge_descriptors;
        for (auto [ei, ei_end] = out_edges(u, graph); ei != ei_end; ++ei) {
            edge_descriptors.push_back(*ei);
        }
        return edge_descriptors;
    });
    mgraph.def("get_all_points", &get_all_points);
}
