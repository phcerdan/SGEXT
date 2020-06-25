/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SGEXT_SUPERGRAPH_HPP
#define SGEXT_SUPERGRAPH_HPP

#include "graph_points_locator.hpp"
#include <boost/graph/subgraph.hpp>

namespace SG {

using SubGraphType = boost::subgraph<GraphType>;
/**
 * Create the root subgraph (a supergraph) of a SpatialGraph
 * with vertex obtained from a list of vtk points.
 *
 * The list of vtkPoints are obtained from merging multiple graphs (at least 2)
 * spatial graphs.
 * Usually the graphs come from downsampling operators,ie. low information
 * graphs are the result of applying the thinning and spatial graph extraction
 * to downsampled images.
 * This facilitate the study of graph topology of zoom-out images,
 * and add details, progresively obtained from higher information images/graphs.
 *
 * This supergraph topology is meaningless, but it acts as a supra-structure
 * to facilitate comparisson between its subgraphs. Each subgraph corresponds to
 * a meaningful spatial graph, obtained from an image.
 *
 *              G_supra
 *
 *             /      \
 *            /        \
 *   G_high  /          \  G_low_info
 *
 *    o                       o
 *    |                       |
 *    o   o                   o
 *  / | \/___o               / \__o
 * |  o |                   /   \
 * o     \_o               o     o
 *
 * G_supra contains all nodes and edges from G_low_info and G_high_info
 * Its topology is meaningless. Multiple vertices could be created in G_supra
 * that actually corresponds to the same shared vertex across scales, but due
 * to minor numerical differences, new vertices would be created.
 *
 * The purpose of the supergraph is to help binding together the scales,
 * for example with local merge operators, where all nodes withing a radius are
 * merged (and its edges handled properly). Remember that the position of a node
 * in a spatial graph is independent from the graph topology.
 * After the mentioned merge, the supergraph unified-vertex can still be linked
 * to its corresponding vertex in the subgraphs, even if the positions is not
 * the same.
 *
 * @param graphs
 *
 * @return Su
 */
SubGraphType create_supergraph(
        const std::vector<std::reference_wrapper<const GraphType>> & graphs);

} // end namespace SG
#endif
