/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// Adapted from: http://paal.mimuw.edu.pl/docs/hash_8hpp.html
//=======================================================================
// Copyright (c) 2013 Piotr Wygocki
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
#ifndef HASH_EDGE_DESCRIPTOR_HPP
#define HASH_EDGE_DESCRIPTOR_HPP

#include <boost/functional/hash.hpp>
#include <boost/graph/graph_traits.hpp>

#include <algorithm>
#include <cstddef>

namespace SG {

template <typename Graph, class Enable = void> struct edge_hash {
    typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
    std::size_t operator()(const Edge &e) const {
        std::size_t hash = 0;
        boost::hash_combine(hash, std::min(e.m_source, e.m_target));
        boost::hash_combine(hash, std::max(e.m_source, e.m_target));
        return hash;
    }
};

template <typename Graph>
struct edge_hash<Graph,
                 typename std::enable_if<std::is_same<
                         typename boost::graph_traits<Graph>::directed_category,
                         boost::directed_tag>::value>::type> {
    typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
    std::size_t operator()(const Edge &e) const {
        std::size_t hash = 0;
        boost::hash_combine(hash, e.m_source);
        boost::hash_combine(hash, e.m_target);
        return hash;
    }
};

} // namespace SG

#endif
