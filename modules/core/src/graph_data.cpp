/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "graph_data.hpp"
#include <sstream>
#include <iterator>
#include <algorithm>

namespace SG {

void print_graph_data(const std::string & name, const std::vector<double> & graph_data, std::ostream & os)
{
    os << "# " << name << std::endl;
    std::ostream_iterator<double> out_iter(os, " ");
    std::copy(std::begin(graph_data), std::end(graph_data), out_iter);
}

std::pair<std::string, std::vector<double> > read_graph_data(std::istream &is)
{
    using header_data_pair = std::pair<std::string, std::vector<double> >;
    header_data_pair output;
    std::string line;
    std::getline(is, line);
    std::string delim_first = "# ";
    auto index_first = line.find(delim_first);
    auto start = index_first + delim_first.length();
    output.first = line.substr(start);
    // Data
    double num;
    std::istringstream ss;
    std::getline(is, line);
    // ss.clear();
    ss.str(line);
    while(ss >> num)
    {
        output.second.push_back(num);
    }
    return output;
}

} //end namespace
