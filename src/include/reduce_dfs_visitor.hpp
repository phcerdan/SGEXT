#ifndef REDUCE_DFS_VISITOR_HPP
#define REDUCE_DFS_VISITOR_HPP
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_iterator.hpp>
#include "array_utilities.hpp"
#include <iostream>
#include <tuple>

/**
 * From:
http://www.boost.org/doc/libs/1_65_1/libs/graph/doc/depth_first_search.html

vis.initialize_vertex(s, g) is invoked on every vertex of the graph before the start of the graph search.
vis.start_vertex(s, g) is invoked on the source vertex once before the start of the search.
vis.discover_vertex(u, g) is invoked when a vertex is encountered for the first time.
vis.examine_edge(e, g) is invoked on every out-edge of each vertex after it is discovered.
vis.tree_edge(e, g) is invoked on each edge as it becomes a member of the edges that form the search tree. If you wish to record predecessors, do so at this event point.
vis.back_edge(e, g) is invoked on the back edges in the graph.
vis.forward_or_cross_edge(e, g) is invoked on forward or cross edges in the graph. In an undirected graph this method is never called.
vis.finish_edge(e, g) is invoked on the non-tree edges in the graph as well as on each tree edge after its target vertex is finished.
vis.finish_vertex(u, g) is invoked on a vertex after all of its out edges have been added to the search tree and all of the adjacent vertices have been discovered (but before their out-edges have been
examined).
//  put(color, u, Color::white())
 */
template <typename SpatialGraph, typename VertexMap, typename ColorMap>
struct ReduceGraphVisitor : public boost::default_dfs_visitor {
    using SpatialGraphVertexBundle =
        typename boost::vertex_bundle_type<SpatialGraph>::type;
    using SpatialVertex = SpatialGraphVertexBundle;
    using SpatialGraphEdgeBundle =
        typename boost::edge_bundle_type<SpatialGraph>::type;
    using SpatialEdge = SpatialGraphEdgeBundle;
    using vertex_descriptor =
        typename boost::graph_traits<SpatialGraph>::vertex_descriptor;
    using edge_descriptor =
        typename boost::graph_traits<SpatialGraph>::edge_descriptor;

    ReduceGraphVisitor(SpatialGraph &sg, ColorMap &color_map, VertexMap &vertex_map)
        : m_sg(sg), m_color_map(color_map), m_vertex_map(vertex_map) {}


    SpatialGraph &m_sg;
    ColorMap &m_color_map;
    VertexMap &m_vertex_map;

  private:
    vertex_descriptor m_source;
    SpatialEdge m_sg_edge;
    bool m_already_started = false;

  public:
    // Commented out to avoid clutter of the output.
    void discover_vertex(vertex_descriptor u, const SpatialGraph &input_sg) { // check!
        // std::cout << "discover_vertex: " << u << " : " << ArrayUtilities::to_string(input_sg[u].pos) << std::endl;
        auto degree = boost::out_degree(u, input_sg);
        if (degree != 2) // starting or ending point.
        {
            // Check if current node have an spatial node already created.
            vertex_descriptor sg_vertex_descriptor;
            bool sg_vertex_exists = false;
            auto vertex_search = m_vertex_map.find(u);
            if(vertex_search != m_vertex_map.end())
            {
                sg_vertex_exists = true;
                sg_vertex_descriptor = vertex_search->second;
            }

            if (!m_already_started) { // Starting node
                m_already_started = true;
                if(!sg_vertex_exists){
                    sg_vertex_descriptor = boost::add_vertex(input_sg[u], m_sg);
                }
                // Store the start point to add an edge at the end.
                m_source = sg_vertex_descriptor;
            } else { // Ending node
                if(!sg_vertex_exists){
                    sg_vertex_descriptor = boost::add_vertex(input_sg[u], m_sg);
                }
                // We remove the last edge_point from the spatial
                // edge (already added in the vertex)
                bool contain_edge_points = (m_sg_edge.edge_points.size() > 0)? true : false;
                if(contain_edge_points)
                  m_sg_edge.edge_points.pop_back();
                // How to check if edge exists?
                auto edge_exist = boost::edge(m_source, sg_vertex_descriptor, m_sg);
                if(!edge_exist.second)
                    boost::add_edge(m_source, sg_vertex_descriptor, m_sg_edge, m_sg);
                else
                {
                  auto & existing_edge_points = m_sg[edge_exist.first].edge_points;
                  if ( existing_edge_points.size() != m_sg_edge.edge_points.size() )
                    boost::add_edge(m_source, sg_vertex_descriptor, m_sg_edge, m_sg);
                  // else // Fine comparisson the points, copy array + sort TODO
                }
            }

            if(!sg_vertex_exists) {
                m_vertex_map[u] = sg_vertex_descriptor;
            }
            // Visiting should end after this function.
        }
    }

    void tree_edge(edge_descriptor e, const SpatialGraph &input_sg) {
        auto target = boost::target(e, input_sg);
        // std::cout << "tree_edge: " << e << " , target: " << target << " : " << ArrayUtilities::to_string(input_sg[target].pos) << std::endl;
        m_sg_edge.edge_points.push_back(input_sg[target].pos);
    }

    void finish_vertex(vertex_descriptor u, const SpatialGraph &input_sg) {
        typedef typename boost::color_traits<typename ColorMap::mapped_type> Color;
        // Restore white for vertices with more than 2 degrees.
        // For tree_edge to work on them.
        if (boost::out_degree(u, input_sg) > 2)
            m_color_map[u] = Color::white();
    }

};

template <class SpatialGraph>
SpatialGraph reduce_spatial_graph_via_dfs(const SpatialGraph &input_sg) {
    SpatialGraph sg;
    using vertex_descriptor = typename boost::graph_traits<
        SpatialGraph>::vertex_descriptor;
    using vertex_iterator =
        typename boost::graph_traits<SpatialGraph>::vertex_iterator;

    using ColorMap = std::map<vertex_descriptor, boost::default_color_type>;
    ColorMap colorMap;
    typedef typename boost::color_traits<typename ColorMap::mapped_type> Color;
    boost::associative_property_map<ColorMap> propColorMap(colorMap);

    // std::cout << "ReduceGraphVistor:" << std::endl;
    using VertexMap = std::unordered_map<vertex_descriptor, vertex_descriptor>;
    VertexMap vertex_map;
    ReduceGraphVisitor<SpatialGraph, VertexMap, ColorMap> vis(sg, colorMap, vertex_map);

    // for each end vertex:
    vertex_iterator vi, vi_end;
    std::tie(vi, vi_end) = boost::vertices(input_sg);
    auto start = *vi;
    // Terminate function.
    // Stop the visit when find a non-chain (degree= 2) vertex.
    auto finish_on_junctions = [&start, &propColorMap](vertex_descriptor u,
                                        const SpatialGraph &g) {
        // Do not terminate at the start.
        if (u == start)
            return false;

        if (boost::out_degree(u, g) != 2) {
            // std::cout << "Exited" << std::endl;
            put(propColorMap, u, Color::white());
            return true;
        } else
            return false; // Do not terminate
    };

    vertex_iterator ui, ui_end;
    for (std::tie(ui, ui_end) = boost::vertices(input_sg); ui != ui_end; ++ui) {
      put(propColorMap, *ui, Color::white());
    }

    for (; vi != vi_end; ++vi) {
        auto degree = boost::out_degree(*vi, input_sg);
        if ( degree != 2) {
            start = *vi;
            // std::cout << "Visit: start: " << start << ". Degree: " << degree << std::endl;
            boost::depth_first_visit(input_sg, start, vis, propColorMap,
                                     finish_on_junctions);
        }
    }

    return sg;
}

/**
 * Convert (copy) object to spatial graph.
 *
 * Object has no stored edges, use its topology to list connected edges.
 * So we cannot remove edges without modifying the topology.
 *
 * The vertex_descriptor of object (the position) is stores in the nodes of
 * spatial graph: a_sg_node.pos
 * The sg_edges are not populated (there are no edge_points).
 *
 * @tparam ObjectGraph Object from DGtal
 * @tparam SpatialGraph
 * @param obj
 *
 * @return SpatialGraph which is a copy of object.
 */
template <class ObjectGraph, class SpatialGraph>
SpatialGraph spatial_graph_from_object(const ObjectGraph & obj)
{
    SpatialGraph sg(boost::num_vertices(obj));
    using obj_vertex_iterator =
        typename boost::graph_traits<ObjectGraph>::vertex_iterator;
    using obj_vertex_descriptor =
        typename boost::graph_traits<ObjectGraph>::vertex_descriptor;
    using sg_vertex_iterator =
        typename boost::graph_traits<SpatialGraph>::vertex_iterator;
    using sg_vertex_descriptor =
        typename boost::graph_traits<SpatialGraph>::vertex_descriptor;
    obj_vertex_iterator ovi, ovi_end, ovi_start;
    std::tie(ovi, ovi_end) = boost::vertices(obj);
    ovi_start = ovi;
    sg_vertex_iterator svi, svi_end, svi_start;
    std::tie(svi, svi_end) = boost::vertices(sg);
    svi_start = svi;
    std::unordered_map<obj_vertex_descriptor, sg_vertex_descriptor> dmap;
    for (; ovi != ovi_end; ++ovi, ++svi) {
        // Copy node position
        for (size_t i = 0; i < ObjectGraph::Point::dimension; ++i)
            sg[*svi].pos[i] = (*ovi)[i];
        // Add to map
        dmap[*ovi] = *svi;
    }
    ovi = ovi_start;
    svi = svi_start;
    for (; ovi != ovi_end; ++ovi, ++svi) {
        // Add edges before checking they don't already exist (undirected graph)
        auto adjacents = boost::adjacent_vertices(*ovi, obj);
        for (; adjacents.first != adjacents.second; ++adjacents.first) {
            auto & s_adjacent = dmap[*adjacents.first];
            auto edge_exist = boost::edge(*svi, s_adjacent, sg);
            if(!edge_exist.second)
                boost::add_edge(*svi, dmap[*adjacents.first], sg);
        }
    }
    return sg;
}

template <class SpatialGraph>
void remove_extra_edges(SpatialGraph & sg) {
    using vertex_descriptor = typename boost::graph_traits<
        SpatialGraph>::vertex_descriptor;
    using vertex_iterator =
        typename boost::graph_traits<SpatialGraph>::vertex_iterator;
    using adjacency_iterator =
        typename boost::graph_traits<SpatialGraph>::adjacency_iterator;
    vertex_iterator vi, vi_end;
    std::tie(vi, vi_end) = boost::vertices(sg);
    for (; vi != vi_end; ++vi) {
        auto degree = boost::out_degree(*vi, sg);
        if (degree > 2){
            adjacency_iterator neighbor_it, neighbor_end_it;
            std::tie(neighbor_it, neighbor_end_it) = boost::adjacent_vertices(*vi, sg);
            // Compare distance between current node and neighbors
            // only if the neighbors are connected between them.
            // Remove the largest edge.
            // o
            // |\
            // oo
            std::vector<std::pair<vertex_descriptor, vertex_descriptor>> neighbors_connected;
            for(; neighbor_it != neighbor_end_it; ++neighbor_it) {
                auto other_neighbor_it = neighbor_it;
                ++other_neighbor_it;
                if (other_neighbor_it != neighbor_end_it){
                    for(; other_neighbor_it != neighbor_end_it; ++other_neighbor_it) {
                        auto edge_exist_pair = boost::edge(*neighbor_it, *other_neighbor_it, sg);
                        // Check edge between neighbors exist.
                        if(edge_exist_pair.second){
                            // Store pair of vertex_descriptor of connected neighbors
                            // Because removing edges causes invalidation of adjacency_iterator
                            neighbors_connected.emplace_back(*neighbor_it, *other_neighbor_it);
                        }
                    }
                }
            }

            // Compare positions of current and neighbor nodes,
            // and remove largest edge.
            for(auto & nv : neighbors_connected){
                auto & sn_first = sg[nv.first];
                auto & sn_second = sg[nv.second];
                auto & sn_current = sg[*vi];
                auto dist_first = ArrayUtilities::distance(sn_current.pos, sn_first.pos);
                auto dist_second = ArrayUtilities::distance(sn_current.pos, sn_second.pos);
                if(dist_first > dist_second){
                    boost::remove_edge(nv.first, *vi, sg);
                } else {
                    boost::remove_edge(nv.second, *vi, sg);
                }
            }
        }
    }
}
#endif
