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

#include "serialize_spatial_graph.hpp"
#include <boost/graph/adj_list_serialize.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <fstream>

namespace SG {

void write_serialized_graph(GraphType& sg, const std::string& absolute_path) {
  std::ofstream out;
  out.open(absolute_path.c_str());
  boost::archive::text_oarchive arch(out);
  arch << sg;
};

GraphType read_serialized_graph(const std::string& absolute_path) {
  GraphType sg;
  std::ifstream in;
  in.open(absolute_path.c_str());
  boost::archive::text_iarchive arch(in);
  arch >> sg;
  return sg;
};
}  // namespace SG
