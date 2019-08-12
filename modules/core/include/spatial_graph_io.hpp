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

#ifndef SPATIAL_GRAPH_IO_HPP
#define SPATIAL_GRAPH_IO_HPP

#include "spatial_graph.hpp"
#include <boost/graph/graphviz.hpp>
#include <iostream>

namespace SG {

// Graphviz
boost::dynamic_properties write_dynamic_properties_sg(GraphType &graph);
boost::dynamic_properties read_dynamic_properties_sg(GraphType &graph);
void write_graphviz_sg(std::ostream &os, GraphType &graph);
void write_graphviz_sg(const std::string &output_file, GraphType &graph);
void read_graphviz_sg(std::istream &is, GraphType &graph);
void read_graphviz_sg(const std::string &input_file, GraphType &graph);
GraphType read_graphviz_sg(const std::string &input_file);

// Serialize
void write_serialized_sg(std::ostream &os, GraphType &graph);
void write_serialized_sg(const std::string &output_file, GraphType &graph);
void read_serialized_sg(std::istream &is, GraphType &graph);
void read_serialized_sg(const std::string &input_file, GraphType &graph);
GraphType read_serialized_sg(const std::string &input_file);

} // namespace SG
#endif
