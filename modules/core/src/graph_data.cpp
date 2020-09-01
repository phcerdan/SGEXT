/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "graph_data.hpp"
#include <algorithm>
#include <fstream>
#include <iterator>
#include <sstream>

namespace SG {

void print_graph_data(const std::string &name,
                      const std::vector<double> &graph_data,
                      std::ostream &os) {
    os << "# " << name << std::endl;
    std::ostream_iterator<double> out_iter(os, " ");
    std::copy(std::begin(graph_data), std::end(graph_data), out_iter);
}

std::pair<std::string, std::vector<double>> read_graph_data(std::istream &is) {
    using header_data_pair = std::pair<std::string, std::vector<double>>;
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
    while (ss >> num) {
        output.second.push_back(num);
    }
    return output;
}

std::vector<std::pair<std::string, std::vector<double>>>
read_graph_data(const std::string &filename) {
    // output
    std::vector<std::pair<std::string, std::vector<double>>> graph_datas;
    // Open file
    std::ifstream inFile(filename.c_str());
    // Count the number of headers
    size_t nlines = std::count(std::istreambuf_iterator<char>(inFile),
                               std::istreambuf_iterator<char>(), '\n');
    // Reset the file
    inFile.clear();
    inFile.seekg(0, std::ios::beg);
    size_t num_headers = nlines / 2;
    // Parse
    for (size_t index = 0; index < num_headers; ++index) {
        graph_datas.emplace_back(SG::read_graph_data(inFile));
    }
    return graph_datas;
}

} // namespace SG
