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
#include "spatial_graph_utilities.hpp"

namespace SG {

PointsIdMapPair get_vtk_points_from_graph(const GraphType &sg) {
  using vertex_descriptor = boost::graph_traits<GraphType>::vertex_descriptor;
  using vertex_iterator = boost::graph_traits<GraphType>::vertex_iterator;
  using edge_iterator = boost::graph_traits<GraphType>::edge_iterator;
  auto points = vtkSmartPointer<vtkPoints>::New();

  std::unordered_map<vtkIdType, std::vector<graph_descriptor>> idMap;
  vertex_iterator vi, vi_end;
  std::tie(vi, vi_end) = boost::vertices(sg);
  for(; vi != vi_end; ++vi) {
    auto id =
        points->InsertNextPoint(sg[*vi].pos[0], sg[*vi].pos[1], sg[*vi].pos[2]);
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
  for(; ei != ei_end; ++ei) {
    auto &sg_edge = sg[*ei];
    auto &sg_edge_points = sg_edge.edge_points;
    for(size_t index = 0; index < sg_edge_points.size(); ++index) {
      const auto &p = sg_edge_points[index];
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
#ifndef NDEBUG
  std::set<PointType> repeated_points;
  bool repeated_exists;
  std::tie(repeated_points, repeated_exists) = check_unique_points_in_graph(sg);
  if(repeated_exists) {
    std::cout
        << "Warning: duplicated points exist in get_vtk_points_from_graph."
           "Repeated Points: "
        << repeated_points.size() << std::endl;
    for(const auto &p : repeated_points) {
      SG::print_pos(std::cout, p);
      std::cout << std::endl;
    }
  }
  // assert(duplicated_points_exist == false);
#endif

  return std::make_pair(points, idMap);
};

void append_new_graph_points(
    PointsIdMapPair &new_graph_point_map_pair, vtkPointLocator *mergePoints,
    std::unordered_map<vtkIdType, std::vector<graph_descriptor>>
        &unique_id_map) {
  append_new_graph_points(new_graph_point_map_pair.first,
                          new_graph_point_map_pair.second, mergePoints,
                          unique_id_map);
}

void append_new_graph_points(
    vtkPoints *new_graph_points,
    const std::unordered_map<vtkIdType, std::vector<graph_descriptor>>
        &new_graph_id_map,
    vtkPointLocator *mergePoints,
    std::unordered_map<vtkIdType, std::vector<graph_descriptor>>
        &unique_id_map) {
  const auto number_of_previous_graphs = unique_id_map.cbegin()->second.size();
  auto unique_points = mergePoints->GetPoints();
  // const auto & g = inputGraph;
  // const auto points_map_pair = get_vtk_points_from_graph(g);
  BoundingBox box(mergePoints->GetBounds());
  if(!box.are_bounds_inside(new_graph_points->GetBounds())) {
    box.Print("existing merge_points bounds");
    box.Print(new_graph_points->GetBounds(),
              "new graph new_graph_points bounds");
    throw std::runtime_error(
        "vtk_new_graph_points_append_graph: new graph has new_graph_points "
        "outside of the merger bounding box");
  }
  assert(box.are_bounds_inside(new_graph_points->GetBounds()));
  vtkIdType lastPtId;
  for(vtkIdType point_index = 0;
      point_index < new_graph_points->GetNumberOfPoints(); ++point_index) {
    // Point will be inserted only if unique
    // lastPtId is modified to the id of the point (recently inserted or already
    // present)
    int is_new_point_inserted = mergePoints->InsertUniquePoint(
        new_graph_points->GetPoint(point_index), lastPtId);
    // If the point already exists push_back the graph_descriptor for the new
    // graph. If it is new, first push the graph descriptors of previous graphs.
    assert(new_graph_id_map.at(point_index).size() == 1);
    const auto &current_graph_gdesc = new_graph_id_map.at(point_index)[0];
    if(!is_new_point_inserted) {
      assert(unique_id_map.at(lastPtId).size() == number_of_previous_graphs);
      unique_id_map[lastPtId].push_back(current_graph_gdesc);
    } else {
      std::vector<graph_descriptor> gdescs_non_existant(
          number_of_previous_graphs);
      unique_id_map[lastPtId] = gdescs_non_existant;
      // And at the end push_back the current_graph descriptor
      assert(unique_id_map.at(lastPtId).size() == number_of_previous_graphs);
      unique_id_map[lastPtId].push_back(current_graph_gdesc);
    }
    assert(unique_id_map.at(lastPtId).size() == number_of_previous_graphs + 1);
  }
};

MergePointsIdMapPair get_vtk_points_from_graphs(
    const std::vector<std::reference_wrapper<const GraphType>> &graphs,
    const BoundingBox *box) {
  assert(graphs.size() > 0);
  auto unique_points = vtkSmartPointer<vtkPoints>::New();
  std::unordered_map<vtkIdType, std::vector<graph_descriptor>> unique_id_map;
  // check vtkMergePoints with InsertUniquePoint
  // The approach is to keep the points from get_vtk_points_from_graphs
  // of the first graph
  // vtkSmartPointer<vtkPointSet> pointSet =
  // vtkSmartPointer<vtkPolyData>::New();
  vtkSmartPointer<vtkMergePoints> mergePoints =
      vtkSmartPointer<vtkMergePoints>::New();
  vtkIdType lastPtId;
  std::vector<PointsIdMapPair> graphs_point_map;
  std::vector<double *> bounds_vector;
  for(size_t graph_index = 0; graph_index < graphs.size(); ++graph_index) {
    graphs_point_map.emplace_back(
        get_vtk_points_from_graph(graphs[graph_index]));
    const auto &points_per_graph = graphs_point_map.back().first;
    bounds_vector.emplace_back(points_per_graph->GetBounds());
  }

  BoundingBox enclosing_box;
  if(!box)
    enclosing_box = BoundingBox::BuildEnclosingBox(bounds_vector);
  else
    enclosing_box = *box;

  // Init the merger with points from the first graph
  {
    unique_points = graphs_point_map[0].first;
    double bounds[6];
    enclosing_box.GetBounds(bounds);
    mergePoints->InitPointInsertion(unique_points, bounds);
    // Insert the points
    unique_id_map = graphs_point_map[0].second;
    for(vtkIdType i = 0; i < unique_points->GetNumberOfPoints(); i++) {
      mergePoints->InsertUniquePoint(unique_points->GetPoint(i), lastPtId);
    }
  }

  for(size_t graph_index = 1; graph_index < graphs.size(); ++graph_index) {
    append_new_graph_points(graphs_point_map[graph_index], mergePoints,
                            unique_id_map);
  }

  return std::make_pair(mergePoints, unique_id_map);
};

}  // namespace SG
