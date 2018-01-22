#include "DGtal/graph/ObjectBoostGraphInterface.h"
#include "catch_header.h"
#include "spatial_graph.hpp"
#include <DGtal/base/Common.h>
#include <DGtal/helpers/StdDefs.h>
#include <DGtal/topology/Object.h>
#include <iostream>

#include "reduce_dfs_visitor.hpp"
#include "spatial_graph_from_object.hpp"

struct spatial_graph {
    using GraphType = SG::GraphAL;
    GraphType g;
    using vertex_iterator =
        typename boost::graph_traits<GraphType>::vertex_iterator;
    using edge_iterator =
        typename boost::graph_traits<GraphType>::edge_iterator;
};
void print_pos(std::ostream &out, const SG::SpatialNode::Point &pos) {
    out << "{";
    for (auto &p : pos) {
        if (p >= 0)
            out << " ";
        out << p << ",";
    }
    out << "}";
}

void print_degrees(const spatial_graph::GraphType &graph) {
    std::cout << "Print degrees spatial_graph:" << std::endl;
    std::cout << "Num Vertices: " << boost::num_vertices(graph) << std::endl;
    spatial_graph::vertex_iterator vi, vi_end;
    std::tie(vi, vi_end) = boost::vertices(graph);
    for (; vi != vi_end; ++vi) {
        std::cout << *vi << ": " << ArrayUtilities::to_string(graph[*vi].pos) <<
            ". Degree: " << boost::out_degree(*vi, graph) << std::endl;
    }
}
void print_edges(const spatial_graph::GraphType &graph) {
    std::cout << "Print edges spatial_graph:" << std::endl;
    std::cout << "Num Edges: " << boost::num_edges(graph) << std::endl;
    spatial_graph::edge_iterator ei, ei_end;
    std::tie(ei, ei_end) = boost::edges(graph);
    for (; ei != ei_end; ++ei) {
        auto source = boost::source(*ei, graph);
        auto target = boost::target(*ei, graph);
        print_pos(std::cout, graph[source].pos);
        std::cout << "---";
        print_pos(std::cout, graph[target].pos);
        std::cout << std::endl;
    }
}
/**
 * Spatial Graph
 *
 * o-o
 * | |
 * o-o
 */
struct sg_square : public spatial_graph {
    sg_square() {
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

struct sg_square_expected : public spatial_graph {
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
struct sg_square_plus_one : public spatial_graph {
    sg_square_plus_one() {
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

struct sg_square_plus_one_expected : public spatial_graph {
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
struct sg_one_edge : public spatial_graph {
    sg_one_edge() {
        using boost::add_edge;
        this->g = GraphType(2);
        // Add edge with an associated SpatialEdge at construction.
        SG::Point n3{{0, 3, 0}};
        SG::Point n2{{0, 2, 0}};
        SG::Point n1{{0, 1, 0}};
        SG::Point p0{{0, 0, 0}};
        SG::Point s1{{0, -1, 0}};
        SG::Point s2{{0, -2, 0}};
        SG::Point s3{{0, -3, 0}};
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
struct sg_easy : public spatial_graph {
    sg_easy() {
        using boost::add_edge;
        this->g = GraphType(4);
        // Add edge with an associated SpatialEdge at construction.
        SG::Point n3{{0, 3, 0}};
        SG::Point n2{{0, 2, 0}};
        SG::Point n1{{0, 1, 0}};
        SG::Point p0{{1, 0, 0}};
        SG::Point e1{{2, 0, 0}};
        SG::Point e2{{3, 0, 0}};
        SG::Point s1{{0, -1, 0}};
        SG::Point s2{{0, -2, 0}};
        SG::Point s3{{0, -3, 0}};

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
struct sg_easy_centered : public spatial_graph {
    sg_easy_centered() {
        using boost::add_edge;
        this->g = GraphType(4);
        // Add edge with an associated SpatialEdge at construction.
        SG::Point n3{{0, 3, 0}};
        SG::Point n2{{0, 2, 0}};
        SG::Point n1{{0, 1, 0}};
        SG::Point p0{{0, 0, 0}};
        SG::Point e1{{1, 0, 0}};
        SG::Point e2{{2, 0, 0}};
        SG::Point s1{{0, -1, 0}};
        SG::Point s2{{0, -2, 0}};
        SG::Point s3{{0, -3, 0}};

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
struct sg_extra_connected_junctions : public spatial_graph {
    sg_extra_connected_junctions() {
        using boost::add_edge;
        this->g = GraphType(6);
        // Add edge with an associated SpatialEdge at construction.
        SG::Point n3{{0, 3, 0}};
        SG::Point n2{{0, 2, 0}};
        SG::Point n1{{0, 1, 0}};
        SG::Point p0{{0, 0, 0}};
        SG::Point e1{{1, 0, 0}};
        SG::Point e2{{2, 0, 0}};
        SG::Point s1{{0, -1, 0}};
        SG::Point s2{{0, -2, 0}};
        SG::Point s3{{0, -3, 0}};

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
struct object_graph {
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

    one_edge() {
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

    easy() {
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

    extra_connected_junctions() {
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
SG::PointContainer all_vertex_positions(const spatial_graph::GraphType &lhs_g) {
    SG::PointContainer lhs_vertex_points;
    for (auto vp = boost::vertices(lhs_g); vp.first != vp.second; ++vp.first) {
        lhs_vertex_points.push_back(lhs_g[*vp.first].pos);
    }
    return lhs_vertex_points;
}

bool equal_vertex_positions(const spatial_graph::GraphType &lhs_g,
                            const spatial_graph::GraphType &rhs_g) {
    auto lhs_vertex_points = all_vertex_positions(lhs_g);
    auto rhs_vertex_points = all_vertex_positions(rhs_g);
    std::sort(lhs_vertex_points.begin(), lhs_vertex_points.end());
    std::sort(rhs_vertex_points.begin(), rhs_vertex_points.end());
    return lhs_vertex_points == rhs_vertex_points;
}

SG::PointContainer all_edge_points(const spatial_graph::GraphType &lhs_g) {
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

bool equal_edge_points(const spatial_graph::GraphType &lhs_g,
                       const spatial_graph::GraphType &rhs_g) {
    auto lhs_edge_points = all_edge_points(lhs_g);
    auto rhs_edge_points = all_edge_points(rhs_g);
    std::sort(lhs_edge_points.begin(), lhs_edge_points.end());
    std::sort(rhs_edge_points.begin(), rhs_edge_points.end());
    return lhs_edge_points == rhs_edge_points;
}

TEST_CASE_METHOD(one_edge, "Convert one_edge obj to spatial graph",
                 "[convert]") {
    std::cout << "Convert one_edge" << std::endl;
    using SpatialGraph = sg_one_edge::GraphType;
    SpatialGraph sg = SG::spatial_graph_from_object<Object, SpatialGraph>(obj);
    CHECK(boost::num_vertices(sg) == boost::num_vertices(obj));
    CHECK(boost::num_edges(sg) == boost::num_edges(obj) / 2.0);
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

TEST_CASE_METHOD(extra_connected_junctions,
                 "Convert extra_connected_junctions obj to spatial graph",
                 "[convert]") {
    std::cout << "Convert extra" << std::endl;
    using SpatialGraph = sg_extra_connected_junctions::GraphType;
    SpatialGraph sg = SG::spatial_graph_from_object<Object, SpatialGraph>(obj);
    CHECK(boost::num_vertices(sg) == boost::num_vertices(obj));
    CHECK(boost::num_edges(sg) == boost::num_edges(obj) / 2.0);
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

TEST_CASE_METHOD(one_edge, "Reduce graph with degrees <=2 to one edge",
                 "[one_edge]") {
    std::cout << "One Edge" << std::endl;
    auto nverts = boost::num_vertices(obj);
    CHECK(nverts == obj.size());
    // std::cout << "V: " << nverts << std::endl;
    auto nedges = boost::num_edges(obj);
    // std::cout << "E: " << nedges << std::endl;
    CHECK(nedges == 12); // Because object has oriented edges

    using SpatialGraph = sg_one_edge::GraphType;
    SpatialGraph sg = SG::spatial_graph_from_object<Object, SpatialGraph>(obj);
    SpatialGraph reduced_g = SG::reduce_spatial_graph_via_dfs<SpatialGraph>(sg);
    SpatialGraph expected_g = sg_one_edge().g;
    CHECK(num_vertices(reduced_g) == num_vertices(expected_g));
    CHECK(num_edges(reduced_g) == num_edges(expected_g));
    CHECK(equal_vertex_positions(reduced_g, expected_g) == true);
    CHECK(equal_edge_points(reduced_g, expected_g) == true);
}

TEST_CASE_METHOD(easy, "Reduce graph with no pitfalls", "[easy]") {
    std::cout << "Easy Graph" << std::endl;
    auto nverts = boost::num_vertices(obj);
    // std::cout << "V: " << nverts << std::endl;
    CHECK(nverts == obj.size());
    auto nedges = boost::num_edges(obj);
    // std::cout << "E: " << nedges << std::endl;
    CHECK(nedges == 16);
    // print_degrees();
    using SpatialGraph = sg_easy::GraphType;
    SpatialGraph sg = SG::spatial_graph_from_object<Object, SpatialGraph>(obj);
    SpatialGraph reduced_g = SG::reduce_spatial_graph_via_dfs<SpatialGraph>(sg);
    SpatialGraph expected_g = sg_easy().g;

    CHECK(num_vertices(reduced_g) == num_vertices(expected_g));
    CHECK(num_edges(reduced_g) == num_edges(expected_g));
    CHECK(equal_vertex_positions(reduced_g, expected_g) == true);
    CHECK(equal_edge_points(reduced_g, expected_g) == true);
    // Dev code:
    // auto reduced_edge_points = all_edge_points(reduced_g);
    // auto expected_edge_points = all_edge_points(expected_g);
    // CHECK(reduced_edge_points.size() == expected_edge_points.size());
    // std::cout << "Reduced edge points" << std::endl;
    // print_point_container(reduced_edge_points);
    // std::cout << "Expected edge points" << std::endl;
    // print_point_container(expected_edge_points);
}

TEST_CASE_METHOD(extra_connected_junctions,
                 "Graph has more junctions than needed, reduction",
                 "[extra_connected_junctions]") {
    std::cout << "Extra Connected Junctions Graph" << std::endl;
    auto nverts = boost::num_vertices(obj);
    // std::cout << "V: " << nverts << std::endl;
    CHECK(nverts == obj.size());
    auto nedges = boost::num_edges(obj);
    // std::cout << "E: " << nedges << std::endl;
    CHECK(nedges == 20);
    // print_degrees();
    using SpatialGraph = sg_extra_connected_junctions::GraphType;
    SpatialGraph sg = SG::spatial_graph_from_object<Object, SpatialGraph>(obj);
    SpatialGraph reduced_g = SG::reduce_spatial_graph_via_dfs<SpatialGraph>(sg);
    SpatialGraph expected_g = sg_extra_connected_junctions().g;

    CHECK(num_vertices(reduced_g) == num_vertices(expected_g));
    CHECK(num_edges(reduced_g) == num_edges(expected_g));

    CHECK(equal_vertex_positions(reduced_g, expected_g) == true);
    CHECK(equal_edge_points(reduced_g, expected_g) == true);
    //// Extra checking if failure:
    // auto reduced_edge_points = all_edge_points(reduced_g);
    // auto expected_edge_points = all_edge_points(expected_g);
    // CHECK(reduced_edge_points.size() == expected_edge_points.size());
    // std::cout << "Reduced edge points" << std::endl;
    // print_point_container(reduced_edge_points);
    // std::cout << "Expected edge points" << std::endl;
    // print_point_container(expected_edge_points);
    // ::print_edges(reduced_g);
}

TEST_CASE_METHOD(extra_connected_junctions,
                 "Graph has more junctions than needed",
                 "[remove_extra_edges]") {
    std::cout << "Remove extra edges" << std::endl;
    using SpatialGraph = sg_extra_connected_junctions::GraphType;
    SpatialGraph sg = SG::spatial_graph_from_object<Object, SpatialGraph>(obj);
    remove_extra_edges(sg);
    SpatialGraph reduced_g = SG::reduce_spatial_graph_via_dfs<SpatialGraph>(sg);
    sg_extra_connected_junctions::GraphType expected_g = sg_easy_centered().g;
    CHECK(num_vertices(reduced_g) == num_vertices(expected_g));
    CHECK(num_edges(reduced_g) == num_edges(expected_g));
    CHECK(equal_vertex_positions(reduced_g, expected_g) == true);
    CHECK(equal_edge_points(reduced_g, expected_g) == true);
}

TEST_CASE("split_loop", "[split_loop]") {
    std::cout << "Split loop" << std::endl;
    using SpatialGraph = spatial_graph::GraphType;
    using vertex_descriptor =
        typename boost::graph_traits<SpatialGraph>::vertex_descriptor;
    using SpatialEdge = typename boost::edge_bundle_type<SpatialGraph>::type;
    using SpatialNode = typename boost::vertex_bundle_type<SpatialGraph>::type;
    SG::Point p0{{0, 0, 0}};
    SG::Point n1{{0, 1, 0}};
    SG::Point n2{{0, 2, 0}};
    SG::Point e1n2{{1, 2, 0}};
    SG::Point e2n2{{2, 2, 0}};
    SG::Point e2n1{{2, 1, 0}};
    SG::Point e2{{2, 0, 0}};
    SG::Point e1{{1, 0, 0}};
    auto sg = SpatialGraph(1);
    vertex_descriptor vertex_id = 0;
    sg[vertex_id].pos = p0;
    SpatialEdge sg_edge;
    sg_edge.edge_points.insert(std::end(sg_edge.edge_points),
                               {n1, n2, e1n2, e2n2, e2n1, e2, e1});
    SG::split_loop(vertex_id, sg_edge, sg);
    CHECK(num_vertices(sg) == 2);
    CHECK(num_edges(sg) == 2);
    CHECK(sg[1].pos == e2n2);
    SpatialEdge::PointContainer expected1 = {{n1, n2, e1n2}};
    std::sort(expected1.begin(), expected1.end());
    SpatialEdge::PointContainer expected2 = {{e1, e2, e2n1}};
    std::sort(expected2.begin(), expected2.end());
    auto edges = boost::edges(sg);
    for (; edges.first != edges.second; ++edges.first) {
        auto &created_sg_edge = sg[*edges.first];
        auto &points = created_sg_edge.edge_points;
        std::sort(points.begin(), points.end());
        bool equal_edge_points = (points == expected1 || points == expected2);
        CHECK(equal_edge_points == true);
    }
}

TEST_CASE_METHOD(sg_square, "Reduce sg_square", "[reduce_graph]") {
    std::cout << "Reduce graph - square" << std::endl;
    using SpatialGraph = spatial_graph::GraphType;
    auto &sg = g;
    auto reduced_g = SG::reduce_spatial_graph_via_dfs<SpatialGraph>(sg);
    CHECK(num_vertices(reduced_g) == 2);
    CHECK(num_edges(reduced_g) == 2);
    ::print_degrees(reduced_g);
    ::print_edges(reduced_g);
    // Expected:
    SpatialGraph expected_g = sg_square_expected().g;
    CHECK(equal_vertex_positions(reduced_g, expected_g) == true);
    CHECK(equal_edge_points(reduced_g, expected_g) == true);
}

TEST_CASE_METHOD(sg_square_plus_one, "Reduce sg_square_plus_one",
                 "[reduce_graph]") {
    std::cout << "Reduce graph - square plus one" << std::endl;
    using SpatialGraph = spatial_graph::GraphType;
    auto &sg = g;
    auto reduced_g = SG::reduce_spatial_graph_via_dfs<SpatialGraph>(sg);
    CHECK(num_vertices(reduced_g) == 3);
    CHECK(num_edges(reduced_g) == 3);
    ::print_degrees(reduced_g);
    ::print_edges(reduced_g);
    // Expected:
    SpatialGraph expected_g = sg_square_plus_one_expected().g;
    CHECK(equal_vertex_positions(reduced_g, expected_g) == true);
    CHECK(equal_edge_points(reduced_g, expected_g) == true);
}
