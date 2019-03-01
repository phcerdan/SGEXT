/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "graph_points_locator.hpp"
#include "vtkPolyData.h"
namespace SG {

std::vector<IdWithGraphDescriptor>
closest_existing_descriptors_by_graph(
        vtkIdList * closeIdList,
        const std::unordered_map<vtkIdType, std::vector<graph_descriptor>> & idMap)
{
    const size_t gdescs_size = idMap.cbegin()->second.size();
    std::vector<IdWithGraphDescriptor> id_graph_descriptors(gdescs_size);
    // Fill id_graph_descriptors from the closest points.
    // the list should be ordered from closest to furthest
    for (vtkIdType closeId_index = 0; closeId_index < closeIdList->GetNumberOfIds(); ++closeId_index){
        vtkIdType idList = closeIdList->GetId(closeId_index);
        auto const & gdescs_at_close_index = idMap.at(idList);
        for (size_t gdescs_index = 0; gdescs_index < gdescs_size ; ++gdescs_index) {
            if(id_graph_descriptors[gdescs_index].exist)
                continue;
            const auto & gdesc = gdescs_at_close_index[gdescs_index];
            if(gdesc.exist) {
                id_graph_descriptors[gdescs_index].exist = true;
                id_graph_descriptors[gdescs_index].id = idList;
                id_graph_descriptors[gdescs_index].descriptor = gdesc;
            }
        }
        if(all_graph_descriptors_exist(id_graph_descriptors))
            break;
    }
    return id_graph_descriptors;
}

std::vector<IdWithGraphDescriptor>
closest_existing_vertex_by_graph(
        vtkIdList * closeIdList,
        const std::unordered_map<vtkIdType, std::vector<graph_descriptor>> & idMap)
{
    const size_t gdescs_size = idMap.cbegin()->second.size();
    std::vector<IdWithGraphDescriptor> id_graph_descriptors(gdescs_size);
    for (vtkIdType closeId_index = 0; closeId_index < closeIdList->GetNumberOfIds(); ++closeId_index){
        vtkIdType idList = closeIdList->GetId(closeId_index);
        auto const & gdescs_at_close_index = idMap.at(idList);
        for (size_t gdescs_index = 0; gdescs_index < gdescs_size ; ++gdescs_index) {
            if(id_graph_descriptors[gdescs_index].exist)
                continue;
            const auto & gdesc = gdescs_at_close_index[gdescs_index];
            if(gdesc.exist && gdesc.is_vertex) {
                id_graph_descriptors[gdescs_index].exist = true;
                id_graph_descriptors[gdescs_index].id = idList;
                id_graph_descriptors[gdescs_index].descriptor = gdesc;
            }
        }
        if(all_graph_descriptors_exist(id_graph_descriptors))
            break;
    }
    return id_graph_descriptors;
}

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
}

bool all_graph_descriptors_exist(const std::vector<IdWithGraphDescriptor> & gdescs)
{
    for(const auto & gdesc_with_id: gdescs){
        if(!gdesc_with_id.exist) return false;
    }
    return true;
}

bool all_graph_descriptors_exist(const std::vector<graph_descriptor> & gdescs)
{
    for(const auto & gdesc: gdescs){
        if(!gdesc.exist) return false;
    }
    return true;
}


vtkSmartPointer<vtkIdList>
graph_closest_n_points_locator(
        const PointType &queryPoint,
        vtkKdTreePointLocator * kdtree,
        const std::unordered_map<vtkIdType, std::vector<graph_descriptor>> & idMap,
        const int closest_n_points)
{
    auto closeIdList = vtkSmartPointer<vtkIdList>::New();
    kdtree->FindClosestNPoints(closest_n_points, queryPoint.data(), closeIdList );

    return closeIdList;
    // auto out_gdescs = SG::closest_existing_descriptors_by_graph(closeIdList, idMap);
    // if(!all_graph_descriptors_exist(out_gdescs))
    //     std::cerr << "WARNING: Graph descriptor not filled for some graph" << std::endl;
    // return out_gdescs;
};

vtkSmartPointer<vtkIdList>
graph_closest_points_by_radius_locator(
        const PointType &queryPoint,
        vtkKdTreePointLocator * kdtree,
        const std::unordered_map<vtkIdType, std::vector<graph_descriptor>> & idMap,
        double radius)
{
    const size_t gdescs_size = idMap.cbegin()->second.size();

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

    return closeIdList;
    // auto out_gdescs = SG::closest_existing_descriptors_by_graph(closeIdList, idMap);
    // if(!all_graph_descriptors_exist(out_gdescs))
    //     std::cerr << "WARNING: Graph descriptor not filled for some graph" << std::endl;
    // return out_gdescs;
};
} // ns SG
