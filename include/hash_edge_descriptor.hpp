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

} // end ns SG

#endif
