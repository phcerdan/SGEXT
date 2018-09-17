#include "gmock/gmock.h"
#include "DGtal/graph/ObjectBoostGraphInterface.h"
#include "spatial_graph.hpp"
#include <DGtal/base/Common.h>
#include <DGtal/helpers/StdDefs.h>
#include <DGtal/topology/Object.h>
#include <iostream>

#include "reduce_spatial_graph_via_dfs.hpp"
#include "spatial_graph_from_object.hpp"
#include "remove_extra_edges.hpp"
#include "spatial_graph_utilities.hpp"
#include "merge_nodes.hpp"
// #include "visualize_spatial_graph.hpp"

// Viewer
// #include "DGtal/io/Color.h"
// #include "DGtal/io/colormaps/GradientColorMap.h"
// #include "DGtal/io/DrawWithDisplay3DModifier.h"
// #include <DGtal/io/viewers/Viewer3D.h>

struct SpatialGraphBaseFixture  {
    using GraphType = SG::GraphAL;
    GraphType g;
    using vertex_iterator =
        typename boost::graph_traits<GraphType>::vertex_iterator;
    using edge_iterator =
        typename boost::graph_traits<GraphType>::edge_iterator;
};

/**
 * Spatial Graph
 *
 * o-o
 * | |
 * o-o
 */
struct sg_square :
    public SpatialGraphBaseFixture, ::testing::Test {
    void SetUp() override {
        using boost::add_edge;
        this->g = GraphType(4);
        g[0].pos = {{0, 0, 0}};
        g[1].pos = {{1, 0, 0}};
        g[2].pos = {{1, 1, 0}};
        g[3].pos = {{0, 1, 0}};
        add_edge(0, 1, g);
        add_edge(1, 2, g);
        add_edge(2, 3, g);
        add_edge(3, 0, g);
    }
};

struct sg_square_expected : public SpatialGraphBaseFixture {
    sg_square_expected() {
        using boost::add_edge;
        this->g = GraphType(2);
        g[0].pos = {{0, 0, 0}};
        g[1].pos = {{1, 1, 0}};

        SG::SpatialEdge se01_1;
        se01_1.edge_points.insert(std::end(se01_1.edge_points),
                                {{1,0,0}});
        add_edge(0, 1, se01_1, g);
        SG::SpatialEdge se01_2;
        se01_2.edge_points.insert(std::end(se01_2.edge_points),
                                {{0,1,0}});
        add_edge(0, 1, se01_2, g);
    }
};

/**
 * Spatial Graph
 *
 * o-o
 * | |
 * o-o
 * |
 * o
 */
struct sg_square_plus_one :
    public SpatialGraphBaseFixture, ::testing::Test {
    void SetUp() override {
        using boost::add_edge;
        this->g = GraphType(5);
        g[0].pos = {{0, 0, 0}};
        g[1].pos = {{1, 0, 0}};
        g[2].pos = {{1, 1, 0}};
        g[3].pos = {{0, 1, 0}};
        g[4].pos = {{0, -1, 0}};
        add_edge(0, 1, g);
        add_edge(1, 2, g);
        add_edge(2, 3, g);
        add_edge(3, 0, g);
        add_edge(4, 0, g);
    }
};

struct sg_square_plus_one_expected : public SpatialGraphBaseFixture {
    sg_square_plus_one_expected() {
        using boost::add_edge;
        this->g = GraphType(3);
        g[0].pos = {{0, 0, 0}};
        g[1].pos = {{1, 1, 0}};
        g[2].pos = {{0, -1, 0}};

        SG::SpatialEdge se01_1;
        se01_1.edge_points.insert(std::end(se01_1.edge_points),
                                {{1,0,0}});
        add_edge(0, 1, se01_1, g);
        SG::SpatialEdge se01_2;
        se01_2.edge_points.insert(std::end(se01_2.edge_points),
                                {{0,1,0}});
        add_edge(0, 1, se01_2, g);

        add_edge(0,2, g);
    }
};

/**
 * Spatial Graph
 *     o
 *     |
 *     o
 */
struct sg_one_edge : public SpatialGraphBaseFixture {
    sg_one_edge() {
        using boost::add_edge;
        this->g = GraphType(2);
        // Add edge with an associated SpatialEdge at construction.
        SG::PointType n3{{0, 3, 0}};
        SG::PointType n2{{0, 2, 0}};
        SG::PointType n1{{0, 1, 0}};
        SG::PointType p0{{0, 0, 0}};
        SG::PointType s1{{0, -1, 0}};
        SG::PointType s2{{0, -2, 0}};
        SG::PointType s3{{0, -3, 0}};
        g[0].pos = n3;
        g[1].pos = s3;

        SG::SpatialEdge se01;
        se01.edge_points.insert(std::end(se01.edge_points),
                                {n2, n1, p0, s1, s2});
        add_edge(0, 1, se01, g);
    }
};

/**
 * Spatial Graph from easy_graph
 *     o
 *     |
 *      o-o
 *     |
 *     o
 */
struct sg_easy :
    public SpatialGraphBaseFixture {
    sg_easy() {
        using boost::add_edge;
        this->g = GraphType(4);
        // Add edge with an associated SpatialEdge at construction.
        SG::PointType n3{{0, 3, 0}};
        SG::PointType n2{{0, 2, 0}};
        SG::PointType n1{{0, 1, 0}};
        SG::PointType p0{{1, 0, 0}};
        SG::PointType e1{{2, 0, 0}};
        SG::PointType e2{{3, 0, 0}};
        SG::PointType s1{{0, -1, 0}};
        SG::PointType s2{{0, -2, 0}};
        SG::PointType s3{{0, -3, 0}};

        g[0].pos = n3;
        g[1].pos = p0;
        g[2].pos = s3;
        g[3].pos = e2;

        SG::SpatialEdge se01;
        se01.edge_points.insert(std::end(se01.edge_points), {n1, n2});
        add_edge(0, 1, se01, g);
        SG::SpatialEdge se12;
        se12.edge_points.insert(std::end(se12.edge_points), {s1, s2});
        add_edge(1, 2, se12, g);
        SG::SpatialEdge se13;
        se13.edge_points.insert(std::end(se13.edge_points), {e1});
        add_edge(1, 3, se13, g);
    }
};

/**
 * Spatial Graph.
 *     o
 *     |
 *     o-o
 *     |
 *     o
 *  extra_connected_junctions reduces to this.
 */
struct sg_easy_centered : public SpatialGraphBaseFixture {
    sg_easy_centered() {
        using boost::add_edge;
        this->g = GraphType(4);
        // Add edge with an associated SpatialEdge at construction.
        SG::PointType n3{{0, 3, 0}};
        SG::PointType n2{{0, 2, 0}};
        SG::PointType n1{{0, 1, 0}};
        SG::PointType p0{{0, 0, 0}};
        SG::PointType e1{{1, 0, 0}};
        SG::PointType e2{{2, 0, 0}};
        SG::PointType s1{{0, -1, 0}};
        SG::PointType s2{{0, -2, 0}};
        SG::PointType s3{{0, -3, 0}};

        g[0].pos = n3;
        g[1].pos = p0;
        g[2].pos = s3;
        g[3].pos = e2;

        SG::SpatialEdge se01;
        se01.edge_points.insert(std::end(se01.edge_points), {n1, n2});
        add_edge(0, 1, se01, g);
        SG::SpatialEdge se12;
        se12.edge_points.insert(std::end(se12.edge_points), {s1, s2});
        add_edge(1, 2, se12, g);
        SG::SpatialEdge se13;
        se13.edge_points.insert(std::end(se13.edge_points), {e1});
        add_edge(1, 3, se13, g);
    }
};

/**
 * There are extra edges in the intersection.
 * o
 * |o
 * o
 * |\
 * o-o-o
 * |/
 * o
 * |o
 * o
 * Ideally we want to remove the diagonal edges in the sg and its nodes.
 * We want to keep the center node, and remove all the other 3,
 * which are really not nodes, but edge_points of the edges going to the center
 * node. All 5 sg_edges have no edge points.
 */
struct sg_extra_connected_junctions : public SpatialGraphBaseFixture {
    sg_extra_connected_junctions() {
        using boost::add_edge;
        this->g = GraphType(6);
        // Add edge with an associated SpatialEdge at construction.
        SG::PointType n3{{0, 3, 0}};
        SG::PointType n2{{0, 2, 0}};
        SG::PointType n1{{0, 1, 0}};
        SG::PointType p0{{0, 0, 0}};
        SG::PointType e1{{1, 0, 0}};
        SG::PointType e2{{2, 0, 0}};
        SG::PointType s1{{0, -1, 0}};
        SG::PointType s2{{0, -2, 0}};
        SG::PointType s3{{0, -3, 0}};

        g[0].pos = n3;
        g[1].pos = n1;
        g[2].pos = p0;
        g[3].pos = s1;
        g[4].pos = s3;
        g[5].pos = e1;
        g[6].pos = e2;

        // North
        SG::SpatialEdge se01;
        se01.edge_points.insert(std::end(se01.edge_points), {n2});
        add_edge(0, 1, se01, g);
        // South
        SG::SpatialEdge se34;
        se34.edge_points.insert(std::end(se34.edge_points), {s2});
        add_edge(3, 4, se34, g);
        // East
        SG::SpatialEdge se56;
        add_edge(5, 6, se56, g);

        // North-Center
        SG::SpatialEdge se21;
        add_edge(2, 1, se21, g);
        // South-Center
        SG::SpatialEdge se23;
        add_edge(2, 3, se23, g);
        // East-Center
        SG::SpatialEdge se25;
        add_edge(2, 5, se25, g);
        // North-East
        SG::SpatialEdge se15;
        add_edge(1, 5, se15, g);
        // South-East
        SG::SpatialEdge se35;
        add_edge(3, 5, se35, g);
    }
};

/**
 * Common typedefs for objects/graphs
 */
struct object_graph : public ::testing::Test {
    using Domain = DGtal::Z3i::Domain;
    using DigitalTopology = DGtal::Z3i::DT26_6;
    using DigitalSet = DGtal::DigitalSetByAssociativeContainer<
        Domain, std::unordered_set<typename Domain::Point>>;
    using Object = DGtal::Object<DigitalTopology, DigitalSet>;
    Object obj;
    using vertex_iterator =
        typename boost::graph_traits<Object>::vertex_iterator;
    using edge_iterator = typename boost::graph_traits<Object>::edge_iterator;
    void print_degrees() {
        std::cout << "Print degrees obj:" << std::endl;
        std::cout << "Num Vertices: " << boost::num_vertices(obj) << std::endl;
        vertex_iterator vi, vi_end;
        std::tie(vi, vi_end) = boost::vertices(obj);
        for (; vi != vi_end; ++vi) {
            std::cout << *vi << ": " << boost::out_degree(*vi, obj)
                      << std::endl;
        }
    }
    void print_edges() {
        std::cout << "Print edges obj:" << std::endl;
        std::cout << "Num Edges: " << boost::num_edges(obj) << std::endl;
        edge_iterator ei, ei_end;
        std::tie(ei, ei_end) = boost::edges(obj);
        for (; ei != ei_end; ++ei) {
            std::cout << boost::source(*ei, obj) << "---"
                      << boost::target(*ei, obj) << std::endl;
        }
    }
};

struct one_edge : public object_graph {

    void SetUp() override {
        DigitalTopology::ForegroundAdjacency adjF;
        DigitalTopology::BackgroundAdjacency adjB;
        DigitalTopology topo(adjF, adjB,
                             DGtal::DigitalTopologyProperties::JORDAN_DT);

        Domain::Point b1(-10, -10, -10);
        Domain::Point b2(10, 10, 10);
        Domain domain(b1, b2);
        DigitalSet obj_set(domain);
        Domain::Point n3(0, 3, 0);
        Domain::Point n2(0, 2, 0);
        Domain::Point n1(0, 1, 0);
        Domain::Point p0(0, 0, 0);
        Domain::Point s1(0, -1, 0);
        Domain::Point s2(0, -2, 0);
        Domain::Point s3(0, -3, 0);
        obj_set.insertNew(n3);
        obj_set.insertNew(n2);
        obj_set.insertNew(n1);
        obj_set.insertNew(p0);
        obj_set.insertNew(s1);
        obj_set.insertNew(s2);
        obj_set.insertNew(s3);
        this->obj = Object(topo, obj_set);
    }
};
/**
 * o: nodes
 * |,-: regular edges
 * The easy graph, with all the adjacent nodes connected with edges.
 *     o
 *     o
 *     o
 *      ooo
 *     o
 *     o
 *     o
 */
struct easy : public object_graph {

    void SetUp() override {
        DigitalTopology::ForegroundAdjacency adjF;
        DigitalTopology::BackgroundAdjacency adjB;
        DigitalTopology topo(adjF, adjB,
                             DGtal::DigitalTopologyProperties::JORDAN_DT);

        Domain::Point b1(-10, -10, -10);
        Domain::Point b2(10, 10, 10);
        Domain domain(b1, b2);
        DigitalSet obj_set(domain);
        Domain::Point n3(0, 3, 0);
        Domain::Point n2(0, 2, 0);
        Domain::Point n1(0, 1, 0);
        Domain::Point p0(1, 0, 0);
        Domain::Point e1(2, 0, 0);
        Domain::Point e2(3, 0, 0);
        Domain::Point s1(0, -1, 0);
        Domain::Point s2(0, -2, 0);
        Domain::Point s3(0, -3, 0);
        obj_set.insertNew(n3);
        obj_set.insertNew(n2);
        obj_set.insertNew(n1);
        obj_set.insertNew(p0);
        obj_set.insertNew(s1);
        obj_set.insertNew(s2);
        obj_set.insertNew(s3);
        obj_set.insertNew(e1);
        obj_set.insertNew(e2);
        this->obj = Object(topo, obj_set);
    }
};

/**
 * o: nodes
 * |,-: regular edges
 * The easy graph, with all the adjacent nodes connected with edges.
 *     o
 *     o
 *     o
 *     ooo
 *     o
 *     o
 *     o
 * Note that the junctions are not only one node. Following 4 nodes are
 * connected between them:
 *  o
 *  oo
 *  o
 *  Ideally, we would like to remove edges except from the junction node.
 */
struct extra_connected_junctions : public object_graph {

    void SetUp() override {
        DigitalTopology::ForegroundAdjacency adjF;
        DigitalTopology::BackgroundAdjacency adjB;
        DigitalTopology topo(adjF, adjB,
                             DGtal::DigitalTopologyProperties::JORDAN_DT);

        Domain::Point b1(-10, -10, -10);
        Domain::Point b2(10, 10, 10);
        Domain domain(b1, b2);
        DigitalSet obj_set(domain);
        Domain::Point n3(0, 3, 0);
        Domain::Point n2(0, 2, 0);
        Domain::Point n1(0, 1, 0);
        Domain::Point p0(0, 0, 0);
        Domain::Point e1(1, 0, 0);
        Domain::Point e2(2, 0, 0);
        Domain::Point s1(0, -1, 0);
        Domain::Point s2(0, -2, 0);
        Domain::Point s3(0, -3, 0);
        obj_set.insertNew(n3);
        obj_set.insertNew(n2);
        obj_set.insertNew(n1);
        obj_set.insertNew(p0);
        obj_set.insertNew(s1);
        obj_set.insertNew(s2);
        obj_set.insertNew(s3);
        obj_set.insertNew(e1);
        obj_set.insertNew(e2);
        this->obj = Object(topo, obj_set);
    }
};

// Helpers
SG::PointContainer all_vertex_positions(const SpatialGraphBaseFixture::GraphType &lhs_g) {
    SG::PointContainer lhs_vertex_points;
    for (auto vp = boost::vertices(lhs_g); vp.first != vp.second; ++vp.first) {
        lhs_vertex_points.push_back(lhs_g[*vp.first].pos);
    }
    return lhs_vertex_points;
}

bool equal_vertex_positions(const SpatialGraphBaseFixture::GraphType &lhs_g,
                            const SpatialGraphBaseFixture::GraphType &rhs_g) {
    auto lhs_vertex_points = all_vertex_positions(lhs_g);
    auto rhs_vertex_points = all_vertex_positions(rhs_g);
    std::sort(lhs_vertex_points.begin(), lhs_vertex_points.end());
    std::sort(rhs_vertex_points.begin(), rhs_vertex_points.end());
    return lhs_vertex_points == rhs_vertex_points;
}

SG::PointContainer all_edge_points(const SpatialGraphBaseFixture::GraphType &lhs_g) {
    SG::PointContainer lhs_edge_points;
    for (auto ep = boost::edges(lhs_g); ep.first != ep.second; ++ep.first) {
        auto &se = lhs_g[*ep.first];
        lhs_edge_points.insert(lhs_edge_points.end(), se.edge_points.begin(),
                               se.edge_points.end());
    }
    return lhs_edge_points;
}

void print_point_container(const SG::PointContainer &pc) {
    for (const auto &v : pc) {
        std::cout << "{ " << v[0] << "," << v[1] << ", " << v[2] << " }"
                  << std::endl;
    }
}

bool equal_edge_points(const SpatialGraphBaseFixture::GraphType &lhs_g,
                       const SpatialGraphBaseFixture::GraphType &rhs_g) {
    auto lhs_edge_points = all_edge_points(lhs_g);
    auto rhs_edge_points = all_edge_points(rhs_g);
    std::sort(lhs_edge_points.begin(), lhs_edge_points.end());
    std::sort(rhs_edge_points.begin(), rhs_edge_points.end());
    return lhs_edge_points == rhs_edge_points;
}

TEST_F(one_edge, object_to_sg) {
    std::cout << "Convert one_edge" << std::endl;
    using SpatialGraph = sg_one_edge::GraphType;
    SpatialGraph sg = SG::spatial_graph_from_object<Object, SpatialGraph>(obj);
    EXPECT_EQ(boost::num_vertices(sg), boost::num_vertices(obj));
    EXPECT_EQ(boost::num_edges(sg), boost::num_edges(obj) / 2.0);
    // Dev code:
    // print_point_container(all_vertex_positions(sg));
    // auto edges = boost::edges(sg);
    // for(; edges.first != edges.second ; ++edges.first){
    //     auto source = boost::source(*edges.first, sg);
    //     auto target = boost::target(*edges.first, sg);
    //     std::cout << *edges.first << std::endl;
    //     std::cout << ArrayUtilities::to_string(sg[source].pos) <<
    //         "---" << ArrayUtilities::to_string(sg[target].pos) << std::endl;
    // }
}

TEST_F(extra_connected_junctions, object_to_sg) {
    std::cout << "Convert extra" << std::endl;
    using SpatialGraph = sg_extra_connected_junctions::GraphType;
    SpatialGraph sg = SG::spatial_graph_from_object<Object, SpatialGraph>(obj);
    EXPECT_EQ(boost::num_vertices(sg), boost::num_vertices(obj));
    EXPECT_EQ(boost::num_edges(sg), boost::num_edges(obj) / 2.0);
    // Dev code:
    // print_point_container(all_vertex_positions(sg));
    // auto edges = boost::edges(sg);
    // for(; edges.first != edges.second ; ++edges.first){
    //     auto source = boost::source(*edges.first, sg);
    //     auto target = boost::target(*edges.first, sg);
    //     std::cout << *edges.first << std::endl;
    //     std::cout << ArrayUtilities::to_string(sg[source].pos) <<
    //         "---" << ArrayUtilities::to_string(sg[target].pos) << std::endl;
    // }
}

TEST_F(one_edge, reduce) {
    std::cout << "One Edge" << std::endl;
    auto nverts = boost::num_vertices(obj);
    EXPECT_EQ(nverts, obj.size());
    // std::cout << "V: " << nverts << std::endl;
    auto nedges = boost::num_edges(obj);
    // std::cout << "E: " << nedges << std::endl;
    EXPECT_EQ(nedges, 12); // Because object has oriented edges

    using SpatialGraph = sg_one_edge::GraphType;
    SpatialGraph sg = SG::spatial_graph_from_object<Object, SpatialGraph>(obj);
    SpatialGraph reduced_g = SG::reduce_spatial_graph_via_dfs(sg);
    SpatialGraph expected_g = sg_one_edge().g;
    EXPECT_EQ(num_vertices(reduced_g), num_vertices(expected_g));
    EXPECT_EQ(num_edges(reduced_g), num_edges(expected_g));
    EXPECT_EQ(equal_vertex_positions(reduced_g, expected_g), true);
    EXPECT_EQ(equal_edge_points(reduced_g, expected_g), true);
}

TEST_F(easy, reduce) {
    std::cout << "Easy Graph" << std::endl;
    auto nverts = boost::num_vertices(obj);
    // std::cout << "V: " << nverts << std::endl;
    EXPECT_EQ(nverts, obj.size());
    auto nedges = boost::num_edges(obj);
    // std::cout << "E: " << nedges << std::endl;
    EXPECT_EQ(nedges, 16);
    // print_degrees();
    using SpatialGraph = sg_easy::GraphType;
    SpatialGraph sg = SG::spatial_graph_from_object<Object, SpatialGraph>(obj);
    SpatialGraph reduced_g = SG::reduce_spatial_graph_via_dfs(sg);
    SpatialGraph expected_g = sg_easy().g;

    EXPECT_EQ(num_vertices(reduced_g), num_vertices(expected_g));
    EXPECT_EQ(num_edges(reduced_g), num_edges(expected_g));
    EXPECT_EQ(equal_vertex_positions(reduced_g, expected_g), true);
    EXPECT_EQ(equal_edge_points(reduced_g, expected_g), true);
    // Dev code:
    // auto reduced_edge_points = all_edge_points(reduced_g);
    // auto expected_edge_points = all_edge_points(expected_g);
    // EXPECT_EQ(reduced_edge_points.size(), expected_edge_points.size());
    // std::cout << "Reduced edge points" << std::endl;
    // print_point_container(reduced_edge_points);
    // std::cout << "Expected edge points" << std::endl;
    // print_point_container(expected_edge_points);
}

TEST_F(extra_connected_junctions, reduce) {
    std::cout << "Extra Connected Junctions Graph" << std::endl;
    auto nverts = boost::num_vertices(obj);
    // std::cout << "V: " << nverts << std::endl;
    EXPECT_EQ(nverts, obj.size());
    auto nedges = boost::num_edges(obj);
    // std::cout << "E: " << nedges << std::endl;
    EXPECT_EQ(nedges, 20);
    // print_degrees();
    using SpatialGraph = sg_extra_connected_junctions::GraphType;
    SpatialGraph sg = SG::spatial_graph_from_object<Object, SpatialGraph>(obj);
    SpatialGraph reduced_g = SG::reduce_spatial_graph_via_dfs(sg);
    SpatialGraph expected_g = sg_extra_connected_junctions().g;

    EXPECT_EQ(num_vertices(reduced_g), num_vertices(expected_g));
    EXPECT_EQ(num_edges(reduced_g), num_edges(expected_g));

    EXPECT_EQ(equal_vertex_positions(reduced_g, expected_g), true);
    EXPECT_EQ(equal_edge_points(reduced_g, expected_g), true);
    //// Extra checking if failure:
    // auto reduced_edge_points = all_edge_points(reduced_g);
    // auto expected_edge_points = all_edge_points(expected_g);
    // EXPECT_EQ(reduced_edge_points.size(), expected_edge_points.size());
    // std::cout << "Reduced edge points" << std::endl;
    // print_point_container(reduced_edge_points);
    // std::cout << "Expected edge points" << std::endl;
    // print_point_container(expected_edge_points);
    // SG::print_edges(reduced_g);
}

TEST_F(extra_connected_junctions, remove_extra_edges) {
    std::cout << "Remove extra edges" << std::endl;
    using SpatialGraph = sg_extra_connected_junctions::GraphType;
    SpatialGraph sg = SG::spatial_graph_from_object<Object, SpatialGraph>(obj);
    remove_extra_edges(sg);
    SpatialGraph reduced_g = SG::reduce_spatial_graph_via_dfs(sg);
    sg_extra_connected_junctions::GraphType expected_g = sg_easy_centered().g;
    EXPECT_EQ(num_vertices(reduced_g), num_vertices(expected_g));
    EXPECT_EQ(num_edges(reduced_g), num_edges(expected_g));
    EXPECT_EQ(equal_vertex_positions(reduced_g, expected_g), true);
    EXPECT_EQ(equal_edge_points(reduced_g, expected_g), true);
}

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
TEST_F(three_connected_nodes, remove_extra_edges) {
    std::cout << "Three connected nodes" << std::endl;
    using SpatialGraph = SpatialGraphBaseFixture::GraphType;
    SpatialGraph sg = SG::spatial_graph_from_object<Object, SpatialGraph>(obj);
    SpatialGraphBaseFixture::vertex_iterator vi, vi_end;
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
    EXPECT_EQ(num_vertices(sg), 6);
    EXPECT_EQ(num_edges(sg), 6);
    EXPECT_EQ(count3degrees, 3);
    EXPECT_EQ(count2degrees, 0);
    EXPECT_EQ(count1degrees, 3);
    bool any_edge_removed = remove_extra_edges(sg);
    EXPECT_FALSE(any_edge_removed);
    EXPECT_EQ(num_vertices(sg), 6);
    EXPECT_EQ(num_edges(sg), 6);
    SpatialGraph reduced_g = SG::reduce_spatial_graph_via_dfs(sg);
    EXPECT_EQ(num_vertices(reduced_g), num_vertices(sg));
    EXPECT_EQ(num_edges(reduced_g), num_edges(sg));
    SG::print_degrees(reduced_g);
    SG::print_edges(reduced_g);
}
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
struct three_connected_nodes_with_self_loop : public object_graph {
    three_connected_nodes_with_self_loop(){
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
        // Extend r0 and r1 to create a connection between n0 and n1
        Domain::Point l1(-2, 0, 0);
        Domain::Point l2(-3, 0, 0);
        Domain::Point l3(-3, 1, 0);
        Domain::Point l4(-3, 2, 0);
        Domain::Point l5(-3, 3, 0);
        Domain::Point l6(-2, 3, 0);
        Domain::Point l7(-1, 3, 0);
        Domain::Point l8(0, 3, 0);

        obj_set.insertNew(n0);
        obj_set.insertNew(n1);
        obj_set.insertNew(n2);
        obj_set.insertNew(r0);
        obj_set.insertNew(r1);
        obj_set.insertNew(r2);
        obj_set.insertNew(l1);
        obj_set.insertNew(l2);
        obj_set.insertNew(l3);
        obj_set.insertNew(l4);
        obj_set.insertNew(l5);
        obj_set.insertNew(l6);
        obj_set.insertNew(l7);
        obj_set.insertNew(l8);
        this->obj = Object(topo, obj_set);
    }
};
TEST_F(three_connected_nodes_with_self_loop, remove_extra_edges) {
    std::cout << "Three connected nodes_with_self_loop" << std::endl;
    using SpatialGraph = SpatialGraphBaseFixture::GraphType;
    SpatialGraph sg = SG::spatial_graph_from_object<Object, SpatialGraph>(obj);
    SG::print_degrees(sg);
    SG::print_edges(sg);
    EXPECT_EQ(num_vertices(sg), 14);
    EXPECT_EQ(num_edges(sg), 17); // Haven't really thought about his, but should be ok.
    bool any_edge_removed = remove_extra_edges(sg);
    EXPECT_EQ(any_edge_removed, true);
    EXPECT_EQ(num_vertices(sg), 14);
    EXPECT_EQ(num_edges(sg), 15); // 2 edges removed
    SpatialGraph reduced_g = SG::reduce_spatial_graph_via_dfs(sg);
    EXPECT_EQ(num_vertices(reduced_g), 4);
    EXPECT_EQ(num_edges(reduced_g), 5);
    SpatialGraphBaseFixture::vertex_iterator vi, vi_end;
    std::tie(vi, vi_end) = boost::vertices(reduced_g);
    size_t count1degrees = 0;
    size_t count2degrees = 0;
    size_t count3degrees = 0;
    for (; vi != vi_end; ++vi) {
        auto degree = boost::out_degree(*vi, reduced_g);
        if (degree == 1)
            count1degrees++;
        if (degree == 2)
            count2degrees++;
        if (degree == 3)
            count3degrees++;
    }
    EXPECT_EQ(count3degrees, 3);
    EXPECT_EQ(count2degrees, 0);
    EXPECT_EQ(count1degrees, 1);
    SG::print_spatial_edges(reduced_g);
}


TEST_F(sg_square, reduce_graph) {
    std::cout << "Reduce graph - square" << std::endl;
    using SpatialGraph = SpatialGraphBaseFixture::GraphType;
    auto &sg = g;
    auto reduced_g = SG::reduce_spatial_graph_via_dfs(sg);
    EXPECT_EQ(num_vertices(reduced_g), 2);
    EXPECT_EQ(num_edges(reduced_g), 2);
    SG::print_degrees(reduced_g);
    SG::print_edges(reduced_g);
    // Expected:
    SpatialGraph expected_g = sg_square_expected().g;
    EXPECT_EQ(equal_vertex_positions(reduced_g, expected_g), true);
    EXPECT_EQ(equal_edge_points(reduced_g, expected_g), true);
}

TEST_F(sg_square_plus_one, reduce_graph) {
    std::cout << "Reduce graph - square plus one" << std::endl;
    using SpatialGraph = SpatialGraphBaseFixture::GraphType;
    auto &sg = g;
    auto reduced_g = SG::reduce_spatial_graph_via_dfs(sg);
    EXPECT_EQ(num_vertices(reduced_g), 3);
    EXPECT_EQ(num_edges(reduced_g), 3);
    SG::print_degrees(reduced_g);
    SG::print_edges(reduced_g);
    // Expected:
    SpatialGraph expected_g = sg_square_plus_one_expected().g;
    EXPECT_EQ(equal_vertex_positions(reduced_g, expected_g), true);
    EXPECT_EQ(equal_edge_points(reduced_g, expected_g), true);
}

/**
 * In 3D, structure that  gives more nodes than expected.
 * Domain::Point p0(0, 0, 0);
 * Domain::Point p1(1, 0, 0);
 * Domain::Point p2(0, 1, 0);
 * Domain::Point p3(-1, -1, 0);
 * Domain::Point n0(1, 2, 1);
 *
 */
struct debug_one : public object_graph {
    debug_one(){
        DigitalTopology::ForegroundAdjacency adjF;
        DigitalTopology::BackgroundAdjacency adjB;
        DigitalTopology topo(adjF, adjB,
                DGtal::DigitalTopologyProperties::JORDAN_DT);

        Domain::Point b1(-10, -10, -10);
        Domain::Point b2(10, 10, 10);
        Domain domain(b1, b2);
        DigitalSet obj_set(domain);
        Domain::Point p0(0, 0, 0);
        Domain::Point p1(1, 0, 0);
        Domain::Point p2(0, 1, 0);
        Domain::Point p3(-1, -1, 0);
        Domain::Point n0(1, 2, 1);
        obj_set.insertNew(p0);
        obj_set.insertNew(p1);
        obj_set.insertNew(p2);
        obj_set.insertNew(p3);
        obj_set.insertNew(n0);
        this->obj = Object(topo, obj_set);
    }
};
TEST_F(debug_one, remove_extra_edges) {
    std::cout << "debug_one" << std::endl;
    using SpatialGraph = SpatialGraphBaseFixture::GraphType;
    SpatialGraph sg = SG::spatial_graph_from_object<Object, SpatialGraph>(obj);
    SpatialGraphBaseFixture::vertex_iterator vi, vi_end;
    std::cout << "Original" << std::endl;
    SG::print_degrees(sg);
    SG::print_edges(sg);
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
    EXPECT_EQ(num_vertices(sg), 5);
    EXPECT_EQ(num_edges(sg), 5);
    EXPECT_EQ(count3degrees, 2);
    EXPECT_EQ(count2degrees, 1);
    EXPECT_EQ(count1degrees, 2);
    bool any_edge_removed = remove_extra_edges(sg);
    EXPECT_EQ(any_edge_removed, true);
    std::cout << "After removal of extra edges" << std::endl;
    SG::print_degrees(sg);
    SG::print_edges(sg);
    EXPECT_EQ(num_vertices(sg), 5);
    EXPECT_EQ(num_edges(sg), 4);
    SpatialGraph reduced_g = SG::reduce_spatial_graph_via_dfs(sg);
    std::cout << "Reduced" << std::endl;
    SG::print_degrees(reduced_g);
    SG::print_spatial_edges(reduced_g);
    EXPECT_EQ(num_vertices(reduced_g), 4);
    EXPECT_EQ(num_edges(reduced_g), 3);
}

 /**
 * In 3D, structure that  gives more nodes than expected.
 * Domain::Point p0(0, 0, 0);
 * Domain::Point p1(1, 1, 0);
 * Domain::Point p2(0, 1, 1);
 * Domain::Point r0(-1, -1, 0);
 * Domain::Point r1(2, 2, 0);
 * Domain::Point r2(0, 1, 2);
 *
 */
struct rare_trio : public object_graph {
    rare_trio(){
        DigitalTopology::ForegroundAdjacency adjF;
        DigitalTopology::BackgroundAdjacency adjB;
        DigitalTopology topo(adjF, adjB,
                DGtal::DigitalTopologyProperties::JORDAN_DT);

        Domain::Point b1(-10, -10, -10);
        Domain::Point b2(10, 10, 10);
        Domain domain(b1, b2);
        DigitalSet obj_set(domain);
        Domain::Point p0(0, 0, 0);
        Domain::Point p1(1, 1, 0);
        Domain::Point p2(0, 1, 1);
        // Branches to give them more than degree 2
        Domain::Point r0(-1, -1, 0);
        Domain::Point r1(2, 2, 0);
        Domain::Point r2(0, 1, 2);
        obj_set.insertNew(p0);
        obj_set.insertNew(p1);
        obj_set.insertNew(p2);
        obj_set.insertNew(r0);
        obj_set.insertNew(r1);
        obj_set.insertNew(r2);
        this->obj = Object(topo, obj_set);
    }
};
TEST_F(rare_trio, remove_extra_edges) {
    std::cout << "rare_trio" << std::endl;
    using SpatialGraph = SpatialGraphBaseFixture::GraphType;
    SpatialGraph sg = SG::spatial_graph_from_object<Object, SpatialGraph>(obj);
    SpatialGraphBaseFixture::vertex_iterator vi, vi_end;
    std::cout << "Original" << std::endl;
    SG::print_degrees(sg);
    SG::print_edges(sg);
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
    EXPECT_EQ(num_vertices(sg), 6);
    EXPECT_EQ(num_edges(sg), 6);
    EXPECT_EQ(count3degrees, 3);
    EXPECT_EQ(count2degrees, 0);
    EXPECT_EQ(count1degrees, 3);
    bool any_edge_removed = remove_extra_edges(sg);
    EXPECT_EQ(any_edge_removed, false);
    std::cout << "After removal of extra edges" << std::endl;
    SG::print_degrees(sg);
    SG::print_edges(sg);
    EXPECT_EQ(num_vertices(sg), 6);
    EXPECT_EQ(num_edges(sg), 6);
    SpatialGraph reduced_g = SG::reduce_spatial_graph_via_dfs(sg);
    std::cout << "Reduced" << std::endl;
    SG::print_degrees(reduced_g);
    SG::print_spatial_edges(reduced_g);
    EXPECT_EQ(num_vertices(reduced_g), 6);
    EXPECT_EQ(num_edges(reduced_g), 6);
    // SG::visualize_spatial_graph(reduced_g);
    std::cout << "Merge 3-connected" << std::endl;
    auto nodes_merged = SG::merge_three_connected_nodes(reduced_g);
    std::cout << nodes_merged <<  " nodes were merged. Those nodes have now degree 0" << std::endl;
    EXPECT_EQ(num_vertices(reduced_g), 6);
    EXPECT_EQ(num_edges(reduced_g), 3);
    SG::print_degrees(reduced_g);
    SG::print_spatial_edges(reduced_g);
    // SG::visualize_spatial_graph(reduced_g);
}


/**
 * See image: buggy_structure.png (ignore red cell).
 * {489 398 3} is the cell below the red cell.
 * 19089--60205  [spatial_edge="[{489 398 3}]"];
 * 19089--60205  [spatial_edge="[{489 398 3},{489 398 1}]"];
 * 19089--60205  [spatial_edge="[{489 398 3},{489 398 1}]"];
 * 19089 [spatial_node="489 399 2"];
 * 60205 [spatial_node="488 398 2"];
 *
 * 20059 [spatial_node="485 398 3"];
 * 9069  [spatial_node="494 402 2"];
 * 20059--60205  [spatial_edge="[{486 398 2},{487 398 1}]"];
 * 19089--9069  [spatial_edge="[{489 398 3},{489 398 1},{490 400 1},
 * {491 401 1},{492 401 0},{493 402 1}]"];
 */
struct buggy_structure : public object_graph {
    buggy_structure(){
        DigitalTopology::ForegroundAdjacency adjF;
        DigitalTopology::BackgroundAdjacency adjB;
        DigitalTopology topo(adjF, adjB,
                DGtal::DigitalTopologyProperties::JORDAN_DT);

        Domain::Point b1(480, 390, -1);
        Domain::Point b2(500, 410, 10);
        Domain domain(b1, b2);
        DigitalSet obj_set(domain);
        Domain::Point p0(489, 399, 2); // 19089
        Domain::Point p1(488, 398, 2); // 60205
        Domain::Point p2(485, 398, 3); // 20059
        Domain::Point p3(491, 401, 1); // 9069(reduced)
        Domain::Point p4(491, 401, 3); // other branch
        Domain::Point r01_0(489, 398, 3); // red
        Domain::Point r01_1(489, 398, 1);
        Domain::Point r12_0(486, 398, 2);
        Domain::Point r12_1(487, 398, 1);
        Domain::Point r03_1(490, 400, 1);
        Domain::Point r04_1(490, 400, 3);
        // Branches to give them more than degree 2
        obj_set.insertNew(p0);
        obj_set.insertNew(p1);
        obj_set.insertNew(p2);
        obj_set.insertNew(p3);
        obj_set.insertNew(p4);
        obj_set.insertNew(r01_0);
        obj_set.insertNew(r01_1);
        obj_set.insertNew(r12_0);
        obj_set.insertNew(r12_1);
        obj_set.insertNew(r03_1);
        obj_set.insertNew(r04_1);
        this->obj = Object(topo, obj_set);
    }
};
TEST_F(buggy_structure, remove_extra_edges) {
    std::cout << "buggy_structure" << std::endl;
    using SpatialGraph = SpatialGraphBaseFixture::GraphType;
    SpatialGraph sg = SG::spatial_graph_from_object<Object, SpatialGraph>(obj);
  // {
  //     DGtal::Z3i::KSpace ks;
  //     ks.init(obj.domain().lowerBound(), obj.domain().upperBound(),true);
  //     int argc(1);
  //     char** argv(nullptr);
  //     QApplication app(argc, argv);
  //     DGtal::Viewer3D<> viewer(ks);
  //     viewer.show();
  //
  //     viewer.setFillColor(DGtal::Color(255, 0, 0, 255));
  //     viewer << Domain::Point(489, 398, 3);
  //     viewer.setFillColor(DGtal::Color(255, 255, 255, 255));
  //     viewer << obj.pointSet();
  //
  //     viewer << DGtal::Viewer3D<>::updateDisplay;
  //
  //     app.exec();
  // }
    // SG::visualize_spatial_graph(sg);
    SpatialGraphBaseFixture::vertex_iterator vi, vi_end;
    std::cout << "Original" << std::endl;
    SG::print_degrees(sg);
    SG::print_edges(sg);
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
    EXPECT_EQ(num_vertices(sg), 11);
    EXPECT_EQ(num_edges(sg), 12);
    EXPECT_EQ(count3degrees, 0);
    EXPECT_EQ(count2degrees, 6);
    EXPECT_EQ(count1degrees, 3);
    bool any_edge_removed = remove_extra_edges(sg);
    EXPECT_EQ(any_edge_removed, false);
    std::cout << "Reduced" << std::endl;
    bool verbose = true;
    SpatialGraph reduced_g = SG::reduce_spatial_graph_via_dfs(sg, verbose);
    // SG::visualize_spatial_graph(reduced_g);
    SG::print_degrees(reduced_g);
    SG::print_spatial_edges(reduced_g);
    // {
    //     DGtal::Z3i::KSpace ks;
    //     ks.init(obj.domain().lowerBound(), obj.domain().upperBound(),true);
    //     int argc(1);
    //     char** argv(nullptr);
    //     QApplication app(argc, argv);
    //     DGtal::Viewer3D<> viewer(ks);
    //     viewer.show();
    //
    //     viewer.setFillColor(DGtal::Color(0, 255, 0, 255));
    //     viewer << Domain::Point(491, 401, 1); // degree 1
    //     viewer << Domain::Point(489, 399, 2); // degree 4
    //     viewer << Domain::Point(485, 398, 3); // 1
    //     viewer << Domain::Point(488, 398, 2); // 3
    //     viewer << Domain::Point(491, 401, 3); // 1
    //     viewer.setFillColor(DGtal::Color(255, 255, 255, 255));
    //     viewer << obj.pointSet();
    //
    //     viewer << DGtal::Viewer3D<>::updateDisplay;
    //
    //     app.exec();
    // }
    EXPECT_EQ(num_vertices(reduced_g), 5);
    EXPECT_EQ(num_edges(reduced_g), 5);
    std::vector<SG::PointType> all_edge_points;
    for (auto edge_pair = boost::edges(reduced_g); edge_pair.first != edge_pair.second; ++edge_pair.first) {
        const auto & eps = reduced_g[*edge_pair.first].edge_points;
        all_edge_points.insert(std::end(all_edge_points),
                std::begin(eps), std::end(eps));
    }
    std::sort(std::begin(all_edge_points), std::end(all_edge_points));
    std::vector<SG::PointType> expected_edge_points = {
        { 486, 398, 2 }, { 487, 398, 1 }, { 489, 398, 1 },
        { 489, 398, 3 }, { 490, 400, 1 }, { 490, 400, 3 }
    };
    std::sort(std::begin(expected_edge_points), std::end(expected_edge_points));
    EXPECT_EQ(all_edge_points, expected_edge_points);
    EXPECT_EQ(all_edge_points.size() + num_vertices(reduced_g), 11);

    // SG::visualize_spatial_graph(reduced_g);
    std::cout << "Merge 3-connected" << std::endl;
    auto nodes_merged = SG::merge_three_connected_nodes(reduced_g);
    std::cout << nodes_merged <<  " nodes were merged. Those nodes have now degree 0" << std::endl;
    EXPECT_EQ(nodes_merged, 0 );
}
