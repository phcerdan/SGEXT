#ifndef SPATIAL_GRAPH_UTILITIES_HPP
#define SPATIAL_GRAPH_UTILITIES_HPP

#include "spatial_graph.hpp"
#include "spatial_node.hpp"

namespace SG {

void print_pos(std::ostream &out, const SG::SpatialNode::PointType &pos) {
    out << "{";
    for (auto &p : pos) {
        if (p >= 0)
            out << " ";
        out << p << ",";
    }
    out << "}";
}

template<typename SpatialGraph>
void print_degrees(const SpatialGraph &graph) {
    std::cout << "Print degrees spatial_graph:" << std::endl;
    std::cout << "Num Vertices: " << boost::num_vertices(graph) << std::endl;
    using vertex_iterator =
        typename boost::graph_traits<SpatialGraph>::vertex_iterator;
    vertex_iterator vi, vi_end;
    std::tie(vi, vi_end) = boost::vertices(graph);
    for (; vi != vi_end; ++vi) {
        std::cout << *vi << ": " << ArrayUtilities::to_string(graph[*vi].pos) <<
            ". Degree: " << boost::out_degree(*vi, graph) << std::endl;
    }
}

template<typename SpatialGraph>
void print_edges(const SpatialGraph &graph) {
    std::cout << "Print edges spatial_graph:" << std::endl;
    std::cout << "Num Edges: " << boost::num_edges(graph) << std::endl;
    using edge_iterator =
        typename boost::graph_traits<SpatialGraph>::edge_iterator;
    edge_iterator ei, ei_end;
    std::tie(ei, ei_end) = boost::edges(graph);
    for (; ei != ei_end; ++ei) {
        auto source = boost::source(*ei, graph);
        auto target = boost::target(*ei, graph);
        print_pos(std::cout, graph[source].pos);
        std::cout << "---";
        print_pos(std::cout, graph[target].pos);
        std::cout << std::endl;
    }
}

template<typename SpatialGraph>
void print_spatial_edges(const SpatialGraph &graph) {
    std::cout << "Print edges spatial_graph:" << std::endl;
    std::cout << "Num Edges: " << boost::num_edges(graph) << std::endl;
    using edge_iterator =
        typename boost::graph_traits<SpatialGraph>::edge_iterator;
    edge_iterator ei, ei_end;
    std::tie(ei, ei_end) = boost::edges(graph);
    for (; ei != ei_end; ++ei) {
        auto source = boost::source(*ei, graph);
        auto target = boost::target(*ei, graph);
        print_pos(std::cout, graph[source].pos);
        std::cout << "---";
        print_pos(std::cout, graph[target].pos);
        std::cout << std::endl;
        std::cout << "edge_points: " << graph[*ei].edge_points.size() << std::endl;
        std::cout << graph[*ei] << std::endl;
    }
}
} //end namespace
#endif
