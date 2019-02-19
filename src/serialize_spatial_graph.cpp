/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "serialize_spatial_graph.hpp"
#include <boost/graph/adj_list_serialize.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <fstream>

namespace SG {

void write_serialized_graph(GraphType & sg, const std::string & absolute_path)
{
    std::ofstream out;
    out.open(absolute_path.c_str());
    boost::archive::text_oarchive arch(out);
    arch << sg;
};

GraphType read_serialized_graph(const std::string & absolute_path)
{
    GraphType sg;
    std::ifstream in;
    in.open(absolute_path.c_str());
    boost::archive::text_iarchive arch(in);
    arch >> sg;
    return sg;
};
} // ns SG
