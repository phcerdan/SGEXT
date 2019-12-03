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
