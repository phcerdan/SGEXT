#ifndef REDUCE_GRAPH_HPP
#define REDUCE_GRAPH_HPP

#include <iostream>
#include <tuple>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

/**
 *
 *
 * @tparam ObjectGraph Graph (from  DGtal Object). It might work with any graph with vertex_property with a member: `pos` holding a point.
 * @tparam SpatialGraph reduces graph, with nodes converted to points in spatial edges.
 * @param g input object graph.
 *
 * @return  reduced spatial graph
 */
template < class ObjectGraph, class SpatialGraph >
SpatialGraph reduce_object_to_spatial_graph(const ObjectGraph & obj)
{
    SpatialGraph sg;
    // Find degree of vertices.
    using input_vertex_iterator = typename boost::graph_traits< ObjectGraph >::vertex_iterator ;

    input_vertex_iterator vi,vi_end;
    std::tie(vi,vi_end) = boost::vertices(obj);
    // using input_out_edge_iterator = typename boost::graph_traits< ObjectGraph >::out_edge_iterator;
    // input_out_edge_iterator eoi, eoi_end;
    // std::tie(eoi, eoi_end) = boost::out_edges(*vi, obj);

    using input_adjacency_iterator = typename boost::graph_traits< ObjectGraph >::adjacency_iterator;
    input_adjacency_iterator avi, avi_end;

    // using output_vertex_bundle_type = typename boost::graph_traits< SpatialGraph >::vertex_bundle_type::type;
    using output_vertex_bundle_type = typename boost::vertex_bundle_type< SpatialGraph >::type;
    using output_edge_bundle_type = typename boost::edge_bundle_type< SpatialGraph >::type;

    while(vi != vi_end)
    {
        auto degree = boost::out_degree(*vi, obj);
        if(degree == 1)
        {
            // Populate sg_vertex and add it to the output.
            output_vertex_bundle_type sg_vertex;
            for( size_t i = 0; i < ObjectGraph::Point::dimension; ++i)
                sg_vertex.pos[i] = (*vi).operator[](i);
            auto source = boost::add_vertex(sg_vertex, sg);
            // Create  a spatial edge made from the "chain nodes, degree=2" existent between the source,
            // and the next adjacent vertice with degree != 2.
            output_edge_bundle_type sg_edge;
            auto v_chain = *vi;
            do {
                std::tie(avi, avi_end) = boost::adjacent_vertices(v_chain, obj);
                const auto & av = *avi;
                degree = boost::out_degree(av, obj);
                std::cout <<"Degree of node:" << degree << std::endl;
                std::cout <<av[0] << av[1] << av[2] << std::endl;

                if( degree == 2) // chain node
                {
                    // add to spatial edge points and keep going.
                    typename output_edge_bundle_type::Point sg_edge_point;
                    for( size_t i = 0; i < ObjectGraph::Point::dimension; ++i)
                    {
                        sg_edge_point[i] = av[i];
                    }
                    sg_edge.edge_points.push_back(sg_edge_point);
                    v_chain = av;
                    std::cout <<"degree 2 in chain" << std::endl;
                    std::cout <<sg_edge_point[0] << sg_edge_point[1] << sg_edge_point[2] << std::endl;
                }
                else // junction or end node
                {
                // find that vertex in the graph, or add it.
                // add edge connecting source and this node, with the attached spatial edge containing all the points(nodes of input graph) in between.
                    std::cout <<"Junction/end in chain" << std::endl;
                    output_vertex_bundle_type sg_vertex_target;
                    for( size_t i = 0; i < ObjectGraph::Point::dimension; ++i)
                        sg_vertex_target.pos[i] = av[i];
                    // TODO, need colormap to mark vertex as already added. Use BFS visitor or similar.
                    auto target = add_vertex(sg_vertex_target, sg);
                    // Add edge
                    add_edge(source, target, sg_edge, sg);

                }
            } while ( degree == 2 );
        }
        ++vi;
    }

    // This approach ignore loops.
    return sg;
}

#endif
