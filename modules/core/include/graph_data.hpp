/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
