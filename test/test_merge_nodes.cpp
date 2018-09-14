#include "DGtal/graph/ObjectBoostGraphInterface.h"
#include "catch_header.h"
#include "spatial_graph.hpp"
#include <DGtal/base/Common.h>
#include <DGtal/helpers/StdDefs.h>
#include <DGtal/topology/Object.h>
#include <iostream>

#include "reduce_spatial_graph_via_dfs.hpp"
#include "spatial_graph_from_object.hpp"
#include "remove_extra_edges.hpp"
#include "merge_nodes.hpp"
#include "spatial_graph_utilities.hpp"
// use it to check there is no exception in cornercase
#include "compute_graph_properties.hpp"

struct object_graph {
    using Domain = DGtal::Z3i::Domain;
    using DigitalTopology = DGtal::Z3i::DT26_6;
    using DigitalSet = DGtal::DigitalSetByAssociativeContainer<
        Domain, std::unordered_set<typename Domain::Point>>;
    using Object = DGtal::Object<DigitalTopology, DigitalSet>;
    using vertex_iterator =
        typename boost::graph_traits<Object>::vertex_iterator;
    using edge_iterator = typename boost::graph_traits<Object>::edge_iterator;

    Object obj;
};

/**
 * In 3D, three nodes connected between them, with edges between them of the same size.
 * Domain::Point n0(0, 0, 0);
 * Domain::Point n1(1, 1, 0);
 * Domain::Point n2(1, 0, 1);
 *
 * Domain::Point r0(-1, 0, 0);
 * Domain::Point r1(1, 2, 0);
 * Domain::Point r2(1, 0, 2);
 *
 */
struct three_connected_nodes : public object_graph {
    three_connected_nodes(){
        DigitalTopology::ForegroundAdjacency adjF;
        DigitalTopology::BackgroundAdjacency adjB;
        DigitalTopology topo(adjF, adjB,
                DGtal::DigitalTopologyProperties::JORDAN_DT);

        Domain::Point b1(-10, -10, -10);
        Domain::Point b2(10, 10, 10);
        Domain domain(b1, b2);
        DigitalSet obj_set(domain);
        Domain::Point n0(0, 0, 0);
        Domain::Point n1(1, 1, 0);
        Domain::Point n2(1, 0, 1);
        Domain::Point r0(-1, 0, 0);
        Domain::Point r1(1, 2, 0);
        Domain::Point r2(1, 0, 2);
        obj_set.insertNew(n0);
        obj_set.insertNew(n1);
        obj_set.insertNew(n2);
        obj_set.insertNew(r0);
        obj_set.insertNew(r1);
        obj_set.insertNew(r2);
        this->obj = Object(topo, obj_set);
    }
};

TEST_CASE_METHOD(three_connected_nodes,
                 "three_connected_nodes",
                 "[merge_nodes]") {
    std::cout << "Three connected nodes" << std::endl;
    using SpatialGraph = SG::GraphAL;
    using vertex_iterator =
        typename boost::graph_traits<SpatialGraph>::vertex_iterator;
    SpatialGraph sg = SG::spatial_graph_from_object<Object, SpatialGraph>(obj);
    vertex_iterator vi, vi_end;
    std::tie(vi, vi_end) = boost::vertices(sg);
    size_t count1degrees = 0;
    size_t count2degrees = 0;
    size_t count3degrees = 0;
    for (; vi != vi_end; ++vi) {
        auto degree = boost::out_degree(*vi, sg);
        if (degree == 1)
            count1degrees++;
        if (degree == 2)
            count2degrees++;
        if (degree == 3)
            count3degrees++;
    }
    CHECK(num_vertices(sg) == 6);
    CHECK(num_edges(sg) == 6);
    CHECK(count3degrees == 3);
    CHECK(count2degrees == 0);
    CHECK(count1degrees == 3);
    bool any_edge_removed = remove_extra_edges(sg);
    CHECK_FALSE(any_edge_removed);
    CHECK(num_vertices(sg) == 6);
    CHECK(num_edges(sg) == 6);
    SpatialGraph reduced_g = SG::reduce_spatial_graph_via_dfs(sg);
    CHECK(num_vertices(reduced_g) == num_vertices(sg));
    CHECK(num_edges(reduced_g) == num_edges(sg));
    std::cout << "After reduction:" << std::endl;
    SG::print_degrees(reduced_g);
    SG::print_edges(reduced_g);

    std::cout << "Merging three_connected_nodes" << std::endl;
    auto nodes_merged = SG::merge_three_connected_nodes(reduced_g);
    CHECK(nodes_merged == 2);
    // Nodes are not removed but cleared (no edges attached, degree=0)
    // CHECK(num_vertices(reduced_g) == num_vertices(sg) - 2);
    CHECK(num_vertices(reduced_g) == num_vertices(sg));
    CHECK(num_edges(reduced_g) == num_edges(sg) - 3);
    std::cout << "After merge:" << std::endl;
    SG::print_degrees(reduced_g);
    SG::print_spatial_edges(reduced_g);
    count1degrees = 0;
    count2degrees = 0;
    count3degrees = 0;
    size_t count0degrees = 0;
    std::tie(vi, vi_end) = boost::vertices(reduced_g);
    for (; vi != vi_end; ++vi) {
        auto degree = boost::out_degree(*vi, reduced_g);
        if (degree == 0)
            count0degrees++;
        if (degree == 1)
            count1degrees++;
        if (degree == 2)
            count2degrees++;
        if (degree == 3)
            count3degrees++;
    }
    CHECK(count3degrees == 1);
    CHECK(count2degrees == 0);
    CHECK(count1degrees == 3);
    CHECK(count0degrees == 2);
}

struct contour_length_cornercase2 : public object_graph {
    contour_length_cornercase2() {
        DigitalTopology::ForegroundAdjacency adjF;
        DigitalTopology::BackgroundAdjacency adjB;
        DigitalTopology topo(adjF, adjB,
                DGtal::DigitalTopologyProperties::JORDAN_DT);

        Domain::Point b1(340, 150, 20);
        Domain::Point b2(360, 170, 40);
        Domain domain(b1, b2);
        DigitalSet obj_set(domain);
        Domain::Point n0(348, 158, 30);
        Domain::Point n00(348, 158, 29);
        Domain::Point n01(348, 159, 28);
        Domain::Point n1(350, 157, 33);
        Domain::Point n2(350, 160, 30);
        Domain::Point n20(351, 160, 31);
        Domain::Point n21(351, 161, 29);
        Domain::Point e0(349, 158, 31);
        Domain::Point e1(350, 158, 32); 
        Domain::Point e10(351, 158, 33);
        Domain::Point e11(351, 159, 33);
        Domain::Point e2(350, 159, 31);
        obj_set.insertNew(n0);
        obj_set.insertNew(n00);
        obj_set.insertNew(n01);
        obj_set.insertNew(n1);
        obj_set.insertNew(n2);
        obj_set.insertNew(n20);
        obj_set.insertNew(n21);
        obj_set.insertNew(e0);
        obj_set.insertNew(e1);
        obj_set.insertNew(e10);
        obj_set.insertNew(e11);
        obj_set.insertNew(e2);
        this->obj = Object(topo, obj_set);
    }
};



// This test spotted a bug where we were merging nodes with neighbors degree > 3
TEST_CASE_METHOD(contour_length_cornercase2,
                 "contour_length_cornercase2",
                 "[merge_nodes]")
{
    using SpatialGraph = SG::GraphAL;
    using vertex_iterator =
        typename boost::graph_traits<SpatialGraph>::vertex_iterator;
    SpatialGraph sg = SG::spatial_graph_from_object<Object, SpatialGraph>(obj);
    vertex_iterator vi, vi_end;
    bool any_edge_removed = remove_extra_edges(sg);
    CHECK(any_edge_removed == true);
    CHECK(num_vertices(sg) == 12);
    CHECK(num_edges(sg) == 13);
    SpatialGraph reduced_g = SG::reduce_spatial_graph_via_dfs(sg);
    CHECK(num_vertices(reduced_g) == 7 );
    CHECK(num_edges(reduced_g) == 8 );
    std::cout << "After reduction:" << std::endl;
    SG::print_degrees(reduced_g);
    SG::print_edges(reduced_g);

    std::cout << "Merging contourlength cornercase2 " << std::endl;
    auto nodes_merged = SG::merge_three_connected_nodes(reduced_g);
    CHECK(nodes_merged == 0);
    // Nodes are not removed but cleared (no edges attached, degree=0)
    // CHECK(num_vertices(reduced_g) == num_vertices(sg) - 2);
    CHECK(num_vertices(reduced_g) == 7);
    CHECK(num_edges(reduced_g) == 8 );
    std::cout << "After merge:" << std::endl;
    SG::print_degrees(reduced_g);
    SG::print_spatial_edges(reduced_g);
    size_t count1degrees = 0;
    size_t count2degrees = 0;
    size_t count3degrees = 0;
    size_t count4degrees = 0;
    size_t count0degrees = 0;
    std::tie(vi, vi_end) = boost::vertices(reduced_g);
    for (; vi != vi_end; ++vi) {
        auto degree = boost::out_degree(*vi, reduced_g);
        if (degree == 0)
            count0degrees++;
        if (degree == 1)
            count1degrees++;
        if (degree == 2)
            count2degrees++;
        if (degree == 3)
            count3degrees++;
        if (degree == 4)
            count4degrees++;
    }
    CHECK(count4degrees == 1);
    CHECK(count3degrees == 3);
    CHECK(count2degrees == 0);
    CHECK(count1degrees == 3);
    CHECK(count0degrees == 0);
    CHECK_NOTHROW(SG::compute_contour_lengths(reduced_g));
}
