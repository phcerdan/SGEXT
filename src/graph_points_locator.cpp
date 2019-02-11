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

#include "graph_points_locator.hpp"
#include "vtkPolyData.h"
namespace SG {

namespace detail {
void fill_graph_descriptors(std::vector<graph_descriptor> & in_out_gdescs,
        vtkIdList * closeIdList,
        const std::unordered_map<vtkIdType, std::vector<graph_descriptor>> & idMap){
    using namespace SG;
    auto gdescs_size = in_out_gdescs.size();
    // Fill out_gdescs from the closest points.
    for (vtkIdType closeId_index = 0; closeId_index < closeIdList->GetNumberOfIds(); ++closeId_index){
        auto const & gdescs_at_close_index = idMap.at(closeIdList->GetId(closeId_index));
        for (size_t gdescs_index = 0; gdescs_index < gdescs_size ; ++gdescs_index) {
            const auto & gdesc = gdescs_at_close_index[gdescs_index];
            if(in_out_gdescs[gdescs_index].exist)
                continue;
            if(gdesc.exist)
                in_out_gdescs[gdescs_index] = gdesc;
        }
        if(all_graph_descriptors_exist(in_out_gdescs))
            break;
    }
}
} // ns detail

vtkSmartPointer<vtkKdTreePointLocator>
build_kdtree_locator(vtkPoints * inputPoints)
{
    auto kdtree = vtkSmartPointer<vtkKdTreePointLocator>::New();
    // Need to build a data set
    auto dataSet = vtkSmartPointer<vtkPolyData>::New();
    dataSet->SetPoints(inputPoints);
    kdtree->SetDataSet(dataSet);
    kdtree->BuildLocator();
    return kdtree;
};

bool all_graph_descriptors_exist(const std::vector<graph_descriptor> & gdescs)
{
    for(const auto & gdesc: gdescs){
        if(!gdesc.exist) return false;
    }
    return true;
}


std::vector<graph_descriptor> graph_closest_n_points_locator(const PointType &queryPoint, vtkKdTreePointLocator * kdtree,
        const std::unordered_map<vtkIdType, std::vector<graph_descriptor>> & idMap,
        const int closest_n_points)
{
    const size_t gdescs_size = idMap.cbegin()->second.size();
    std::vector<graph_descriptor> out_gdescs(gdescs_size);

    auto closeIdList = vtkSmartPointer<vtkIdList>::New();
    kdtree->FindClosestNPoints(closest_n_points, queryPoint.data(), closeIdList );

    SG::detail::fill_graph_descriptors(out_gdescs, closeIdList, idMap);

    if(!all_graph_descriptors_exist(out_gdescs))
        std::cerr << "WARNING: Graph descriptor not filled for some graph" << std::endl;

    return out_gdescs;
};

std::vector<graph_descriptor> graph_closest_points_by_radius_locator(const PointType &queryPoint, vtkKdTreePointLocator * kdtree,
        const std::unordered_map<vtkIdType, std::vector<graph_descriptor>> & idMap,
        double radius)
{
    const size_t gdescs_size = idMap.cbegin()->second.size();
    std::vector<graph_descriptor> out_gdescs(gdescs_size);

    auto closeIdList = vtkSmartPointer<vtkIdList>::New();
    kdtree->FindPointsWithinRadius(radius, queryPoint.data(), closeIdList);

    if(!closeIdList->GetNumberOfIds())
        std::cerr << "WARNING: No points found within radius " << radius << std::endl;

    // Order the list of points by distance
    std::vector<double> distances2(closeIdList->GetNumberOfIds());
    for (vtkIdType closeId_index = 0; closeId_index < closeIdList->GetNumberOfIds(); ++closeId_index){
        double* point = kdtree->GetDataSet()->GetPoint(closeIdList->GetId(closeId_index));
        distances2[closeId_index] =
            std::pow(point[0] - queryPoint[0], 2) +
            std::pow(point[1] - queryPoint[1], 2) +
            std::pow(point[2] - queryPoint[2], 2);
    }
    std::vector<long int> ids(closeIdList->GetNumberOfIds());
    std::iota(ids.begin(), ids.end(), 0);
    sort(ids.begin(), ids.end(),
            [&distances2](size_t i1, size_t i2){
            return distances2[i1] < distances2[i2];
            });
    auto real_ids = ids;
    for (vtkIdType closeId_index = 0; closeId_index < closeIdList->GetNumberOfIds(); ++closeId_index){
        real_ids[closeId_index] = static_cast<long int>(closeIdList->GetId(ids[closeId_index]));
    }

    // Fill the vtkIdList with the ordered ids
    for (vtkIdType closeId_index = 0; closeId_index < closeIdList->GetNumberOfIds(); ++closeId_index){
        closeIdList->SetId(closeId_index, real_ids[closeId_index]);
    }

    SG::detail::fill_graph_descriptors(out_gdescs, closeIdList, idMap);

    if(!all_graph_descriptors_exist(out_gdescs))
        std::cerr << "WARNING: Graph descriptor not filled for some graph" << std::endl;

    return out_gdescs;
};
} // ns SG
