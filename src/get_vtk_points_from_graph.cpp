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

#include "get_vtk_points_from_graph.hpp"
#include <unordered_map>

namespace SG {

std::pair<
vtkSmartPointer<vtkPoints>,
std::unordered_map< vtkIdType, std::vector<graph_descriptor>>
> get_vtk_points_from_graph(const GraphType & sg){

    using vertex_descriptor =
         boost::graph_traits<GraphType>::vertex_descriptor;
    using vertex_iterator =
         boost::graph_traits<GraphType>::vertex_iterator;
    using edge_iterator =
         boost::graph_traits<GraphType>::edge_iterator;

    auto points = vtkSmartPointer<vtkPoints>::New();

    std::unordered_map< vtkIdType, std::vector<graph_descriptor>> idMap;
    vertex_iterator vi, vi_end;
    std::tie(vi, vi_end) = boost::vertices(sg);
    for (; vi != vi_end; ++vi) {
        auto id = points->InsertNextPoint(sg[*vi].pos[0],
                                          sg[*vi].pos[1],
                                          sg[*vi].pos[2]);
        graph_descriptor gdesc;
        gdesc.exist = true;
        gdesc.is_vertex = true;
        gdesc.vertex_d = *vi;
        std::vector<graph_descriptor> gdescs;
        gdescs.push_back(gdesc);
        idMap[id] = gdescs;
    }

    edge_iterator ei, ei_end;
    std::tie(ei, ei_end) = boost::edges(sg);
    for (; ei != ei_end; ++ei) {
        auto source = boost::source(*ei, sg);
        auto target = boost::target(*ei, sg);
        auto &sg_edge = sg[*ei];
        auto &sg_edge_points = sg_edge.edge_points;
        for (size_t index = 0; index < sg_edge_points.size(); ++index) {
            const auto & p = sg_edge_points[index];
            auto id = points->InsertNextPoint(p[0], p[1], p[2]);
            graph_descriptor gdesc;
            gdesc.exist = true;
            gdesc.is_edge = true;
            gdesc.edge_d = *ei;
            gdesc.edge_points_index = index;
            std::vector<graph_descriptor> gdescs;
            gdescs.push_back(gdesc);
            idMap[id] = gdescs;
        }
    }
    return std::make_pair(points, idMap);
};

void vtk_points_append_graph( const GraphType &inputGraph,
        vtkPointLocator * mergePoints,
        std::unordered_map< vtkIdType, std::vector<graph_descriptor>> & unique_id_map)
{
    const auto number_of_previous_graphs = unique_id_map.cbegin()->second.size();
    auto unique_points = mergePoints->GetPoints();
    const auto & g = inputGraph;
    const auto points_map_pair = get_vtk_points_from_graph(g);
    const auto & points = points_map_pair.first;
    const auto & id_map = points_map_pair.second;
    vtkIdType lastPtId;
    for (vtkIdType point_index = 0; point_index < points->GetNumberOfPoints(); ++point_index){
        // Point will be inserted only if unique
        // lastPtId is modified to the id of the point (recently inserted or already present)
        int is_new_point_inserted = mergePoints->InsertUniquePoint(points->GetPoint(point_index), lastPtId);
        // if the point already exists push_back the graph_descriptor for the new graph
        // if it is new, first push the graph descriptors of previous graphs.
        assert(id_map.at(point_index).size() == 1);
        const auto & current_graph_gdesc = id_map.at(point_index)[0];
        if(!is_new_point_inserted) {
            assert(unique_id_map.at(lastPtId).size() == number_of_previous_graphs);
            unique_id_map[lastPtId].push_back(current_graph_gdesc);
        } else {
            std::vector<graph_descriptor> gdescs_non_existant(number_of_previous_graphs);
            unique_id_map[lastPtId] = gdescs_non_existant;
            // And at the end push_back the current_graph descriptor
            assert(unique_id_map.at(lastPtId).size() == number_of_previous_graphs);
            unique_id_map[lastPtId].push_back(current_graph_gdesc);
        }
        assert(unique_id_map.at(lastPtId).size() == number_of_previous_graphs + 1);
    }
};


std::pair<
vtkSmartPointer<vtkMergePoints>,
std::unordered_map< vtkIdType, std::vector<graph_descriptor>>
> get_vtk_points_from_graphs(const std::vector<std::reference_wrapper<const GraphType>> & graphs)
{
    assert(graphs.size() > 0);
    auto unique_points = vtkSmartPointer<vtkPoints>::New();
    std::unordered_map< vtkIdType, std::vector<graph_descriptor>> unique_id_map;
    // check vtkMergePoints with InsertUniquePoint
    // The approach is to keep the points from get_vtk_points_from_graphs
    // of the first graph
    // vtkSmartPointer<vtkPointSet> pointSet = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkMergePoints> mergePoints = vtkSmartPointer<vtkMergePoints>::New();
    vtkIdType lastPtId;
    {
        const auto points_map_pair = get_vtk_points_from_graph(graphs[0]);
        unique_points = points_map_pair.first;
        unique_id_map = points_map_pair.second;
        // pointSet->SetPoints(unique_points);
        // mergePoints->SetDataSet(pointSet);
        // mergePoints->SetDivisions(10, 10, 10);

        /* Expand the bounding box a 50% from the initial point set. */
        double * boundsFromInitialPoints = unique_points->GetBounds();
        double expandDistance[3];
        expandDistance[0] = std::abs((boundsFromInitialPoints[1] - boundsFromInitialPoints[0])/4.0);
        expandDistance[1] = std::abs((boundsFromInitialPoints[3] - boundsFromInitialPoints[2])/4.0);
        expandDistance[2] = std::abs((boundsFromInitialPoints[5] - boundsFromInitialPoints[4])/4.0);
        double bounds[6];
        bounds[0] = boundsFromInitialPoints[0] - expandDistance[0]; //xmin
        bounds[1] = boundsFromInitialPoints[1] + expandDistance[0]; //xmax
        bounds[2] = boundsFromInitialPoints[2] - expandDistance[1]; //ymin
        bounds[3] = boundsFromInitialPoints[3] + expandDistance[1]; //ymax
        bounds[4] = boundsFromInitialPoints[4] - expandDistance[2]; //zmin
        bounds[5] = boundsFromInitialPoints[5] + expandDistance[2]; //zmax
        // Prepare for insertion
        mergePoints->InitPointInsertion(unique_points, bounds);
        // Insert the points
        for (vtkIdType i = 0; i < unique_points->GetNumberOfPoints(); i++)
        {
            mergePoints->InsertUniquePoint(unique_points->GetPoint(i), lastPtId);
            // They have to be unique from a single graph.
            assert(unique_id_map.at(lastPtId).size() == 1);
        }
    }

    for (size_t graph_index = 1; graph_index < graphs.size() ; ++graph_index){
        vtk_points_append_graph(graphs[graph_index], mergePoints, unique_id_map);
    }
    return std::make_pair(mergePoints, unique_id_map);
};

} // ns SG
