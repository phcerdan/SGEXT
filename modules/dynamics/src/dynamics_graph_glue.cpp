/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "dynamics_graph_glue.hpp"
#include "system.hpp"
#include "edge_points_utilities.hpp" // for contour_length

namespace SG {
ParticleGraphGlueData particles_from_graph(const GraphType &graph) {
    ParticleGraphGlueData glue_data;
    const auto num_vertices = boost::num_vertices(graph);
    auto &particles = glue_data.particle_collection.particles;
    auto &particle_graph_map = glue_data.particle_graph_map;
    auto &graph_particle_map = glue_data.graph_particle_map;
    auto &connected_list = glue_data.connected_list;
    particles.reserve(num_vertices);
    connected_list.collection.reserve(num_vertices);
    // we assign consecutive ids to particle_collection, so it's sorted.
    glue_data.particle_collection.sorted = true;
    size_t particle_id = 0;
    for (auto [ui, ui_end] = boost::vertices(graph); ui != ui_end;
         ++ui, ++particle_id) {
        const Particle particle = {particle_id, graph[*ui].pos};
        particles.emplace_back(particle);
        particle_graph_map->operator[](particle_id) = *ui;
        graph_particle_map->operator[](*ui) = particle_id;
        const auto degree = boost::out_degree(*ui, graph);
        ParticleNeighbors connected_particles;
        connected_particles.particle_id = particle_id;
        connected_particles.neighbors.reserve(degree);
        for (auto [adj_i, adj_i_end] = boost::adjacent_vertices(*ui, graph);
             adj_i != adj_i_end; ++adj_i) {
            connected_particles.neighbors.emplace_back(*adj_i);
        }
        connected_list.collection.emplace_back(connected_particles);
    }
    // add bonds
    // BondChain has contour length attribute
    glue_data.bond_collection = make_unique_bonds_from_system_conexions<BondChain>(*(glue_data.sys));
    // iterate over bonds and populate contour length
    for(auto & bond : glue_data.bond_collection.bonds) {
        const auto source_g = graph_particle_map->at(bond->id_a);
        const auto target_g = graph_particle_map->at(bond->id_b);
        // graph is undirected, source, target does not matter
        const auto [edge_desc, exists] = boost::edge(source_g, target_g, graph);
        assert(exists == true && "Edge has to exist");
        std::static_pointer_cast<BondChain>(bond)->length_contour = contour_length(edge_desc, graph);
    }

    return glue_data;
};
} // namespace SG
