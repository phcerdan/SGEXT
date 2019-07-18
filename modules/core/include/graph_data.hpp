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

#ifndef graph_data_HPP
#define graph_data_HPP
#include <iostream>
#include <string>
#include <utility> // pair
#include <vector>
namespace SG {

/**
 * Print the data with format:
 * # name
 * value value value
 *
 * @param name degrees, whatever,
 * @param data input data
 * @param os ostream to print the data into
 */
void print_graph_data(const std::string &name,
                      const std::vector<double> &data,
                      std::ostream &os);

/**
 * Read data form a graph_data 2 lines of format:
 * # name
 * value value value
 *
 * @param is input file stream
 *
 * @return vector of pair [string, vector<double>]
 */
std::pair<std::string, std::vector<double> > read_graph_data(std::istream &is);
} // namespace SG
#endif
