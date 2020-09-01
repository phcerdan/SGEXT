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
