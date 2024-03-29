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

#include "reduce_spatial_graph_via_dfs.hpp"
#include "reduce_dfs_visitor.hpp"
namespace SG {

GraphType reduce_spatial_graph_via_dfs(const GraphType &input_sg,
                                       bool verbose) {
    GraphType sg;
    using vertex_descriptor = boost::graph_traits<GraphType>::vertex_descriptor;
    using vertex_iterator = boost::graph_traits<GraphType>::vertex_iterator;

    using ColorMap = std::map<vertex_descriptor, boost::default_color_type>;
    ColorMap colorMap;
    using Color = boost::color_traits<ColorMap::mapped_type>;
    boost::associative_property_map<ColorMap> propColorMap(colorMap);

    // std::cout << "ReduceGraphVistor:" << std::endl;
    using VertexMap = std::unordered_map<vertex_descriptor, vertex_descriptor>;
    VertexMap vertex_map;
    bool is_not_loop = false;
    ReduceGraphVisitor<GraphType, VertexMap, ColorMap> vis(
            sg, colorMap, vertex_map, is_not_loop, verbose);

    // for each end vertex:
    vertex_iterator vi, vi_end;
    std::tie(vi, vi_end) = boost::vertices(input_sg);
    auto vi_start = vi;
    auto start = *vi;
    // Terminate function.
    // Stop the visit when find a non-chain (degree= 2) vertex.
    auto finish_on_junctions = [&start, &is_not_loop, &propColorMap,
                                &verbose](vertex_descriptor u,
                                          const GraphType &g) {
        // Do not terminate at the start.
        // The self-loop case is handled by back_edge (using is_not_loop)
        if (u == start) {
            return false;
        }

        if (boost::out_degree(u, g) != 2) {
            if (verbose) {
                std::cout << "Exited due to finish_on_junctions" << std::endl;
            }
            is_not_loop = true;
            put(propColorMap, u, Color::white());
            return true;
        }
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
            if (verbose) {
                std::cout << "Visit: start: " << start << " : "
                          << ArrayUtilities::to_string(input_sg[start].pos)
                          << ". Degree: " << degree << std::endl;
            }
            boost::depth_first_visit(input_sg, start, vis, propColorMap,
                                     finish_on_junctions);
        }
    }
    for (vi = vi_start; vi != vi_end; ++vi) {
        auto degree = boost::out_degree(*vi, input_sg);
        if (degree > 2) {
            start = *vi;
            if (verbose) {
                std::cout << "Visit: start: " << start << " : "
                          << ArrayUtilities::to_string(input_sg[start].pos)
                          << ". Degree: " << degree << std::endl;
            }
            boost::depth_first_visit(input_sg, start, vis, propColorMap,
                                     finish_on_junctions);
        }
    }

    {
        bool end_visit_flag = false;
        auto finish_on_end_visit_flag = [&end_visit_flag](vertex_descriptor /*unused*/,
                                                        const GraphType & /*unused*/) {
            // finish if end_visit_flag is true
            return end_visit_flag;
        };
        // Detect self-loops (no degree > 2 in any vertex)
        SelfLoopGraphVisitor<GraphType, VertexMap, ColorMap> vis_self_loop(
                sg, colorMap, start, end_visit_flag);
        for (vi = vi_start; vi != vi_end; ++vi) {
            if (get(propColorMap, *vi) == Color::white() &&
                boost::out_degree(*vi, input_sg) == 2) {
                start = *vi;
                end_visit_flag = false;
                if (verbose) {
                    std::cout << "Self-loops: Visit: start: " << start << " : "
                              << ArrayUtilities::to_string(input_sg[start].pos)
                              << std::endl;
                }
                boost::depth_first_visit(input_sg, start, vis_self_loop,
                                         propColorMap,
                                         finish_on_end_visit_flag);
            }
        }
    }

    return sg;
}
} // namespace SG
