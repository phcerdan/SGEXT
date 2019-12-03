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

#include "dynamics_graph_glue.hpp"

namespace SG {
ParticleGraphGlueData particles_from_graph(const GraphType &graph) {
    ParticleGraphGlueData glue_data;
    const auto num_vertices = boost::num_vertices(graph);
    auto &particles = glue_data.particle_collection.particles;
    auto &particle_graph_map = glue_data.particle_graph_map;
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
    return glue_data;
};
} // namespace SG
