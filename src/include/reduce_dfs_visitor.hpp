#ifndef REDUCE_DFS_VISITOR_HPP
#define REDUCE_DFS_VISITOR_HPP
#include "array_utilities.hpp"
#include <algorithm>
#include <boost/graph/adjacency_iterator.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
// Modified by me boost/graph/depth_first_search.hpp
// #include <depth_first_search_with_exit.hpp>
#include <boost/graph/graph_traits.hpp>
#include <iostream>
#include <tuple>

namespace SG {

/**
 *
 * Use DFS (Depth first search/visitor) to remove all nodes with degree 2 and
add them as edge_points of a spatial_edge.
 *
 * This visitor constructs a new spatial_graph with the result.
 *
 * Extra from:
http://www.boost.org/doc/libs/1_65_1/libs/graph/doc/depth_first_search.html

vis.initialize_vertex(s, g) is invoked on every vertex of the graph before the
start of the graph search.

vis.start_vertex(s, g) is invoked on the source vertex once before the start of
the search.

vis.discover_vertex(u, g) is invoked when a vertex is encountered for the first
time.

vis.examine_edge(e, g) is invoked on every out-edge of each vertex after it is
discovered.

vis.tree_edge(e, g) is invoked on each edge as it becomes a member of the edges
that form the search tree. If you wish to record predecessors, do so at this
event point.

vis.back_edge(e, g) is invoked on the back edges in the graph.

vis.forward_or_cross_edge(e, g) is invoked on forward or cross edges in the
graph. In an undirected graph this method is never called.

vis.finish_edge(e, g) is invoked on the non-tree edges in the graph as well as
on each tree edge after its target vertex is finished.

vis.finish_vertex(u, g) is invoked on a vertex after all of its out edges have
been added to the search tree and all of the adjacent vertices have been
discovered (but before their out-edges have been examined).

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

    ReduceGraphVisitor(SpatialGraph &sg, ColorMap &color_map,
                       VertexMap &vertex_map, bool &is_not_loop)
        : m_sg(sg), m_color_map(color_map), m_vertex_map(vertex_map),
          m_is_not_loop(is_not_loop) {}

    /**
     * Copy Constructor.
     * DFS visit and search takes a copy of the visitor.
     * Set the boolean reference to false at copy (needed to stop function).
     */
    ReduceGraphVisitor(const ReduceGraphVisitor &other)
        : m_sg(other.m_sg),
          m_color_map(other.m_color_map), m_vertex_map(other.m_vertex_map),
          m_is_not_loop(other.m_is_not_loop) {
        m_is_not_loop = false;
    }

    SpatialGraph &m_sg;
    ColorMap &m_color_map;
    VertexMap &m_vertex_map;
    bool &m_is_not_loop;

  protected:
    SpatialEdge m_sg_edge;
    static const vertex_descriptor max_vertex_id = std::numeric_limits<vertex_descriptor>::max();
    vertex_descriptor m_sg_source = max_vertex_id;
    vertex_descriptor m_source = max_vertex_id;
    bool m_already_started = false;

  public:
    // Commented out to avoid clutter of the output.
    void discover_vertex(vertex_descriptor u,
                         const SpatialGraph &input_sg) { // check!
        auto degree = boost::out_degree(u, input_sg);
        // std::cout << "discover_vertex: " << u << " : "
        //           << ArrayUtilities::to_string(input_sg[u].pos)
        //           << ". Degree:" << degree << std::endl;
        if (degree != 2) // starting or ending point.
        {
            // Check if current node have an spatial node already created.
            vertex_descriptor sg_vertex_descriptor;
            bool sg_vertex_exists = false;
            auto vertex_search = m_vertex_map.find(u);
            if (vertex_search != m_vertex_map.end()) {
                sg_vertex_exists = true;
                sg_vertex_descriptor = vertex_search->second;
            }

            if (!m_already_started) { // Starting node
                m_already_started = true;
                if (!sg_vertex_exists) {
                    sg_vertex_descriptor = boost::add_vertex(input_sg[u], m_sg);
                }
                // Store the start point to add an edge at the end.
                // std::cout << "m_sg_source: old: " << m_sg_source << std::endl;
                m_sg_source = sg_vertex_descriptor;
                m_source = u;
                // std::cout << "m_sg_source: changed to: " << m_sg_source << std::endl;
            } else { // Ending node
                // Add node if it doesn't exist.
                if (!sg_vertex_exists) {
                    sg_vertex_descriptor = boost::add_vertex(input_sg[u], m_sg);
                }
                // Remove the last edge_point from the spatial
                // edge (already added in the vertex)
                bool contain_edge_points =
                    (m_sg_edge.edge_points.size() > 0) ? true : false;
                if (contain_edge_points)
                    m_sg_edge.edge_points.pop_back();

                // How to check if edge exists?
                auto edge_exist =
                    boost::edge(m_sg_source, sg_vertex_descriptor, m_sg);
                if (!edge_exist.second)
                    boost::add_edge(m_sg_source, sg_vertex_descriptor, m_sg_edge,
                                    m_sg);
                else {
                    auto &existing_edge_points =
                        m_sg[edge_exist.first].edge_points;
                    if (existing_edge_points.size() !=
                        m_sg_edge.edge_points.size())
                        boost::add_edge(m_sg_source, sg_vertex_descriptor,
                                        m_sg_edge, m_sg);
                    // else // Fine comparisson the points, copy array + sort
                    // TODO
                }
            }

            if (!sg_vertex_exists) {
                m_vertex_map[u] = sg_vertex_descriptor;
            }
            // Visiting should end after this function.
        }
    }

    void tree_edge(edge_descriptor e, const SpatialGraph &input_sg) {
        auto target = boost::target(e, input_sg);

        // std::cout << "tree_edge: " << e << " , target: " << target << " : "
        //           << ArrayUtilities::to_string(input_sg[target].pos)
        //           << std::endl;
        m_sg_edge.edge_points.push_back(input_sg[target].pos);
    }
    // Used only for loops. The ending vertex of the loop doesn't use tree_edge.
    // Sadly this gets called even after hitting finish_on_junction, because the 
    // recurisve nature of dfs_visit.
    void back_edge(edge_descriptor e, const SpatialGraph &input_sg) {
        auto target = boost::target(e, input_sg);
        // std::cout << "back_edge: " << e << " , target: " << target << " : "
        //           << ArrayUtilities::to_string(input_sg[target].pos)
        //           << std::endl;
        // Check if it is a loop and store in the reference. Used by
        // terminate function of the dfs_visit: finish_on_junctions.
        if (!m_is_not_loop && target == m_source && m_sg_edge.edge_points.size() > 1){
            // std::cout << "Loop: " << target  << std::endl;
            m_is_not_loop = false;
            split_loop(m_vertex_map[target], m_sg_edge, m_sg);
            return;
        }
    }

    void finish_vertex(vertex_descriptor u, const SpatialGraph &input_sg) {
        typedef typename boost::color_traits<typename ColorMap::mapped_type>
            Color;
        // std::cout << "Finish vertex: " << u << ": "
        //           << ArrayUtilities::to_string(input_sg[u].pos) << std::endl;
        // Restore white for vertices with more than 2 degrees.
        // For tree_edge to work on them.
        if (boost::out_degree(u, input_sg) > 2)
            m_color_map[u] = Color::white();
    }
};

template <typename SpatialGraph, typename VertexMap, typename ColorMap>
struct SelfLoopGraphVisitor : public boost::default_dfs_visitor {
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

    SelfLoopGraphVisitor(SpatialGraph &sg, ColorMap &color_map,
                       const vertex_descriptor &start, bool & end_visit_flag)
        : m_sg(sg), m_color_map(color_map),
        m_start(start), m_end_visit_flag(end_visit_flag) {}

    SpatialGraph &m_sg;
    ColorMap &m_color_map;
    const vertex_descriptor & m_start;
    bool & m_end_visit_flag;
    protected:
    SpatialEdge m_sg_edge;
    static const vertex_descriptor max_vertex_id = std::numeric_limits<vertex_descriptor>::max();
    vertex_descriptor m_sg_source = max_vertex_id;
    vertex_descriptor m_source = max_vertex_id;
    bool m_already_started = false;
    public:
    void discover_vertex(vertex_descriptor u,
                         const SpatialGraph &input_sg) {
        if (boost::out_degree(u, input_sg) == 2 && u == m_start && !this->m_already_started) // starting point
        {
            this->m_already_started = true;
            auto sg_vertex_descriptor = boost::add_vertex(input_sg[u], this->m_sg);
            this->m_sg_source = sg_vertex_descriptor;
            this->m_source = m_start;
        }
    }

    void tree_edge(edge_descriptor e, const SpatialGraph &input_sg) {
        auto target = boost::target(e, input_sg);
        this->m_sg_edge.edge_points.push_back(input_sg[target].pos);
    }

    void back_edge(edge_descriptor e, const SpatialGraph &input_sg) {
        auto target = boost::target(e, input_sg);
        if (target == this->m_source && this->m_sg_edge.edge_points.size() > 2){
            // std::cout << "Loop: " << target  << std::endl;
            split_loop(this->m_sg_source, this->m_sg_edge, this->m_sg);
            // This will trigger lambda function to end the visit
            this->m_end_visit_flag = true;
            return;
        }
    }
};

/**
 * Split self-loop (node with an edge to itself) given the vertex_descriptor,
 * spatial_edge containing the edge_points, and the graph.
 * The graph will be modified adding an extra node in the position of the
 * median of the edge_points list.
 * The added node and the loop_vertex_id will be connected with two edges, each
 * containing half of the edge_points of the given sg_edge.
 *
 * Warning: it uses add_vertex, and add_edge so it can invalidate vertex and
 * edge descriptors, iterators etc if the graph uses a vector instead of a list
 * for storing vertices indices. See boost docs. For listS and vecS (undirected)
 * it only invalidates ALL adjacency_iterators.
 *
 * Also note that the adjacency_list must: allow_parallel_edge_tag. All
 * containers except setS and hash_setS allow it.
 * http://www.boost.org/doc/libs/1_60_0/libs/graph/doc/adjacency_list.html
 *
 * @tparam SpatialGraph
 * @param loop_vertex_id vertex_descriptor of the node that has edge connecting
 * to itself.
 * @param sg_edge SpatialEdge containing edge_points.
 * @param input_sg input graph
 *
 */
template <class SpatialGraph>
void split_loop(
    typename SpatialGraph::vertex_descriptor loop_vertex_id,
    const typename boost::edge_bundle_type<SpatialGraph>::type &sg_edge,
    SpatialGraph &input_sg)
{
    using vertex_descriptor =
        typename boost::graph_traits<SpatialGraph>::vertex_descriptor;
    using edge_descriptor =
        typename boost::graph_traits<SpatialGraph>::edge_descriptor;
    using SpatialEdge = typename boost::edge_bundle_type<SpatialGraph>::type;
    using SpatialNode = typename boost::vertex_bundle_type<SpatialGraph>::type;
    auto &edge_points = sg_edge.edge_points;
    size_t middle_index = edge_points.size() / 2;
    // std::cout << "VertexId:" << loop_vertex_id << " .EdgePoints.size():" << edge_points.size()  <<". middle_index: " <<middle_index << std::endl;
    SpatialNode sg_node;
    sg_node.pos = edge_points[middle_index];
    auto created_vertex_id = boost::add_vertex(sg_node, input_sg);
    auto nth = std::next(edge_points.begin(), middle_index);
    SpatialEdge created_sg_edge1;
    created_sg_edge1.edge_points.insert(std::end(created_sg_edge1.edge_points),
                                        edge_points.begin(), nth);
    SpatialEdge created_sg_edge2;
    created_sg_edge2.edge_points.insert(std::end(created_sg_edge2.edge_points),
                                        std::next(nth), edge_points.end());
    boost::add_edge(loop_vertex_id, created_vertex_id,
        created_sg_edge1, input_sg);
    boost::add_edge(loop_vertex_id, created_vertex_id,
        created_sg_edge2, input_sg);
    // std::cout << "Degree Original: " << boost::degree(loop_vertex_id, input_sg) << std::endl;
    // std::cout << "Degree created: " << boost::degree(created_vertex_id, input_sg) << std::endl;
}
/**
 *
 * Uses ReduceGraphVisitor to create a new spatial graph from the input
 * with no chain-nodes (degree 2) and populated spatial edges with
 * the pos of those chain-nodes.
 *
 * The implementation right now starts the visit at end and
 * junction nodes (degree!=2) and then use SelfLoopVisitor
 * to visit unexplored nodes of degree == 2 (part of self-loops)
 * We do store the self-loops as 2 nodes of 2-degree, with 2 spatial edges using @sa split_loop function.
 *
 * @tparam SpatialGraph Graph with spatial info.
 * @param input_sg input
 *
 * @return reduced graph
 */
template <class SpatialGraph>
SpatialGraph reduce_spatial_graph_via_dfs(const SpatialGraph &input_sg) {
    SpatialGraph sg;
    using vertex_descriptor =
        typename boost::graph_traits<SpatialGraph>::vertex_descriptor;
    using vertex_iterator =
        typename boost::graph_traits<SpatialGraph>::vertex_iterator;

    using ColorMap = std::map<vertex_descriptor, boost::default_color_type>;
    ColorMap colorMap;
    typedef typename boost::color_traits<typename ColorMap::mapped_type> Color;
    boost::associative_property_map<ColorMap> propColorMap(colorMap);

    // std::cout << "ReduceGraphVistor:" << std::endl;
    using VertexMap = std::unordered_map<vertex_descriptor, vertex_descriptor>;
    VertexMap vertex_map;
    bool is_not_loop = false;
    ReduceGraphVisitor<SpatialGraph, VertexMap, ColorMap> vis(
        sg, colorMap, vertex_map, is_not_loop);

    // for each end vertex:
    vertex_iterator vi, vi_end;
    std::tie(vi, vi_end) = boost::vertices(input_sg);
    auto vi_start = vi;
    auto start = *vi;
    // Terminate function.
    // Stop the visit when find a non-chain (degree= 2) vertex.
    auto finish_on_junctions = [&start, &is_not_loop, &propColorMap](
                                   vertex_descriptor u, const SpatialGraph &g) {
        // Do not terminate at the start.
        // The self-loop case is handled by back_edge (using is_not_loop)
        if (u == start)
            return false;

        if (boost::out_degree(u, g) != 2) {
            // std::cout << "Exited" << std::endl;
            is_not_loop = true;
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
        if (degree == 1) {
            start = *vi;
            // std::cout << "Visit: start: " << start << " : "
            //           << ArrayUtilities::to_string(input_sg[start].pos)
            //           << ". Degree: " << degree << std::endl;
            boost::depth_first_visit(input_sg, start, vis, propColorMap,
                                     finish_on_junctions);
        }
    }
    for (vi = vi_start; vi != vi_end; ++vi) {
        auto degree = boost::out_degree(*vi, input_sg);
        if (degree > 2) {
            start = *vi;
            // std::cout << "Visit: start: " << start << " : "
            //           << ArrayUtilities::to_string(input_sg[start].pos)
            //           << ". Degree: " << degree << std::endl;
            boost::depth_first_visit(input_sg, start, vis, propColorMap,
                                     finish_on_junctions);
        }
    }

    {
      bool end_visit_flag = false;
      auto finish_on_end_visit_flag = [&end_visit_flag, &propColorMap](
          vertex_descriptor u, const SpatialGraph &g) {
        if (end_visit_flag)
          return true;
        return false;
      };
      // Detect self-loops (no degree > 2 in any vertex)
      SelfLoopGraphVisitor<SpatialGraph, VertexMap, ColorMap> vis_self_loop(
          sg, colorMap, start, end_visit_flag);
      for (vi = vi_start; vi != vi_end; ++vi) {
        if (get(propColorMap, *vi) == Color::white() && boost::out_degree(*vi, input_sg) == 2){
          start = *vi;
          end_visit_flag = false;
          // std::cout << "Self-loops: Visit: start: " << start << " : "
          //           << ArrayUtilities::to_string(input_sg[start].pos) << std::endl;
          boost::depth_first_visit(input_sg, start, vis_self_loop, propColorMap, finish_on_end_visit_flag);
        }
      }
    }

    return sg;
}

} // namespace SG
#endif
