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
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include "hash_edge_descriptor.hpp"
#include "spatial_graph.hpp"
#include "spatial_graph_utilities.hpp"
// For pickle
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/graph/adj_list_serialize.hpp>

namespace py = pybind11;
using namespace SG;

void init_spatial_graph(py::module &m) {
    auto mgraph = m.def_submodule("graph");
    mgraph.doc() = "Graph submodule with graph related function and classes";
    // py::class_<GraphType::vertex_descriptor>(mgraph, "vertex_descriptor")
    //         .def(py::init());
    // TODO probably missing something wrappable in vertex_descriptor
    py::class_<GraphType::edge_descriptor>(mgraph, "edge_descriptor")
            .def(py::init(), R"(Default constructor)")
            .def(py::init([](GraphType::vertex_descriptor s,
                             GraphType::vertex_descriptor t) {
                return std::make_unique<GraphType::edge_descriptor>(s, t,
                                                                    nullptr);
            }), py::arg("source"), py::arg("target"),
                R"(Constructor from source and target vertex descriptors)")
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
            .def("__repr__",
                 [](const GraphType::edge_descriptor &ed) {
                     return "<edge_descriptor: source: " +
                            std::to_string(ed.m_source) +
                            "; target: " + std::to_string(ed.m_target) + " >";
                 })
            // Allow using my own edge_hash for dictionaries
            .def("__hash__", [](const GraphType::edge_descriptor &self) {
                return edge_hash<GraphType>()(self);
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
            .def(py::init(), R"(Default constructor, empty graph)")
            .def(py::init<size_t>(),py::arg("num_vertices"),
                    R"(Constructor with number of vertices of the graph)")
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
            // For pickle support (copy.deepcopy) define getstate and setstate
            .def(py::pickle(
                // getstate: equivalent to write graph to file
                [](const GraphType & graph) -> std::string {
                std::ostringstream os;
                boost::archive::text_oarchive arch(os);
                arch << graph;
                return os.str();
                },
                // setstate: equivalent to read graph from file
                [](const std::string & serialized_str) -> GraphType {
                std::istringstream os(serialized_str);
                boost::archive::text_iarchive arch(os);
                GraphType graph;
                arch >> graph;
                return graph;
                }))
            .def("spatial_node",
                 [](GraphType &graph, const size_t &n) -> SpatialNode& {
                     return graph[n];
                 }, py::return_value_policy::reference,
                 R"(Return the spatial_node associated to vertex n.)")
            .def("spatial_edge",
                 [](GraphType &graph,
                    const GraphType::edge_descriptor &ed) -> SpatialEdge& {
                     return graph[ed];
                 }, py::return_value_policy::reference,
                 R"(Return spatial edge from edge descriptor)")
            .def("edge",
                 [](const GraphType &graph,
                    const GraphType::vertex_descriptor s,
                    const GraphType::vertex_descriptor t)
                         -> std::pair<GraphType::edge_descriptor, bool> {
                     return boost::edge(s, t, graph);
                 }, py::arg("source"), py::arg("target"),
R"(
Parameters
----------
source: vertex_descriptor
    source vertex
target: vertex_descriptor
    target vertex

Returns
-------
edge_descriptor
    edge connecting source and target vertices
bool
    true if edge exists in graph, false otherwise
)")
            .def("set_vertex",
                 [](GraphType &graph, const size_t &n,
                    const SpatialNode &sn) -> void { graph[n] = sn; },
                 py::arg("vertex"), py::arg("mesh_graph_node"),
                 R"(Set the mesh_graph_node of input vertex.)")
            .def("set_edge",
                 [](GraphType &graph, const GraphType::edge_descriptor &ed,
                    const SpatialEdge &se) -> void { graph[ed] = se; },
                 py::arg("edge"), py::arg("spatial_edge"),
                 R"(Set the spatial edge of input edge.)")
            .def("num_vertices",
                 [](const GraphType &graph) -> size_t {
                     return boost::num_vertices(graph);
                 }, R"(Returns number of vertices of the graph.)")
            .def("num_edges",
                 [](const GraphType &graph) -> size_t {
                     return boost::num_edges(graph);
                 }, R"(Returns number of edges of the graph.)")
            .def("num_edge_points",
                 [](const GraphType &graph) -> size_t {
                     return SG::num_edge_points(graph);
                 }, R"(Returns the sum of edge points from all edges.)")
            .def("source",
                 [](const GraphType &graph,
                    const GraphType::edge_descriptor &u) {
                     return boost::source(u, graph);
                 }, py::arg("edge"), R"(Returns source vertex of input edge.)")
            .def("target",
                 [](const GraphType &graph,
                    const GraphType::edge_descriptor &u) {
                     return boost::target(u, graph);
                 }, py::arg("edge"), R"(Returns target vertex of input edge.)")
            .def("out_edges",
                 [](const GraphType &graph, size_t u) {
                     std::vector<GraphType::edge_descriptor> edge_descriptors;
                     for (auto [ei, ei_end] = out_edges(u, graph); ei != ei_end;
                          ++ei) {
                         edge_descriptors.push_back(*ei);
                     }
                     return edge_descriptors;
                 }, py::arg("vertex"),
                 R"(Returns tuple with all the out edges of input vertex.)")
            .def("adjacent_vertices",
                 [](const GraphType &graph, size_t u) {
                 std::vector<GraphType::vertex_descriptor> vertex_descriptors;
                 for (auto [vi, vi_end] = boost::adjacent_vertices(u, graph);
                         vi != vi_end; ++vi) {
                     vertex_descriptors.push_back(*vi);
                 }
                 return vertex_descriptors;
                 }, py::arg("vertex"),
                 R"(Return tuple with neighbors of input vertex)")
            .def("add_edge",
                 [](GraphType &graph, size_t u, size_t v,
                    const SpatialEdge &se) {
                     return boost::add_edge(u, v, se, graph);
                 }, py::arg("source"), py::arg("target"), py::arg("spatial_edge"),
        R"(Add edge from source to target vertices with the input spatial edge.)")
            .def("vertices",
                 [](GraphType &graph) {
                     std::vector<size_t> vertices_descriptors;
                     for (auto [vi, vi_end] = boost::vertices(graph);
                          vi != vi_end; vi++) {
                         vertices_descriptors.push_back(*vi);
                     }
                     return vertices_descriptors;
                 }, R"(Returns a tuple with all vertices of the graph.)")
            .def("edges", [](GraphType &graph) {
                std::vector<GraphType::edge_descriptor> edges_descriptors;
                for (auto [ei, ei_end] = boost::edges(graph); ei != ei_end;
                     ei++) {
                    edges_descriptors.push_back(*ei);
                }
                return edges_descriptors;
              }, R"(Returns a tuple with all edges of the graph.)"
            )
            .def("add_vertex",
                 [](GraphType &graph, const SpatialNode &sn) {
                    return boost::add_vertex(sn, graph);
                 }, py::arg("spatial_node") = SpatialNode(),
                 R"(Add a vertex to the graph. A SpatialNode can be optionally provided)");

    // return type: Not wrapped
    // .def_readwrite("edges", &GraphType::m_edges);

    mgraph.def("num_edge_points", &num_edge_points<GraphType>,
            R"(Returns the sum of edge points from all edges.)");
    mgraph.def("add_edge",
               [](size_t u, size_t v, const SpatialEdge &se, GraphType &graph) {
                   return boost::add_edge(u, v, se, graph);
               }, py::arg("source"), py::arg("target"),
                  py::arg("spatial_edge"), py::arg("graph"),
        R"(Add edge from source to target vertices with the input spatial edge.)"
               );
    mgraph.def("add_vertex",
                 [](const SpatialNode &sn, GraphType &graph) {
                    return boost::add_vertex(sn, graph);
                 }, py::arg("spatial_node") = SpatialNode(), py::arg("graph"),
                 R"(Add a vertex to the graph. A SpatialNode can be optionally provided)"
                 );
    mgraph.def("source",
               [](const GraphType::edge_descriptor &u, const GraphType &graph) {
                   return boost::source(u, graph);
               }, py::arg("edge"), py::arg("graph"),
               R"(Return source vertex of input edge of given graph.)"
               );
    mgraph.def("target",
               [](const GraphType::edge_descriptor &u, const GraphType &graph) {
                   return boost::target(u, graph);
               }, py::arg("edge"), py::arg("graph"),
               R"(Return source vertex of input edge of given graph.)"
               );
    mgraph.def("vertices", [](GraphType &graph) {
        std::vector<size_t> vertices_descriptors;
        for (auto [vi, vi_end] = boost::vertices(graph); vi != vi_end; vi++) {
            vertices_descriptors.push_back(*vi);
        }
        return vertices_descriptors;
    }, py::arg("graph"), R"(Returns a tuple with all the vertices of the graph.)"
    );
    mgraph.def("edges", [](GraphType &graph) {
        std::vector<GraphType::edge_descriptor> edges_descriptors;
        for (auto [ei, ei_end] = boost::edges(graph); ei != ei_end; ei++) {
            edges_descriptors.push_back(*ei);
        }
        return edges_descriptors;
    }, py::arg("graph"), R"(Returns a tuple with all edges of the graph.)"
    );
    mgraph.def("out_edges", [](size_t u, const GraphType &graph) {
        std::vector<GraphType::edge_descriptor> edge_descriptors;
        for (auto [ei, ei_end] = out_edges(u, graph); ei != ei_end; ++ei) {
            edge_descriptors.push_back(*ei);
        }
        return edge_descriptors;
    }, py::arg("vertex"), py::arg("graph"),
    R"(Return tuple with output edges of input vertex)"
    );
    mgraph.def("adjacent_vertices", [](size_t u, const GraphType &graph) {
            std::vector<GraphType::vertex_descriptor> vertex_descriptors;
            for (auto [vi, vi_end] = boost::adjacent_vertices(u, graph);
                    vi != vi_end; ++vi) {
            vertex_descriptors.push_back(*vi);
            }
            return vertex_descriptors;
        }, py::arg("vertex"), py::arg("graph"),
        R"(Return tuple with neighbors of input vertex.)"
    );
    mgraph.def("get_all_points", &get_all_points,
R"(
Returns
-------
[Array3D]
    Tuple with all the spatial points of the graph.
    Includes position of vertices and position of edge points.
[graph_descriptor]
    Tuple with all the graph_descriptor associated to the spatial points.
    Associating each spatial point from the first tuple with the graph.
    It categorizes the point into a vertex, or an edge.
)"
    );

}
