/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SG_DYNAMICS_GRAPH_GLUE_HPP
#define SG_DYNAMICS_GRAPH_GLUE_HPP

#include "dynamics_common_data.hpp"
#include "spatial_graph.hpp"

namespace SG {
struct ParticleGraphGlueData {
    ParticleGraphGlueData():
        sys(std::make_unique<System>()),
        particle_graph_map(std::make_unique<particle_graph_map_t>()) {};
    using particle_id_t = decltype(Particle::id);
    using graph_vertex_id_t = GraphType::vertex_descriptor;
    using particle_graph_map_t =
            std::unordered_map<particle_id_t, graph_vertex_id_t>;
    std::unique_ptr<System> sys;
    std::unique_ptr<particle_graph_map_t> particle_graph_map;
    ParticleCollection & particle_collection = sys->all;
    ParticleNeighborsCollection & connected_list = sys->bonds;
};

ParticleGraphGlueData particles_from_graph(const GraphType &graph);
} // namespace SG
#endif