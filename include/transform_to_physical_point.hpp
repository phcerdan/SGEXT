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

#ifndef TRANSFORM_TO_PHYSICAL_POINT_HPP
#define TRANSFORM_TO_PHYSICAL_POINT_HPP

#include "spatial_graph.hpp" // for SG::GraphAL
#include "spatial_edge.hpp" // for SG::PointType

namespace SG {

template<typename TImage>
SG::PointType index_array_to_physical_point_array(const SG::PointType &input_array,
    const TImage* itk_image)
{
  auto dim = TImage::ImageDimension;
  if (input_array.size() != dim)
    throw std::runtime_error("index_array_to_physical_point_array, image dimension" +
        std::to_string(dim) + " doesn't fit with array size " +
        std::to_string( input_array.size() ));

  using ITKIndexType = typename TImage::IndexType;
  using ITKPointType = typename TImage::PointType;

  ITKIndexType input_index;
  for (size_t i = 0; i < dim; i++) {
    input_index[i] = input_array[i];
  }

  ITKPointType physical_point;
  itk_image->TransformIndexToPhysicalPoint(input_index, physical_point);

  SG::PointType ret;
  for (size_t i = 0; i < dim; i++) {
    ret[i] = physical_point[i];
  }
  return ret;
};

template<typename TImage>
void transform_graph_to_physical_point( SG::GraphAL & sg, const TImage* itk_image)
{
  // Loop over all nodes and transform pos.
  auto verts = boost::vertices(sg);
  for(auto&& vi = verts.first; vi != verts.second; ++vi)
  {
    sg[*vi].pos = std::move(index_array_to_physical_point_array(sg[*vi].pos, itk_image));
  }
  // Loop over all edges and transform edge_points
  auto edges = boost::edges(sg);
  for (auto ei = edges.first; ei != edges.second; ++ei) {
    for(auto && ep : sg[*ei].edge_points) {
      ep = std::move(index_array_to_physical_point_array(ep, itk_image));
    }
  }
};

} // end namespace
#endif
