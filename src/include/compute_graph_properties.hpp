#ifndef COMPUTE_GRAPH_PROPERTIES_HPP
#define COMPUTE_GRAPH_PROPERTIES_HPP

#include "spatial_graph.hpp"
// #include <iostream>
// #include <tuple>
// #include <boost/graph/adjacency_list.hpp>
// #include <boost/graph/graph_traits.hpp>
// #include "spatial_graph.hpp"

namespace SG
{

/*
 * Compute degree of all nodes
 *
 * @param sg input spatial graph
 *
 * @return vector with degrees
 */
std::vector<unsigned int> compute_degrees(const SG::GraphAL & sg);

/**
 * Compute end to end distances of nodes
 * It iterates over all edges and compute distance(target,source)
 *
 * @param sg input spatial graph
 *
 * @return vector with distances.
 */
std::vector<double> compute_ete_distances(const SG::GraphAL & sg);

/**
 * Compute contour distances, taking into account every point in the spatial edges.
 * Filtering out edges with less than \b minimum_size_edges edge_points
 *
 * @param sg input spatial graph
 * @param minimum_size_edges filter out edges with less than this number of points.
 *
 * @return vector with distances.
 */
std::vector<double> compute_countour_distances(const SG::GraphAL & sg,
                const size_t minimum_size_edges);

/**
 * Compute contour distances, taking into account every point in the spatial edges.
 *
 * @param sg input spatial graph
 *
 * @return vector with distances.
 */
std::vector<double> compute_countour_distances(const SG::GraphAL & sg);

/**
 * Compute end to end distances of nodes
 * It iterates over all edges and compute distance(target,source)
 * but filtering out edges with less than \b minimum_size_edges edge_points
 *
 * @param sg input spatial graph
 * @param minimum_size_edges filter out edges with less than this number of points.
 *
 * @return vector with distances.
 */
std::vector<double> compute_ete_distances(const SG::GraphAL & sg,
                const size_t minimum_size_edges);
/**
 * Compute angles between all adjacent edges in sg
 *
 * @param sg input spatial graph
 *
 * @return  vector with angles.
 */
std::vector<double> compute_angles(const SG::GraphAL & sg);

/**
 * Compute angles between adjacent edges in sg
 * but filtering out edges with less than \b minimum_size_edges edge_points
 * and the ability to ignore paralell edges.
 *
 * @param sg input spatial graph
 * @param minimum_size_edges filter out edges with less than this number of points.
 * @param ignore_parallel_edges flag to don't compute angles for parallel edges.
 * if false, parallel edges will have an angle of 0.0.
 *
 * @return  vector with angles.
 */
std::vector<double> compute_angles(const SG::GraphAL & sg,
        const size_t minimum_size_edges, const bool ignore_parallel_edges = false);

/**
 * Compute std::cos of input angles.
 *
 * @param angles input @sa compute_angles
 *
 * @return vector with cosines of angles
 */
std::vector<double> compute_cosines(const std::vector<double> & angles);
} // end namespace
#endif
